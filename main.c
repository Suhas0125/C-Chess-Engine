#include <stdio.h>

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

Renderer renderer;

int main(){
    
    InitWindow(800, 800, "C_Chess_Engine");
    
    // 100 = each square is 100×100 pixels
    // whole board = 800×800 (8 × 100)
    int tileSize = 100; // How big each chess square is in pixels

    Board board;
    Board_Init(&board);

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

// testing start

    

// testing end

    while(!WindowShouldClose()){

        // engine testing start

        

        // engine testing end

        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse = GetMousePosition();
        int mouseRow = mouse.y / tileSize;
        int mouseCol = mouse.x / tileSize;
        
        if (mouseRow < 0 || mouseRow > 7) mouseRow = -1;
        if (mouseCol < 0 || mouseCol > 7) mouseCol = -1;

        // Handle left mouse click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (mouseRow != -1 && mouseCol != -1)
            {
                printf("\n========================================\n");
                printf("Move %d\n", board.fullmoveNumber);
                printf("Side to move : %s\n",
                    board.sideToMove == SIDE_WHITE ? "WHITE" : "BLACK");

                //--------------------------------------------------
                // Print every legal move in current position
                //--------------------------------------------------

                MoveList allLegalMoves;
                MoveList_Init(&allLegalMoves);

                GenerateLegalMoves(&board, &history, &allLegalMoves);

                printf("Legal moves available : %d\n", allLegalMoves.count);
                MoveList_Print(&allLegalMoves);

                //--------------------------------------------------
                // Try playing selected move
                //--------------------------------------------------

                int movePlayed = 0;

                for (int i = 0; i < selectedMoves.count; i++)
                {
                    Move move = selectedMoves.moves[i];

                    if (move.toRow == mouseRow &&
                        move.toCol == mouseCol)
                    {
                        printf("\nHuman plays:\n");
                        Move_Print(&move);

                        MakeMove(&board, &history, &move);

                        selectedRow = -1;
                        selectedCol = -1;
                        MoveList_Init(&selectedMoves);

                        movePlayed = 1;
                        break;
                    }
                }

                //--------------------------------------------------
                // Engine turn
                //--------------------------------------------------

                if (movePlayed)
                {
                    printf("\n----------------------------------------\n");
                    printf("Engine turn (%s)\n",
                        board.sideToMove == SIDE_WHITE ?
                        "WHITE" : "BLACK");

                    MoveList engineLegalMoves;
                    MoveList_Init(&engineLegalMoves);

                    GenerateLegalMoves(&board, &history, &engineLegalMoves);

                    printf("Engine legal moves : %d\n",
                        engineLegalMoves.count);

                    MoveList_Print(&engineLegalMoves);

                    if (engineLegalMoves.count == 0)
                    {
                        if (IsKingInCheck(&board, board.sideToMove))
                        {
                            printf("\n***** CHECKMATE *****\n");
                        }
                        else
                        {
                            printf("\n***** STALEMATE *****\n");
                        }

                        continue;
                    }

                    SearchResult result =
                        SearchBestMove(&board, &history, 4);

                    printf("\nSearch result\n");
                    printf("Best move : ");
                    Move_Print(&result.move);

                    printf("Evaluation : %d\n", result.score);
                    printf("Nodes      : %llu\n", result.nodes);

                    MakeMove(&board, &history, &result.move);

                    printf("\nEngine played successfully.\n");

                    continue;
                }

                //--------------------------------------------------
                // Selecting a white piece
                //--------------------------------------------------

                Piece piece = board.squares[mouseRow][mouseCol];

                if (IsWhitePiece(piece))
                {
                    selectedRow = mouseRow;
                    selectedCol = mouseCol;

                    MoveList legalMoves;
                    MoveList_Init(&legalMoves);

                    GenerateLegalMoves(&board, &history, &legalMoves);

                    MoveList_Init(&selectedMoves);

                    for (int i = 0; i < legalMoves.count; i++)
                    {
                        Move move = legalMoves.moves[i];

                        if (move.fromRow == selectedRow &&
                            move.fromCol == selectedCol)
                        {
                            MoveList_Add(
                                &selectedMoves,
                                move.fromRow,
                                move.fromCol,
                                move.toRow,
                                move.toCol,
                                move.promotion,
                                move.flags
                            );
                        }
                    }

                    printf("\nSelected piece legal moves:\n");
                    MoveList_Print(&selectedMoves);
                }
                else
                {
                    selectedRow = -1;
                    selectedCol = -1;
                    MoveList_Init(&selectedMoves);
                }
            }
        }

        DrawGame(
            &renderer,
            &board,
            selectedRow,
            selectedCol,
            &selectedMoves
        );

        EndDrawing();
    }


    UnloadRenderer(&renderer);

    CloseWindow();

    return 0;
}