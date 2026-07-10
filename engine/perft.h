#ifndef PERFT_H
#define PERFT_H

#include <stdint.h>

#include "board.h"
#include "history.h"

// Recursively counts all leaf nodes up to the given depth
uint64_t Perft(Board *board, History *history, int depth);

// Prints root moves and their individual subtree counts
void PerftDivide(Board *board, History *history, int depth);

#endif