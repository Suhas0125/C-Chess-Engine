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


// Returns the evaluation of the position by searching to the given depth using Alpha-Beta pruning
int Negamax(Board *board,
            History *history,
            int depth,
            int alpha,
            int beta,
            unsigned long long *nodes);

// Returns the best move found at the given search depth.
SearchResult SearchBestMove(Board *board, History *history, int depth);

// Finds and plays the best move for the current side.
SearchResult MakeEngineMove(Board *board,
                            History *history,
                            int depth);

#endif