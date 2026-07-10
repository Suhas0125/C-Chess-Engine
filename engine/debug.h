#ifndef DEBUG_H
#define DEBUG_H

#include "board.h"
#include "history.h"
#include "move.h"

// Print current board information.
void Debug_PrintBoardState(const Board *board);

// Print every legal move in the current position.
void Debug_PrintLegalMoves(Board *board, History *history);

// Print the current game state.
void Debug_PrintGameState(Board *board, History *history);

#endif