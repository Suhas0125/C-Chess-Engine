#ifndef HISTORY_H
#define HISTORY_H

#include "board.h"

#define MAX_GAME_MOVES 1024

// Stores all previous board states.
typedef struct{
    Board boards[MAX_GAME_MOVES];
    int count;
} History;

// Initializes the history stack.
void History_Init(History *history);

// Saves the current board state.
void History_Push(History *history, const Board *board);

// Restores the most recent board state.
int History_Pop(History *history, Board *board);

// Returns 1 if two board positions are identical.
int PositionEquals(const Board *a, const Board *b);

#endif