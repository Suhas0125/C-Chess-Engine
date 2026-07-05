#include <stdio.h>

#include "raylib.h"
#include "engine/board.h"
#include "engine/fen.h"
#include "ui/renderer.h"
#include "engine/move.h"

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



    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse = GetMousePosition();
        int mouseRow = mouse.y / tileSize;
        int mouseCol = mouse.x / tileSize;
        
        if (mouseRow < 0 || mouseRow > 7) mouseRow = -1;
        if (mouseCol < 0 || mouseCol > 7) mouseCol = -1;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if (mouseRow != -1 && mouseCol != -1){
                Piece p = board.squares[mouseRow][mouseCol];

                printf("Clicked square: %d,%d Piece: %d\n",
                    mouseRow, mouseCol, p);
            }
        }

        DrawGame(&renderer, &board);

        EndDrawing();
    }


    UnloadRenderer(&renderer);

    CloseWindow();

    return 0;
}