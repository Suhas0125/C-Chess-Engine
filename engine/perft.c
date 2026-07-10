#include <stdio.h>

#include "perft.h"
#include "legalmove.h"
#include "makemove.h"
#include "move.h"

uint64_t Perft(Board *board, History *history, int depth){
    if (depth == 0){
        return 1ULL;
    }

    MoveList moveList;
    GenerateLegalMoves(board, history, &moveList);

    uint64_t nodes = 0;

    for (int i = 0; i < moveList.count; i++){
        MakeMove(board, history, &moveList.moves[i]);
        nodes += Perft(board, history, depth - 1);
        UndoMove(board, history);
    }

    return nodes;
}

void PerftDivide(Board *board, History *history, int depth){
    if (depth == 0) return;

    MoveList moveList;
    GenerateLegalMoves(board, history, &moveList);

    uint64_t totalNodes = 0;

    printf("Perft Divide Depth: %d\n\n", depth);

    for (int i = 0; i < moveList.count; i++) {
        Move *move = &moveList.moves[i];

        MakeMove(board, history, move);
        uint64_t nodes = Perft(board, history, depth - 1);
        UndoMove(board, history);

        // Convert board coordinates to algebraic notation
        char fromFile = 'a' + move->fromCol;
        char fromRank = '8' - move->fromRow;
        char toFile = 'a' + move->toCol;
        char toRank = '8' - move->toRow;

        // Handle promotion formatting
        if (move->flags & MOVE_PROMOTION){
            char promoChar = 'q'; 
            if (move->promotion == W_KNIGHT || move->promotion == B_KNIGHT) promoChar = 'n';
            else if (move->promotion == W_BISHOP || move->promotion == B_BISHOP) promoChar = 'b';
            else if (move->promotion == W_ROOK || move->promotion == B_ROOK) promoChar = 'r';
            
            printf("%c%c%c%c%c : %llu\n", fromFile, fromRank, toFile, toRank, promoChar, nodes);
        }
        else{
            printf("%c%c%c%c : %llu\n", fromFile, fromRank, toFile, toRank, nodes);
        }

        totalNodes += nodes;
    }

    printf("\nTotal : %llu\n", totalNodes);
}

