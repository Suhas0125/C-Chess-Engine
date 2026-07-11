#ifndef TT_H
#define TT_H

#include <stdint.h>
#include "move.h"

// TT Flags
#define HASH_EXACT 0 // Exact score (Usually a PV node)
#define HASH_ALPHA 1 // Upper bound (Failed low - move was bad)
#define HASH_BETA  2 // Lower bound (Failed high - move caused a cutoff)

typedef struct {
    uint64_t key;
    Move bestMove;
    int score;
    int depth;
    int flag;
} TTEntry;

// Initializes the hash table to a specific size in Megabytes
void TT_Init(int megabytes);

// Frees the hash table from memory
void TT_Free(void);

// Clears all entries
void TT_Clear(void);

// Saves an evaluation to the table
void TT_Store(uint64_t key, int depth, int score, int flag, Move bestMove);

// Attempts to retrieve a saved evaluation. Returns 1 if a cutoff occurs.
int TT_Probe(uint64_t key, int depth, int alpha, int beta, int *score, Move *bestMove);

#endif