#include "gamestate.h"

#include "attack.h"
#include "legalmove.h"
#include "history.h"

// Returns 1 if the specified side is checkmated.
int IsCheckmate(Board *board, History *history, Side side){
    Side originalSide = board->sideToMove;

    // Generate legal moves for the specified side.
    board->sideToMove = side;

    MoveList legalMoves;
    GenerateLegalMoves(board, history, &legalMoves);

    // Restore the original side to move.
    board->sideToMove = originalSide;

    // King is in check and no legal moves
    return IsKingInCheck(board, side) &&
           legalMoves.count == 0;
}

// Returns 1 if the specified side is stalemated.
int IsStalemate(Board *board, History *history, Side side)
{
    Side originalSide = board->sideToMove;

    // Generate legal moves for the specified side.
    board->sideToMove = side;

    MoveList legalMoves;
    GenerateLegalMoves(board, history, &legalMoves);

    // Restore the original side to move.
    board->sideToMove = originalSide;

    // King is NOT in check and no legal moves
    return !IsKingInCheck(board, side) &&
           legalMoves.count == 0;
}

// Returns 1 if the fifty-move rule draw can be claimed.
int IsDrawByFiftyMoveRule(const Board *board){
    return board->halfmoveClock >= 100;
}

// Returns 1 if the current position has occurred three or more times.
int IsDrawByThreefoldRepetition(const Board *board, const History *history){
    int repetitions = 0;

    // Compare the current position with every position in history.
    for (int i = 0; i < history->count; i++){
        if (PositionEquals(board, &history->boards[i])){
            repetitions++;
        }
    }

    return repetitions >= 3;
}