#include "tt.h"
#include <stdlib.h>
#include <stdio.h>

TTEntry *ttTable = NULL;
int ttSize = 0;

void TT_Init(int megabytes) {
    int entrySize = sizeof(TTEntry);
    ttSize = (megabytes * 1024 * 1024) / entrySize;
    
    if (ttTable != NULL) {
        free(ttTable);
    }
    
    ttTable = (TTEntry *)malloc(ttSize * entrySize);
    if (ttTable == NULL) {
        printf("Error: Could not allocate Transposition Table!\n");
        exit(1);
    }
    
    TT_Clear();
}

void TT_Free(void) {
    if (ttTable != NULL) {
        free(ttTable);
        ttTable = NULL;
    }
}

void TT_Clear(void) {
    for (int i = 0; i < ttSize; i++) {
        ttTable[i].key = 0;
        ttTable[i].depth = 0;
        ttTable[i].flag = 0;
        ttTable[i].score = 0;
        // Move is zero-initialized by setting key=0
    }
}

void TT_Store(uint64_t key, int depth, int score, int flag, Move bestMove) {
    int index = key % ttSize;
    
    // Always replace scheme (Simple and effective)
    ttTable[index].key = key;
    ttTable[index].depth = depth;
    ttTable[index].score = score;
    ttTable[index].flag = flag;
    ttTable[index].bestMove = bestMove;
}

int TT_Probe(uint64_t key, int depth, int alpha, int beta, int *score, Move *bestMove) {
    int index = key % ttSize;
    
    if (ttTable[index].key == key) {
        // Always extract the best move for move-ordering, even if depth is lower
        *bestMove = ttTable[index].bestMove;
        
        // We can only use the score if we searched to at least the same depth
        if (ttTable[index].depth >= depth) {
            int cachedScore = ttTable[index].score;
            
            if (ttTable[index].flag == HASH_EXACT) {
                *score = cachedScore;
                return 1; // Valid cutoff
            }
            if (ttTable[index].flag == HASH_ALPHA && cachedScore <= alpha) {
                *score = cachedScore;
                return 1; // Valid cutoff
            }
            if (ttTable[index].flag == HASH_BETA && cachedScore >= beta) {
                *score = cachedScore;
                return 1; // Valid cutoff
            }
        }
    }
    return 0; // No cutoff
}