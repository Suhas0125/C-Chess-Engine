#include <stdio.h>
#include <time.h>

#include "tests.h"
#include "fen.h"
#include "perft.h"

void RunAutomatedPerftSuite(Board *board, History *history){
    // Array of standard tests at depths that execute in just a few seconds total
    PerftTest suite[] = {
        {"Starting Position", FEN_START, 4, 197281ULL},
        {"Kiwipete", FEN_KIWIPETE, 4, 4085603ULL},
        {"Position 3 (Endgame)", FEN_POS3, 4, 43238ULL},
        {"Position 4 (Mirrored)", FEN_POS4, 4, 422333ULL},
        {"Position 5 (Promotions)", FEN_POS5, 4, 2103487ULL}
    };

    int numTests = sizeof(suite) / sizeof(suite[0]);
    int passed = 0;

    printf("\n========== AUTOMATED PERFT SUITE ==========\n");

    for (int i = 0; i < numTests; i++){
        printf("Test %d/%d: %s\n", i + 1, numTests, suite[i].name);
        printf("FEN: %s\n", suite[i].fen);
        
        // Reset board and history for each test
        Board_FromFEN(board, suite[i].fen);
        History_Init(history);

        clock_t start = clock();
        uint64_t nodes = Perft(board, history, suite[i].depth);
        clock_t end = clock();
        
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

        printf("Depth: %d | Expected: %llu | Engine: %llu\n", 
                suite[i].depth, suite[i].expectedNodes, nodes);

        if (nodes == suite[i].expectedNodes){
            printf("Result: PASSED (%.3f seconds)\n", time_spent);
            passed++;
        }
        else{
            printf("Result: FAILED\n");
        }
        printf("-------------------------------------------\n");
    }

    printf("Suite completed: %d/%d passed.\n", passed, numTests);
    printf("===========================================\n\n");
}