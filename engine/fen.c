#include "fen.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// board to fen helpers
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

// given board, construct fen
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

// fen to board helpers
static Piece CharToPiece(char c){
    switch (c){
        case 'P': return W_PAWN;
        case 'N': return W_KNIGHT;
        case 'B': return W_BISHOP;
        case 'R': return W_ROOK;
        case 'Q': return W_QUEEN;
        case 'K': return W_KING;

        case 'p': return B_PAWN;
        case 'n': return B_KNIGHT;
        case 'b': return B_BISHOP;
        case 'r': return B_ROOK;
        case 'q': return B_QUEEN;
        case 'k': return B_KING;

        default:
            return EMPTY;
    }
}

static int algebraicToSquare(const char *str){
    if (str[0] == '-')
        return -1;

    int col = str[0] - 'a';
    int row = '8' - str[1];

    return row * 8 + col;
}

// Construct board from given fen
int Board_FromFEN(Board *board, const char *fen){

    //--------------------------------------------------
    // Clear board
    //--------------------------------------------------
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            board->squares[row][col] = EMPTY;
        }
    }

    board->castling.whiteKingSide = 0;
    board->castling.whiteQueenSide = 0;
    board->castling.blackKingSide = 0;
    board->castling.blackQueenSide = 0;

    board->enPassantSquare = -1;
    board->halfmoveClock = 0;
    board->fullmoveNumber = 1;

    //--------------------------------------------------
    // 1. Piece placement
    //--------------------------------------------------

    int row = 0;
    int col = 0;

    while (*fen && *fen != ' '){
        if (*fen == '/'){
            row++;
            col = 0;
        }
        else if (isdigit((unsigned char)*fen)){
            col += *fen - '0';
        }
        else{
            board->squares[row][col] = CharToPiece(*fen);
            col++;
        }

        fen++;
    }

    if (*fen != ' ')
        return 0;

    fen++;

    //--------------------------------------------------
    // 2. Side to move
    //--------------------------------------------------

    if (*fen == 'w')
        board->sideToMove = SIDE_WHITE;
    else if (*fen == 'b')
        board->sideToMove = SIDE_BLACK;
    else
        return 0;

    fen += 2;

    //--------------------------------------------------
    // 3. Castling rights
    //--------------------------------------------------

    if (*fen == '-'){
        fen++;
    }
    else{
        while (*fen != ' '){
            switch (*fen){
                case 'K':
                    board->castling.whiteKingSide = 1;
                    break;

                case 'Q':
                    board->castling.whiteQueenSide = 1;
                    break;

                case 'k':
                    board->castling.blackKingSide = 1;
                    break;

                case 'q':
                    board->castling.blackQueenSide = 1;
                    break;
            }

            fen++;
        }
    }

    fen++;

    //--------------------------------------------------
    // 4. En passant
    //--------------------------------------------------

    if (*fen == '-'){
        board->enPassantSquare = -1;
        fen++;
    }
    else{
        board->enPassantSquare = algebraicToSquare(fen);
        fen += 2;
    }

    fen++;

    //--------------------------------------------------
    // 5. Halfmove clock
    //--------------------------------------------------

    board->halfmoveClock = atoi(fen);

    while (*fen != ' ')
        fen++;

    fen++;

    //--------------------------------------------------
    // 6. Fullmove number
    //--------------------------------------------------

    board->fullmoveNumber = atoi(fen);

    return 1;
}

// print fen string for current board
void PrintFEN(const Board *board){
    char fen[100];
    Board_ToFEN(board, fen);

    printf("%s\n", fen);
}