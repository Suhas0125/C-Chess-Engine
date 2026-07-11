#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"
#include "history.h"

// Stores the result of a completed search.
typedef struct{
    Move move;
    int score;
    unsigned long long nodes;
    int depth;
} SearchResult;

// time controls
extern bool searchStopped;
extern long long startTimeMs;
extern long long stopTimeMs;
extern bool timeControlEnabled;

// Quiescence Search: Evaluates tactical captures beyond depth 0
int QuiescenceSearch(Board *board, History *history, int alpha, int beta, unsigned long long *nodes);

// Returns the evaluation of the position by searching to the given depth using Alpha-Beta pruning
int Negamax(Board *board,
            History *history,
            int depth,
            int ply,        // ply - distance from root (step 6C)
            int alpha,
            int beta,
            unsigned long long *nodes);

// Returns the best move found at the given search depth.
SearchResult SearchBestMove(Board *board, History *history, int depth, long long allottedTimeMs);

// Finds and plays the best move for the current side.
SearchResult MakeEngineMove(Board *board,
                            History *history,
                            int depth);

#endif