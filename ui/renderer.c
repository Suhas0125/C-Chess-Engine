#include "renderer.h"
#include "../engine/move.h"
#include <math.h>
#include <stdio.h>

// -------------------------
// internal helper
// -------------------------
static Texture2D LoadPieceTexture(const char *path){
    return LoadTexture(path);
}

// Maps a true board row/col to the screen row/col it should be drawn at.
// Board coordinates never change; only where they land on screen does.
static int ToScreenRow(int row, bool flipped) { return flipped ? 7 - row : row; }
static int ToScreenCol(int col, bool flipped) { return flipped ? 7 - col : col; }

// Draws a piece at exact pixel coordinates (useful for dragging)
static void DrawPieceEx(Piece p, float x, float y, int tileSize, PieceTextures *tex){
    if (p == EMPTY) return;

    Texture2D t;
    switch (p)
    {
        case W_PAWN:   t = tex->wP; break;
        case W_KNIGHT: t = tex->wN; break;
        case W_BISHOP: t = tex->wB; break;
        case W_ROOK:   t = tex->wR; break;
        case W_QUEEN:  t = tex->wQ; break;
        case W_KING:   t = tex->wK; break;

        case B_PAWN:   t = tex->bP; break;
        case B_KNIGHT: t = tex->bN; break;
        case B_BISHOP: t = tex->bB; break;
        case B_ROOK:   t = tex->bR; break;
        case B_QUEEN:  t = tex->bQ; break;
        case B_KING:   t = tex->bK; break;
        default: return;
    }

    Rectangle source = {0, 0, t.width, t.height};
    Rectangle dest = { x, y, tileSize, tileSize };
    DrawTexturePro(t, source, dest, (Vector2){0,0}, 0.0f, WHITE);
}

// Draws an arrow
static void DrawArrow(Vector2 start, Vector2 end, Color color) {
    float thick = 6.0f;
    DrawLineEx(start, end, thick, color);
    
    Vector2 dir = { end.x - start.x, end.y - start.y };
    float length = sqrtf(dir.x*dir.x + dir.y*dir.y);
    if (length == 0) return;
    
    dir.x /= length; 
    dir.y /= length;
    
    float headSize = 25.0f;
    Vector2 perp = { -dir.y, dir.x };
    
    Vector2 p1 = end;
    Vector2 p2 = { end.x - dir.x * headSize + perp.x * (headSize * 0.5f), 
                   end.y - dir.y * headSize + perp.y * (headSize * 0.5f) };
    Vector2 p3 = { end.x - dir.x * headSize - perp.x * (headSize * 0.5f), 
                   end.y - dir.y * headSize - perp.y * (headSize * 0.5f) };
                   
    DrawTriangle(p1, p2, p3, color);
}

// -------------------------
// public API
// -------------------------
void InitRenderer(Renderer *r) {
    r->tileSize = 100;
    r->tex.wP = LoadPieceTexture("assets/pieces/wP.png");
    r->tex.wN = LoadPieceTexture("assets/pieces/wN.png");
    r->tex.wB = LoadPieceTexture("assets/pieces/wB.png");
    r->tex.wR = LoadPieceTexture("assets/pieces/wR.png");
    r->tex.wQ = LoadPieceTexture("assets/pieces/wQ.png");
    r->tex.wK = LoadPieceTexture("assets/pieces/wK.png");

    r->tex.bP = LoadPieceTexture("assets/pieces/bP.png");
    r->tex.bN = LoadPieceTexture("assets/pieces/bN.png");
    r->tex.bB = LoadPieceTexture("assets/pieces/bB.png");
    r->tex.bR = LoadPieceTexture("assets/pieces/bR.png");
    r->tex.bQ = LoadPieceTexture("assets/pieces/bQ.png");
    r->tex.bK = LoadPieceTexture("assets/pieces/bK.png");
}

