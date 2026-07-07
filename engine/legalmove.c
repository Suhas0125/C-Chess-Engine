#include "legalmove.h"

#include "move.h"
#include "movegen.h"
#include "makemove.h"
#include "attack.h"

// Generates all legal moves for the current position.
void GenerateLegalMoves(Board *board, History *history, MoveList *legalMoves)
{
    MoveList pseudoMoves;
    MoveList_Init(&pseudoMoves);

    // Generate all pseudo-legal moves.
    GenerateMoves(board, &pseudoMoves);

    // Start with an empty legal move list.
    MoveList_Init(legalMoves);

    // Remember whose turn it is.
    Side movingSide = board->sideToMove;

    // Test every pseudo-legal move.
    for (int i = 0; i < pseudoMoves.count; i++){
        Move move = pseudoMoves.moves[i];

        // Play the move.
        MakeMove(board, history, &move);

        // Is our own king still safe?
        if (!IsKingInCheck(board, movingSide)){
            // if no, then add the moves temporarily as a set of legal moves at the present board postion
            MoveList_Add(
                legalMoves,
                move.fromRow,
                move.fromCol,
                move.toRow,
                move.toCol,
                move.promotion,
                move.flags
            );
        }

        // Restore the original position.
        UndoMove(board, history);
    }
}