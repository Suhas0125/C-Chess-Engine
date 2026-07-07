#ifndef ATTACK_H
#define ATTACK_H

#include "board.h"

// Returns 1 if the given square is attacked by the specified side.
int IsSquareAttacked(const Board *board, int row, int col, Side attacker);

// Returns 1 if the specified side's king is currently in check.
int IsKingInCheck(const Board *board, Side side);

#endif