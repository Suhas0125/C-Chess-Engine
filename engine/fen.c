#include "fen.h"
#include <string.h>
#include <stdio.h>

static char pieceToChar(Piece p)
{
    switch (p)
    {
        case W_PAWN:   return 'P';
        case W_KNIGHT: return 'N';
        case W_BISHOP: return 'B';
        case W_ROOK:   return 'R';
        case W_QUEEN:  return 'Q';
        case W_KING:   return 'K';

        case B_PAWN:   return 'p';
        case B_KNIGHT: return 'n';
        case B_BISHOP: return 'b';
        case B_ROOK:   return 'r';
        case B_QUEEN:  return 'q';
        case B_KING:   return 'k';

        default: return 0;
    }
}

static void squareToAlgebraic(int sq, char *out){
    if (sq < 0)
    {
        out[0] = '-';
        out[1] = '\0';
        return;
    }

    int row = sq / 8;
    int col = sq % 8;

    // convert to chess coordinates
    char file = 'a' + col;
    char rank = '8' - row;

    out[0] = file;
    out[1] = rank;
    out[2] = '\0';
}

void Board_ToFEN(const Board *b, char *out){
    int index = 0;

    // -----------------------------
    // 1. PIECE PLACEMENT
    // -----------------------------
    for (int row = 0; row < 8; row++){
        int emptyCount = 0;

        for (int col = 0; col < 8; col++){
            Piece p = b->squares[row][col];

            if (p == EMPTY){
                emptyCount++;
            }
            else{
                if (emptyCount > 0){
                    out[index++] = '0' + emptyCount;
                    emptyCount = 0;
                }

                out[index++] = pieceToChar(p);
            }
        }

        if (emptyCount > 0)
            out[index++] = '0' + emptyCount;

        if (row != 7)
            out[index++] = '/';
    }

    out[index++] = ' ';

    // -----------------------------
    // 2. SIDE TO MOVE
    // -----------------------------
    out[index++] = (b->sideToMove == SIDE_WHITE) ? 'w' : 'b';

    out[index++] = ' ';

    // -----------------------------
    // 3. CASTLING RIGHTS
    // -----------------------------
    int hasCastling = 0;

    if (b->castling.whiteKingSide)  { out[index++] = 'K'; hasCastling = 1; }
    if (b->castling.whiteQueenSide) { out[index++] = 'Q'; hasCastling = 1; }
    if (b->castling.blackKingSide)  { out[index++] = 'k'; hasCastling = 1; }
    if (b->castling.blackQueenSide) { out[index++] = 'q'; hasCastling = 1; }

    if (!hasCastling)
        out[index++] = '-';

    out[index++] = ' ';

    // -----------------------------
    // 4. EN PASSANT
    // -----------------------------
    char ep[3];
    squareToAlgebraic(b->enPassantSquare, ep);

    if (ep[0] == '-' && ep[1] == '\0'){
        out[index++] = '-';
    }
    else{
        out[index++] = ep[0];
        out[index++] = ep[1];
    }

    out[index++] = ' ';

    // -----------------------------
    // 5. HALFMOVE CLOCK
    // -----------------------------
    index += sprintf(out + index, "%d", b->halfmoveClock);

    out[index++] = ' ';

    // -----------------------------
    // 6. FULLMOVE NUMBER
    // -----------------------------
    index += sprintf(out + index, "%d", b->fullmoveNumber);

    out[index] = '\0';
}