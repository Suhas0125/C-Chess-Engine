#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "board.h"
#include "move.h"
#include "history.h"

// Executes a move on the board.
void MakeMove(Board *board, History *history, const Move *move);

// Restores the previous board state.
int UndoMove(Board *board, History *history);

#endif