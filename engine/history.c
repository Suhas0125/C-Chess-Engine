#include "history.h"

// Initializes an empty history stack.
void History_Init(History *history){
    history->count = 0;
}

// Saves the current board state.
void History_Push(History *history, const Board *board){
    if (history->count >= MAX_GAME_MOVES)
        return;

    Board_Copy(&history->boards[history->count], board);
    history->count++;
}

// Restores the most recent board state.
int History_Pop(History *history, Board *board){
    if (history->count == 0)
        return 0;

    history->count--;

    Board_Copy(board, &history->boards[history->count]);

    return 1;
}

// Returns 1 if two board positions are identical.
int PositionEquals(const Board *a, const Board *b){
    // Compare every square.
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            if (a->squares[row][col] != b->squares[row][col]){
                return 0;
            }
        }
    }

    //  Two positions are identical only if all of these are the same:

    // ✅ Piece placement
    // ✅ Side to move
    // ✅ Castling rights
    // ✅ En passant square

    // Notice what is not included:

    // ❌ Halfmove clock
    // ❌ Fullmove number

    // Those don't affect the legal moves available, so FIDE does not consider them part of the position.

    if (a->sideToMove != b->sideToMove)
        return 0;

    if (a->enPassantSquare != b->enPassantSquare)
        return 0;

    if (a->castling.whiteKingSide != b->castling.whiteKingSide)
        return 0;

    if (a->castling.whiteQueenSide != b->castling.whiteQueenSide)
        return 0;

    if (a->castling.blackKingSide != b->castling.blackKingSide)
        return 0;

    if (a->castling.blackQueenSide != b->castling.blackQueenSide)
        return 0;

    return 1;
}

