#include <limits.h>

#include "search.h"

#include "evaluate.h"
#include "legalmove.h"
#include "makemove.h"
#include "attack.h"

// Evaluation of the best move found during the last search.
int bestEvaluation = 0;

// Searches the position by evaluating every legal move.
int Negamax(Board *board,
            History *history,
            int depth,
            int alpha,
            int beta,
            unsigned long long *nodes)
{
    // Count this position as one searched node.
    (*nodes)++;

    // Stop searching when the maximum depth is reached.
    if (depth == 0){
        return EvaluatePosition(board);
    }

    MoveList legalMoves;
    GenerateLegalMoves(board, history, &legalMoves);

    // Handle positions with no legal moves.
    if (legalMoves.count == 0){

        // Current side is checkmated.
        if (IsKingInCheck(board, board->sideToMove)){
            return -100000;
        }

        // Current side is stalemated.
        return 0;
    }

    int bestScore = INT_MIN;

    // Try every legal move.
    for (int i = 0; i < legalMoves.count; i++){
        Move move = legalMoves.moves[i];

        // Play the move.
        MakeMove(board, history, &move);

        // Search one ply deeper from the opponent's perspective.
        int score = -Negamax(board,         // score turned negative
                     history,
                     depth - 1,
                     -beta,
                     -alpha,
                     nodes);

        // Restore the previous position.
        UndoMove(board, history);

        // Keep the best score found so far.
        if (score > bestScore){
            bestScore = score;
        }
        
        // Update alpha with the best score found so far.
        if (bestScore > alpha){
            alpha = bestScore;
        }

        // Stop searching if the remaining moves cannot improve the result.
        if (alpha >= beta){
            break;
        }
    }

    return bestScore;
}

// Searches all legal moves and returns the best one.
SearchResult SearchBestMove(Board *board, History *history, int depth){

    SearchResult result;

    result.move = (Move){0};
    result.score = INT_MIN;
    result.nodes = 0;

    result.depth = depth;

    MoveList legalMoves;
    GenerateLegalMoves(board, history, &legalMoves);

    // remove up
    printf("\n========== ROOT SEARCH ==========\n");
    printf("Legal moves at root: %d\n", legalMoves.count);

    for (int i = 0; i < legalMoves.count; i++)
    {
        printf("Root move %d: ", i + 1);
        Move_Print(&legalMoves.moves[i]);
    }
    // remove up

    // No legal moves available.
    if (legalMoves.count == 0){

        // Checkmate.
        if (IsKingInCheck(board, board->sideToMove)){
            result.score = -CHECKMATE_SCORE;
        }

        // Stalemate.
        else{
            result.score = DRAW_SCORE;
        }

        return result;
    }

    // Search every legal move.
    for (int i = 0; i < legalMoves.count; i++){

        Move move = legalMoves.moves[i];

        // Play the move.
        MakeMove(board, history, &move);

        // Evaluate the resulting position.
        int score = -Negamax(board,
                     history,
                     depth - 1,
                     INT_MIN,
                     INT_MAX,
                     &result.nodes);

        // remove down
        printf("Score: %d for ", score);
        Move_Print(&move);
        // remove up

        // Restore the previous position.
        UndoMove(board, history);

        // Keep the best move found so far.
        if (score > result.score){
            result.score = score;
            result.move = move;
        }
    }

    // remove down
    printf("\nBEST MOVE CHOSEN:\n");
    Move_Print(&result.move);
    printf("Best score = %d\n", result.score);
    printf("===============================\n");
    // remove up

    return result;
}

// Finds, executes, and returns the best move for the current side.
SearchResult MakeEngineMove(Board *board,
                            History *history,
                            int depth){

    // Search for the best move.
    SearchResult result = SearchBestMove(board, history, depth);

    // No legal move (checkmate or stalemate)
    if (result.score == -CHECKMATE_SCORE ||
        result.score == DRAW_SCORE){
        return result;
    }  

    // Play the selected move.
    MakeMove(board, history, &result.move);

    return result;
}

