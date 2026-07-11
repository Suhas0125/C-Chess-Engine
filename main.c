#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "engine/board.h"
#include "engine/fen.h"
#include "ui/renderer.h"
#include "engine/move.h"
#include "engine/movegen.h"
#include "engine/makemove.h"
#include "engine/history.h"
#include "engine/attack.h"
#include "engine/legalmove.h"
#include "engine/gamestate.h"
#include "engine/evaluate.h"
#include "engine/pst.h"
#include "engine/search.h"
#include "engine/debug.h"
#include "engine/perft.h"
#include "engine/tests.h"
#include "engine/zobrist.h"
#include "engine/tt.h"
#include "engine/uci.h"
#include "engine/time_utils.h"

Renderer renderer;

int main(int argc, char *argv[]){

    // 1. Initialize core systems FIRST (Available to both UCI and Raylib)
    Zobrist_Init();
    TT_Init(64);

    // 2. UCI Mode Check
    if (argc > 1 && strcmp(argv[1], "uci") == 0) {
        UCI_Loop();
        TT_Free();
        return 0;
    }
    
    // 3. Raylib UI Initialization
    InitWindow(800, 800, "C_Chess_Engine");
    // 100 = each square is 100×100 pixels
    // whole board = 800×800 (8 × 100)
    int tileSize = 100; // How big each chess square is in pixels

    Board board;
    Board_FromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // CRITICAL: Ensure the root position has a hash key generated!
    board.hashKey = Zobrist_GenerateKey(&board);

    SetTargetFPS(60);
    InitRenderer(&renderer);
    
    History history;
    History_Init(&history);

    // Currently selected square (-1 means no selection).
    int selectedRow = -1;
    int selectedCol = -1;

    // Stores the legal moves for the currently selected piece.
    MoveList selectedMoves;
    MoveList_Init(&selectedMoves);

    // ... [Everything above this loop remains the same] ...
    
    UIContext uiCtx = {0};
    int rightClickStartRow = -1;
    int rightClickStartCol = -1;

    while(!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        int mouseRow = mousePos.y / tileSize;
        int mouseCol = mousePos.x / tileSize;
        bool validMouse = (mouseRow >= 0 && mouseRow <= 7 && mouseCol >= 0 && mouseCol <= 7);

        uiCtx.mousePos = mousePos;

        // ----------------------------------------------------
        // RIGHT CLICK LOGIC (Highlights and Arrows)
        // ----------------------------------------------------
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && validMouse) {
            rightClickStartRow = mouseRow;
            rightClickStartCol = mouseCol;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && validMouse && rightClickStartRow != -1) {
            if (rightClickStartRow == mouseRow && rightClickStartCol == mouseCol) {
                // Toggle square highlight
                bool found = false;
                for (int i = 0; i < uiCtx.highlightCount; i++) {
                    if (uiCtx.highlights[i].row == mouseRow && uiCtx.highlights[i].col == mouseCol) {
                        uiCtx.highlights[i] = uiCtx.highlights[--uiCtx.highlightCount];
                        found = true; break;
                    }
                }
                if (!found && uiCtx.highlightCount < 64) uiCtx.highlights[uiCtx.highlightCount++] = (UISquare){mouseRow, mouseCol};
            }
            else {
                // Toggle arrow
                bool found = false;
                for (int i = 0; i < uiCtx.arrowCount; i++) {
                    if (uiCtx.arrows[i].fromRow == rightClickStartRow && uiCtx.arrows[i].fromCol == rightClickStartCol &&
                        uiCtx.arrows[i].toRow == mouseRow && uiCtx.arrows[i].toCol == mouseCol) {
                        uiCtx.arrows[i] = uiCtx.arrows[--uiCtx.arrowCount];
                        found = true; break;
                    }
                }
                if (!found && uiCtx.arrowCount < 64) uiCtx.arrows[uiCtx.arrowCount++] = (UIArrow){rightClickStartRow, rightClickStartCol, mouseRow, mouseCol};
            }
            rightClickStartRow = -1;
        }

        // ----------------------------------------------------
        // LEFT CLICK LOGIC (Selection and Dragging)
        // ----------------------------------------------------
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            uiCtx.highlightCount = 0; 
            uiCtx.arrowCount = 0;

            if (validMouse) {
                // Check if clicking a valid move dot
                Move *foundMove = NULL;
                for (int i = 0; i < selectedMoves.count; i++) {
                    if (selectedMoves.moves[i].toRow == mouseRow && selectedMoves.moves[i].toCol == mouseCol) {
                        foundMove = &selectedMoves.moves[i];
                        break;
                    }
                }

                if (foundMove != NULL && !uiCtx.isDragging) {
                    MakeMove(&board, &history, foundMove);
                    uiCtx.lastMove = *foundMove;
                    uiCtx.hasLastMove = true;
                    selectedRow = -1; selectedCol = -1;
                    MoveList_Init(&selectedMoves);
                    goto ENGINE_TURN; 
                } 
                else {
                    // Start dragging piece
                    Piece p = board.squares[mouseRow][mouseCol];
                    if ((board.sideToMove == SIDE_WHITE && IsWhitePiece(p)) || (board.sideToMove == SIDE_BLACK && IsBlackPiece(p))) {
                        selectedRow = mouseRow;
                        selectedCol = mouseCol;
                        uiCtx.isDragging = true;
                        uiCtx.dragRow = mouseRow;
                        uiCtx.dragCol = mouseCol;
                        
                        MoveList legalMoves;
                        MoveList_Init(&legalMoves);
                        GenerateLegalMoves(&board, &history, &legalMoves);
                        MoveList_Init(&selectedMoves);
                        for(int i = 0; i < legalMoves.count; i++) {
                            if (legalMoves.moves[i].fromRow == selectedRow && legalMoves.moves[i].fromCol == selectedCol) {
                                selectedMoves.moves[selectedMoves.count++] = legalMoves.moves[i];
                            }
                        }
                    }
                    else {
                        selectedRow = -1; selectedCol = -1;
                        MoveList_Init(&selectedMoves);
                    }
                }
            }
        }

        // Drop piece logic
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && uiCtx.isDragging) {
            uiCtx.isDragging = false;
            
            // Only trigger move if we dragged to a new square
            if (validMouse && (mouseRow != uiCtx.dragRow || mouseCol != uiCtx.dragCol)) {
                Move *dropMove = NULL;
                for (int i = 0; i < selectedMoves.count; i++) {
                    if (selectedMoves.moves[i].toRow == mouseRow && selectedMoves.moves[i].toCol == mouseCol) {
                        dropMove = &selectedMoves.moves[i];
                        break;
                    }
                }

                if (dropMove != NULL) {
                    MakeMove(&board, &history, dropMove);
                    uiCtx.lastMove = *dropMove;
                    uiCtx.hasLastMove = true;
                    selectedRow = -1; selectedCol = -1;
                    MoveList_Init(&selectedMoves);
                    
                    ENGINE_TURN: // Trigger Engine
                    
                    // Render human move instantly before the engine freezes the thread
                    BeginDrawing();
                    DrawGame(&renderer, &board, -1, -1, &selectedMoves, &uiCtx);
                    EndDrawing();

                    MoveList engineLegalMoves;
                    MoveList_Init(&engineLegalMoves);
                    GenerateLegalMoves(&board, &history, &engineLegalMoves);
                    
                    if (engineLegalMoves.count > 0) {
                        // -1 allottedTimeMs means standard fixed depth search (e.g. depth 5)
                        SearchResult result = SearchBestMove(&board, &history, 5, -1);
                        MakeMove(&board, &history, &result.move);
                        uiCtx.lastMove = result.move;
                        uiCtx.hasLastMove = true;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGame(&renderer, &board, selectedRow, selectedCol, &selectedMoves, &uiCtx);
        EndDrawing();
    }

    // 5. Cleanup at the very end
    UnloadRenderer(&renderer);
    CloseWindow();
    TT_Free(); // Free the TT only when the program is completely finished!

    return 0;
}