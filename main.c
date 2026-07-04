#include "raylib.h"

int main(){
    InitWindow(800, 450, "Raylib Test");

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello Raylib!", 300, 200, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}