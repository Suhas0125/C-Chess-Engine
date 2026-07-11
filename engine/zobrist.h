#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "board.h"
#include <stdint.h>

// Initialize the random numbers used for hashing
void Zobrist_Init(void);

// Generate the unique hash key for a given board position
unsigned long long Zobrist_GenerateKey(const Board *board);

#endif