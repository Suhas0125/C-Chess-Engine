#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "board.h"
#include "history.h"
#include "move.h"

// Returns 1 if the specified side is checkmated.
int IsCheckmate(Board *board, History *history, Side side);

// Returns 1 if the specified side is stalemated.
int IsStalemate(Board *board, History *history, Side side);

// Returns 1 if the fifty-move rule draw can be claimed.
int IsDrawByFiftyMoveRule(const Board *board);

// Returns 1 if the current position has occurred three or more times.
int IsDrawByThreefoldRepetition(const Board *board, const History *history);

#endif