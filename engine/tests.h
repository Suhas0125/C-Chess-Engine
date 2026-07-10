#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>
#include "board.h"
#include "history.h"

// Famous Perft FENs (Step 5F)
#define FEN_START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN_POS3 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"
#define FEN_POS4 "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
#define FEN_POS5 "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"

typedef struct{
    const char *name;
    const char *fen;
    int depth;
    uint64_t expectedNodes;
} PerftTest;

// Runs the suite of official positions (Step 5G)
void RunAutomatedPerftSuite(Board *board, History *history);

#endif