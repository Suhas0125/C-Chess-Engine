#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "engine/board.h"
#include "engine/fen.h"
#include "ui/renderer.h"
#include "ui/menu.h"
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

// With real clocks, a hard depth-5 cap would leave time on the table in
// slower controls. Let the clock decide how deep the engine goes instead;
// this matches search.c's own internal MAX_PLY.
#define ENGINE_MAX_DEPTH 64

// Time / 30 + Increment / 2, with a safety floor and ceiling, matching the
// allocation formula already used for wtime/btime in ParseGo() (Phase 7C).
static long long ComputeTimeBudget(long long remainingMs, long long incrementMs) {
    long long budget = remainingMs / 30 + incrementMs / 2;

    if (budget > remainingMs - 50) budget = remainingMs - 50;
    if (budget < 50) budget = 50;

    return budget;
}

// Runs a timed engine search for whichever side is currently to move,
// executes the resulting move, and settles that side's clock (search time
// deducted, increment added). Assumes engineLegalMoves.count > 0.
static void PlayEngineMove(Board *board, History *history, UIContext *uiCtx,
                           long long *whiteTimeMs, long long *blackTimeMs, long long incrementMs) {

    long long *engineClock = (board->sideToMove == SIDE_WHITE) ? whiteTimeMs : blackTimeMs;
    long long budget = ComputeTimeBudget(*engineClock, incrementMs);

    long long searchStart = GetTimeMs();
    SearchResult result = SearchBestMove(board, history, ENGINE_MAX_DEPTH, budget);
    long long elapsedMs = GetTimeMs() - searchStart;

    MakeMove(board, history, &result.move);
    uiCtx->lastMove = result.move;
    uiCtx->hasLastMove = true;

    *engineClock -= elapsedMs;
    *engineClock += incrementMs;
    if (*engineClock < 0) *engineClock = 0;
}

