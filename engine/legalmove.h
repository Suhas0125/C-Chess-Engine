#ifndef LEGALMOVE_H
#define LEGALMOVE_H

#include "board.h"
#include "move.h"
#include "history.h"

// Generates all legal moves for the current position.
void GenerateLegalMoves(Board *board, History *history, MoveList *legalMoves);

#endif