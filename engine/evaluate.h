#ifndef EVALUATE_H
#define EVALUATE_H

#include "board.h"

#define CHECKMATE_SCORE 100000
#define DRAW_SCORE 0

// Returns the evaluation of the current position.
// Positive: White is better.
// Negative: Black is better.
// Zero: Equal.
int EvaluatePosition(const Board *board);

#endif