typedef enum {
    APP_MENU,
    APP_PLAYING,
    APP_GAMEOVER
} AppState;

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

    SetTargetFPS(60);
    InitRenderer(&renderer);

    AppState appState = APP_MENU;
    GameSetup setup = {0};
    bool flipped = false;
    const char *gameOverMessage = "";

    Board board;
    History history;

    // Currently selected square (-1 means no selection).
    int selectedRow = -1;
    int selectedCol = -1;

    // Stores the legal moves for the currently selected piece.
    MoveList selectedMoves;
    MoveList_Init(&selectedMoves);

    UIContext uiCtx = {0};
    int rightClickStartRow = -1;
    int rightClickStartCol = -1;

    long long whiteTimeMs = 0;
    long long blackTimeMs = 0;
    long long incrementMs = 0;
    Side humanSide = SIDE_WHITE;

    bool shouldQuit = false;

    while(!WindowShouldClose() && !shouldQuit) {

        // ------------------------------------------------------------
        // MENU STATE
        // ------------------------------------------------------------
        if (appState == APP_MENU) {
            BeginDrawing();
            bool started = DrawMenuFrame(&setup);
            EndDrawing();

            if (started) {
                humanSide = setup.humanSide;
                whiteTimeMs = setup.initialMs;
                blackTimeMs = setup.initialMs;
                incrementMs = setup.incrementMs;
                flipped = (humanSide == SIDE_BLACK);

                Board_FromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                board.hashKey = Zobrist_GenerateKey(&board);
                History_Init(&history);

                selectedRow = -1;
                selectedCol = -1;
                MoveList_Init(&selectedMoves);
                uiCtx = (UIContext){0};
                rightClickStartRow = -1;
                rightClickStartCol = -1;

                appState = APP_PLAYING;

                // If the human plays Black, the engine (White) moves first.
                if (board.sideToMove != humanSide) {
                    PlayEngineMove(&board, &history, &uiCtx, &whiteTimeMs, &blackTimeMs, incrementMs);
                }
            }

            continue;
        }

        // ------------------------------------------------------------
        // GAME OVER STATE
        // ------------------------------------------------------------
        if (appState == APP_GAMEOVER) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawGame(&renderer, &board, -1, -1, &selectedMoves, &uiCtx, flipped);
            DrawClocks(whiteTimeMs, blackTimeMs, board.sideToMove, flipped);
            GameOverAction action = DrawGameOver(gameOverMessage);
            EndDrawing();

            if (action == GAMEOVER_PLAY_AGAIN) {
                appState = APP_MENU; // Falls into the menu branch next frame.
            } else if (action == GAMEOVER_QUIT) {
                shouldQuit = true;
            }

            continue;
        }

        // ------------------------------------------------------------
        // PLAYING STATE
        // ------------------------------------------------------------

        // Real-time clock: decrement whoever's turn it currently is.
        // (The engine's own thinking time is settled separately, right
        // after its blocking search call, since no frames render during it.)
        long long frameMs = (long long)(GetFrameTime() * 1000.0);
        if (board.sideToMove == SIDE_WHITE) {
            whiteTimeMs -= frameMs;
        } else {
            blackTimeMs -= frameMs;
        }

        if (whiteTimeMs <= 0) {
            whiteTimeMs = 0;
            appState = APP_GAMEOVER;
            gameOverMessage = "Black wins on time";
            continue;
        }
        if (blackTimeMs <= 0) {
            blackTimeMs = 0;
            appState = APP_GAMEOVER;
            gameOverMessage = "White wins on time";
            continue;
        }

        Vector2 mousePos = GetMousePosition();
        int rawRow = (int)(mousePos.y / tileSize);
        int rawCol = (int)(mousePos.x / tileSize);
        bool validMouse = (rawRow >= 0 && rawRow <= 7 && rawCol >= 0 && rawCol <= 7);

        // Convert screen coordinates to true board coordinates so all
        // downstream logic (and stored highlights/arrows) stays in board
        // space regardless of orientation; DrawGame handles the visual flip.
        int mouseRow = flipped ? 7 - rawRow : rawRow;
        int mouseCol = flipped ? 7 - rawCol : rawCol;

        uiCtx.mousePos = mousePos;

        bool isHumanTurn = (board.sideToMove == humanSide);

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
        // LEFT CLICK LOGIC (Selection and Dragging) — human only
        // ----------------------------------------------------
        if (isHumanTurn && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

        // Drop piece logic — human only
        if (isHumanTurn && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && uiCtx.isDragging) {
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

                    // The human's move just happened in real time (frameMs
                    // already accounted for it tick by tick); settle the increment.
                    if (humanSide == SIDE_WHITE) whiteTimeMs += incrementMs;
                    else blackTimeMs += incrementMs;

                    // Render human move instantly before the engine freezes the thread
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    DrawGame(&renderer, &board, -1, -1, &selectedMoves, &uiCtx, flipped);
                    DrawClocks(whiteTimeMs, blackTimeMs, board.sideToMove, flipped);
                    EndDrawing();

                    MoveList engineLegalMoves;
                    MoveList_Init(&engineLegalMoves);
                    GenerateLegalMoves(&board, &history, &engineLegalMoves);
                    
                    if (engineLegalMoves.count > 0) {
                        PlayEngineMove(&board, &history, &uiCtx, &whiteTimeMs, &blackTimeMs, incrementMs);

                        // Check whether the human now has a reply at all.
                        MoveList replyMoves;
                        MoveList_Init(&replyMoves);
                        GenerateLegalMoves(&board, &history, &replyMoves);
                        if (replyMoves.count == 0) {
                            appState = APP_GAMEOVER;
                            if (IsKingInCheck(&board, board.sideToMove)) {
                                gameOverMessage = (board.sideToMove == SIDE_WHITE)
                                    ? "Black wins by checkmate" : "White wins by checkmate";
                            } else {
                                gameOverMessage = "Draw by stalemate";
                            }
                        }
                    } else {
                        appState = APP_GAMEOVER;
                        if (IsKingInCheck(&board, board.sideToMove)) {
                            gameOverMessage = (board.sideToMove == SIDE_WHITE)
                                ? "Black wins by checkmate" : "White wins by checkmate";
                        } else {
                            gameOverMessage = "Draw by stalemate";
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawGame(&renderer, &board, selectedRow, selectedCol, &selectedMoves, &uiCtx, flipped);
        DrawClocks(whiteTimeMs, blackTimeMs, board.sideToMove, flipped);
        EndDrawing();
    }

    // 5. Cleanup at the very end
    UnloadRenderer(&renderer);
    CloseWindow();
    TT_Free(); // Free the TT only when the program is completely finished!

    return 0;
}