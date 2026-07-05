#include "renderer.h"

// -------------------------
// internal helper
// -------------------------
static Texture2D LoadPieceTexture(const char *path)
{
    return LoadTexture(path);
}

// -------------------------
// piece drawing
// -------------------------
static void DrawPiece(Piece p, int row, int col, int tileSize, PieceTextures *tex)
{
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
    Rectangle dest = {
        col * tileSize,
        row * tileSize,
        tileSize,
        tileSize
    };

    DrawTexturePro(t, source, dest, (Vector2){0,0}, 0.0f, WHITE);
}

// -------------------------
// public API
// -------------------------
void InitRenderer(Renderer *r)
{
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

void UnloadRenderer(Renderer *r)
{
    UnloadTexture(r->tex.wP);
    UnloadTexture(r->tex.wN);
    UnloadTexture(r->tex.wB);
    UnloadTexture(r->tex.wR);
    UnloadTexture(r->tex.wQ);
    UnloadTexture(r->tex.wK);

    UnloadTexture(r->tex.bP);
    UnloadTexture(r->tex.bN);
    UnloadTexture(r->tex.bB);
    UnloadTexture(r->tex.bR);
    UnloadTexture(r->tex.bQ);
    UnloadTexture(r->tex.bK);
}

void DrawGame(Renderer *r, Board *board)
{
    int ts = r->tileSize;

    // draw board
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Color color = ((row + col) % 2 == 0) ? BEIGE : BROWN;

            DrawRectangle(
                col * ts,
                row * ts,
                ts,
                ts,
                color
            );
        }
    }

    // draw pieces
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Piece p = board->squares[row][col];
            DrawPiece(p, row, col, ts, &r->tex);
        }
    }
}