void UnloadRenderer(Renderer *r) {
    UnloadTexture(r->tex.wP); UnloadTexture(r->tex.wN); UnloadTexture(r->tex.wB);
    UnloadTexture(r->tex.wR); UnloadTexture(r->tex.wQ); UnloadTexture(r->tex.wK);
    UnloadTexture(r->tex.bP); UnloadTexture(r->tex.bN); UnloadTexture(r->tex.bB);
    UnloadTexture(r->tex.bR); UnloadTexture(r->tex.bQ); UnloadTexture(r->tex.bK);
}

void DrawGame(Renderer *r, Board *board, int selectedRow, int selectedCol, MoveList *selectedMoves, UIContext *ctx, bool flipped) {
    int ts = r->tileSize;

    // 1. Draw Board & Highlights
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Color color = ((row + col) % 2 == 0) ? BEIGE : BROWN;

            // Highlight last played move
            if (ctx->hasLastMove && 
               ((row == ctx->lastMove.fromRow && col == ctx->lastMove.fromCol) || 
                (row == ctx->lastMove.toRow && col == ctx->lastMove.toCol))) {
                color = (Color){205, 210, 106, 255}; // Yellow highlight
            }

            // Right-click highlight
            for (int i = 0; i < ctx->highlightCount; i++) {
                if (ctx->highlights[i].row == row && ctx->highlights[i].col == col) {
                    color = (Color){235, 97, 80, 255}; // Red highlight
                }
            }

            // Currently selected square
            if (row == selectedRow && col == selectedCol) {
                color = GOLD;
            }

            int sr = ToScreenRow(row, flipped);
            int sc = ToScreenCol(col, flipped);
            DrawRectangle(sc * ts, sr * ts, ts, ts, color);
        }
    }

    // 2. Draw Pieces (Skip dragged piece)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (ctx->isDragging && row == ctx->dragRow && col == ctx->dragCol) {
                continue; 
            }
            Piece p = board->squares[row][col];
            int sr = ToScreenRow(row, flipped);
            int sc = ToScreenCol(col, flipped);
            DrawPieceEx(p, sc * ts, sr * ts, ts, &r->tex);
        }
    }

    // 3. Draw Legal Moves (On top of pieces)
    for (int i = 0; i < selectedMoves->count; i++) {
        Move move = selectedMoves->moves[i];
        int sr = ToScreenRow(move.toRow, flipped);
        int sc = ToScreenCol(move.toCol, flipped);
        float centerX = sc * ts + ts / 2.0f;
        float centerY = sr * ts + ts / 2.0f;

        if (board->squares[move.toRow][move.toCol] != EMPTY || (move.flags & MOVE_EN_PASSANT)) {
            // Draw a thick ring for captures
            DrawRing((Vector2){centerX, centerY}, ts * 0.40f, ts * 0.48f, 0, 360, 36, (Color){0, 0, 0, 60});
        } else {
            // Draw a solid dot for empty squares
            DrawCircle(centerX, centerY, ts * 0.15f, (Color){0, 0, 0, 60});
        }
    }

    // 4. Draw Arrows
    for (int i = 0; i < ctx->arrowCount; i++) {
        int fsr = ToScreenRow(ctx->arrows[i].fromRow, flipped);
        int fsc = ToScreenCol(ctx->arrows[i].fromCol, flipped);
        int tsr = ToScreenRow(ctx->arrows[i].toRow, flipped);
        int tsc = ToScreenCol(ctx->arrows[i].toCol, flipped);
        Vector2 start = { fsc * ts + ts/2.0f, fsr * ts + ts/2.0f };
        Vector2 end = { tsc * ts + ts/2.0f, tsr * ts + ts/2.0f };
        DrawArrow(start, end, (Color){255, 170, 0, 200});
    }

    // 5. Draw Dragged Piece (On top of everything, follows raw mouse pixels)
    if (ctx->isDragging) {
        Piece p = board->squares[ctx->dragRow][ctx->dragCol];
        DrawPieceEx(p, ctx->mousePos.x - ts / 2, ctx->mousePos.y - ts / 2, ts, &r->tex);
    }
}

