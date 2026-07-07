#ifndef MOVE_H
#define MOVE_H

#include "board.h"

#define MAX_MOVES 256

// Special move flags
typedef enum{
    MOVE_NONE        = 0,
    MOVE_CAPTURE     = 1 << 0, // 1
    MOVE_DOUBLE_PAWN = 1 << 1, // 2
    MOVE_EN_PASSANT  = 1 << 2, // 4
    MOVE_CASTLING    = 1 << 3, // 8
    MOVE_PROMOTION   = 1 << 4  // 16
} MoveFlags;

// One chess move
typedef struct{
    int fromRow;
    int fromCol;

    int toRow;
    int toCol;

    Piece promotion;
    int flags;

} Move;

// List of all moves generated
typedef struct{
    Move moves[MAX_MOVES];
    int count;
} MoveList;

// Clears the move list
void MoveList_Init(MoveList *list);

// Adds one move to the move list
void MoveList_Add(
    MoveList *list,
    int fromRow,
    int fromCol,
    int toRow,
    int toCol,
    Piece promotion,
    int flags
);

// Prints every move in a MoveList (for debugging).
void MoveList_Print(const MoveList *list);

#endif