// Formats milliseconds as mm:ss, clamped at zero.
static void FormatClock(long long ms, char *buf, int bufSize) {
    if (ms < 0) ms = 0;
    long long totalSeconds = ms / 1000;
    int minutes = (int)(totalSeconds / 60);
    int seconds = (int)(totalSeconds % 60);
    snprintf(buf, bufSize, "%02d:%02d", minutes, seconds);
}

static void DrawClockLabel(const char *sideName, long long timeMs, int y, bool active) {
    char timeText[16];
    FormatClock(timeMs, timeText, sizeof(timeText));

    char full[48];
    snprintf(full, sizeof(full), "%s  %s", sideName, timeText);

    int fontSize = 22;
    int textWidth = MeasureText(full, fontSize);
    Rectangle bg = { 8, (float)y, textWidth + 16.0f, 30 };

    Color bgColor = active ? (Color){255, 215, 0, 210} : (Color){0, 0, 0, 140};
    Color textColor = active ? BLACK : RAYWHITE;

    DrawRectangleRec(bg, bgColor);
    DrawText(full, 16, y + 4, fontSize, textColor);
}

void DrawClocks(long long whiteTimeMs, long long blackTimeMs, Side sideToMove, bool flipped) {
    // Place each clock on the screen edge matching that color's home rank
    // (board row 7 = White's rank, row 0 = Black's rank).
    int whiteScreenRow = ToScreenRow(7, flipped);
    int blackScreenRow = ToScreenRow(0, flipped);

    int whiteY = (whiteScreenRow == 0) ? 8 : 762;
    int blackY = (blackScreenRow == 0) ? 8 : 762;

    DrawClockLabel("White", whiteTimeMs, whiteY, sideToMove == SIDE_WHITE);
    DrawClockLabel("Black", blackTimeMs, blackY, sideToMove == SIDE_BLACK);
}

GameOverAction DrawGameOver(const char *message) {
    DrawRectangle(0, 300, 800, 200, (Color){0, 0, 0, 190});

    int fontSize = 28;
    int textWidth = MeasureText(message, fontSize);
    DrawText(message, (800 - textWidth) / 2, 335, fontSize, RAYWHITE);

    Rectangle playAgainBtn = { 220, 410, 160, 50 };
    Rectangle quitBtn = { 420, 410, 160, 50 };

    Vector2 mouse = GetMousePosition();
    bool hoverPlay = CheckCollisionPointRec(mouse, playAgainBtn);
    bool hoverQuit = CheckCollisionPointRec(mouse, quitBtn);

    DrawRectangleRec(playAgainBtn, hoverPlay ? (Color){150, 230, 150, 255} : (Color){110, 190, 110, 255});
    DrawRectangleLinesEx(playAgainBtn, 2, DARKGRAY);
    const char *playLabel = "Play Again";
    int playWidth = MeasureText(playLabel, 20);
    DrawText(playLabel, (int)(playAgainBtn.x + (playAgainBtn.width - playWidth) / 2.0f), (int)(playAgainBtn.y + 15), 20, BLACK);

    DrawRectangleRec(quitBtn, hoverQuit ? (Color){230, 150, 150, 255} : (Color){190, 110, 110, 255});
    DrawRectangleLinesEx(quitBtn, 2, DARKGRAY);
    const char *quitLabel = "Quit";
    int quitWidth = MeasureText(quitLabel, 20);
    DrawText(quitLabel, (int)(quitBtn.x + (quitBtn.width - quitWidth) / 2.0f), (int)(quitBtn.y + 15), 20, BLACK);

    if (hoverPlay && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return GAMEOVER_PLAY_AGAIN;
    if (hoverQuit && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return GAMEOVER_QUIT;
    return GAMEOVER_NONE;
}