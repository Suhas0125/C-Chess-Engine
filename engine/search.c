// --- engine/search.c ---
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "search.h"
#include "evaluate.h" // GetPieceValue is now pulled from here
#include "legalmove.h"
#include "makemove.h"
#include "attack.h"
#include "gamestate.h"
#include "tt.h" // step 6E

// Safe infinity bounds to prevent C's two's-complement INT_MIN negation bug
#define INFINITY_SCORE 1000000
#define MAX_PLY 64

// --- PV Table (Principal Variation) ---
static Move pvArray[MAX_PLY][MAX_PLY];
static int pvLength[MAX_PLY];

// The best line found in the previous iteration, used for move ordering
static Move pvLine[MAX_PLY];
static int pvLineLength = 0;

// Killer Moves: Store up to 2 killer moves per ply
static Move killerMoves[MAX_PLY][2];

// History Heuristic: [side][fromSquare][toSquare]
static int historyTable[2][64][64];

// Helper to compare two moves
static int MovesEqual(Move a, Move b){
    return (a.fromRow == b.fromRow && a.fromCol == b.fromCol && 
            a.toRow == b.toRow && a.toCol == b.toCol && 
            a.promotion == b.promotion);
}

// Helper to check if a move is a killer move
static int IsKillerMove(Move move, int ply) {
    if (MovesEqual(move, killerMoves[ply][0])) return 1;
    if (MovesEqual(move, killerMoves[ply][1])) return 2;
    return 0;
}

// Evaluation of the best move found during the last search.
int bestEvaluation = 0;

// Scores and sorts moves using MVV-LVA (Most Valuable Victim - Least Valuable Attacker) and the PV Move and Killer Heuristics
static void SortMoves(const Board *board, MoveList *list, Move *pvMove, int ply) {
    int scores[MAX_MOVES];

    for (int i = 0; i < list->count; i++) {
        Move *m = &list->moves[i];
        scores[i] = 0;

        // 1. PV / TT Move (Absolute Priority)
        if (pvMove != NULL && MovesEqual(*m, *pvMove)) {
            scores[i] = 2000000;
            continue;
        }

        // 2. Captures (MVV-LVA)
        if (m->flags & MOVE_CAPTURE) {
            Piece attacker = board->squares[m->fromRow][m->fromCol];
            Piece victim = board->squares[m->toRow][m->toCol];
            if (m->flags & MOVE_EN_PASSANT) {
                victim = (IsWhitePiece(attacker)) ? B_PAWN : W_PAWN;
            }
            scores[i] = 1000000 + (GetPieceValue(victim) * 10) - GetPieceValue(attacker);
            continue;
        }

        // 3. Promotions (Treat as highly as captures)
        if (m->flags & MOVE_PROMOTION) {
            scores[i] = 900000 + GetPieceValue(m->promotion);
            continue;
        }

        // 4. Killer Moves (Quiet moves that caused cutoffs at this ply)
        int killerMatch = IsKillerMove(*m, ply);
        if (killerMatch == 1) {
            scores[i] = 800000;
            continue;
        } else if (killerMatch == 2) {
            scores[i] = 700000;
            continue;
        }

        // 5. History Heuristic (Quiet moves sorted by global historical success)
        int fromSq = m->fromRow * 8 + m->fromCol;
        int toSq = m->toRow * 8 + m->toCol;
        scores[i] = historyTable[board->sideToMove][fromSq][toSq];
    }

    // Selection sort
    for (int i = 0; i < list->count - 1; i++) {
        int bestIdx = i;
        for (int j = i + 1; j < list->count; j++) {
            if (scores[j] > scores[bestIdx]) {
                bestIdx = j;
            }
        }
        if (bestIdx != i) {
            int tempScore = scores[i];
            scores[i] = scores[bestIdx];
            scores[bestIdx] = tempScore;

            Move tempMove = list->moves[i];
            list->moves[i] = list->moves[bestIdx];
            list->moves[bestIdx] = tempMove;
        }
    }
}

// Evaluates tactical captures beyond depth 0 to prevent the Horizon Effect
int QuiescenceSearch(Board *board, History *history, int alpha, int beta, unsigned long long *nodes){

    (*nodes)++;

    // 1. "Stand-pat" evaluation: what is the score if we do nothing?
    int standPat = EvaluatePosition(board);

    // If standing pat is better than beta, the opponent won't allow this position
    if (standPat >= beta) {
        return beta;
    }
    
    // Update alpha if standing pat is our new best minimum expectation
    if (alpha < standPat) {
        alpha = standPat;
    }

    MoveList legalMoves;
    GenerateLegalMoves(board, history, &legalMoves);
    // Q-Search doesn't track PV, so pass NULL
    SortMoves(board, &legalMoves, NULL, 0);

    for (int i = 0; i < legalMoves.count; i++) {
        Move move = legalMoves.moves[i];

        // 2. ONLY consider capture moves in Quiescence Search
        if (!(move.flags & MOVE_CAPTURE)) {
            continue;
        }

        MakeMove(board, history, &move);
        
        // Pass negated beta and alpha down the tree
        int score = -QuiescenceSearch(board, history, -beta, -alpha, nodes);
        
        UndoMove(board, history);

        // Fail-hard beta cutoff
        if (score >= beta) {
            return beta;
        }
        
        // Found a better move
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}

// Searches the position by evaluating every legal move.
int Negamax(Board *board,
            History *history,
            int depth,
            int ply,
            int alpha,
            int beta,
            unsigned long long *nodes)
{
    // Count this position as one searched node.
    (*nodes)++;

    // Initialize PV length for this ply
    pvLength[ply] = ply;

    // Phase 6A: Instead of static EvaluatePosition, drop into Quiescence Search
    if(depth == 0){
        return QuiescenceSearch(board, history, alpha, beta, nodes);
    }

    // Safety check to prevent array out-of-bounds in deep games
    if(ply >= MAX_PLY - 1){
        return EvaluatePosition(board);
    }

    // Track the original alpha to determine node type for TT at the end
    int originalAlpha = alpha;

    // --- 1. PROBE TRANSPOSITION TABLE ---
    Move ttMove = {0};
    int ttScore;
    
    if (TT_Probe(board->hashKey, depth, alpha, beta, &ttScore, &ttMove)) {
        return ttScore; // Instant cutoff! We already calculated this position.
    }
    // ------------------------------------

    MoveList legalMoves;
    GenerateLegalMoves(board, history, &legalMoves);

    // Handle positions with no legal moves.
    if (legalMoves.count == 0){
        // Current side is checkmated.
        if (IsKingInCheck(board, board->sideToMove)){
            return -CHECKMATE_SCORE + ply; // Prefer faster mates using ply
        }

        // Current side is stalemated.
        return DRAW_SCORE;
    }

    // --- 2. MOVE ORDERING (Prefer TT Move over PV Move) ---
    Move *moveToSortFirst = NULL;
    
    // Check if TT returned a valid move (by checking if flags/squares are populated)
    if (ttMove.fromRow != ttMove.toRow || ttMove.fromCol != ttMove.toCol) {
        moveToSortFirst = &ttMove;
    } else if (ply < pvLineLength) {
        moveToSortFirst = &pvLine[ply];
    }

    SortMoves(board, &legalMoves, moveToSortFirst, ply);
    // ------------------------------------------------------

    int bestScore = -INFINITY_SCORE;
    Move bestMove = {0};

    // Try every legal move.
    for (int i = 0; i < legalMoves.count; i++){
        Move move = legalMoves.moves[i];

        // Play the move.
        MakeMove(board, history, &move);

        // Search one ply deeper from the opponent's perspective.
        int score = -Negamax(board,         
                             history,
                             depth - 1,
                             ply + 1,
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

            // Write the PV move to the triangular table
            pvArray[ply][ply] = move;
            // Copy the PV from the deeper plies into this ply's line
            for(int next = ply + 1; next < pvLength[ply + 1]; next++){
                pvArray[ply][next] = pvArray[ply + 1][next];
            }
            pvLength[ply] = pvLength[ply + 1];
        }

        // Stop searching if the remaining moves cannot improve the result (Alpha-Beta Pruning)
        if (alpha >= beta){
            // ------ RECORD HEURISTICS ON BETA CUTOFF ------
            if (!(move.flags & MOVE_CAPTURE)) {
                // 1. Update Killer Moves
                if (!MovesEqual(move, killerMoves[ply][0])) {
                    killerMoves[ply][1] = killerMoves[ply][0]; // Shift old killer down
                    killerMoves[ply][0] = move;                // Store new killer
                }
                
                // 2. Update History Table (reward depth squared)
                int fromSq = move.fromRow * 8 + move.fromCol;
                int toSq = move.toRow * 8 + move.toCol;
                historyTable[board->sideToMove][fromSq][toSq] += (depth * depth);
            }
            // ----------------------------------------------
            break;
        }
    }

    int hashFlag;
    if (bestScore <= originalAlpha) {
        hashFlag = HASH_ALPHA; // Failed low (Move is bad)
    }
    else if (bestScore >= beta) {
        hashFlag = HASH_BETA;  // Failed high (Beta cutoff)
    }
    else {
        hashFlag = HASH_EXACT; // PV Node
    }

    // --- 4. STORE EXACT SCORE IN TT ---
    TT_Store(board->hashKey, depth, bestScore, hashFlag, bestMove);

    return bestScore;
}

// Searches all legal moves and returns the best one.
SearchResult SearchBestMove(Board *board, History *history, int maxDepth){

    SearchResult result;

    result.move = (Move){0};
    result.nodes = 0;
    result.depth = maxDepth;
    result.score = -INFINITY_SCORE;

    pvLineLength = 0; // Reset PV for new search
    
    // Clear heuristics before a new root search
    memset(killerMoves, 0, sizeof(killerMoves));
    memset(historyTable, 0, sizeof(historyTable));

    printf("\n========== ITERATIVE DEEPENING ==========\n");

    // Loop from Depth 1 to maxDepth
    for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++) {
        
        // Reset the root PV length tracker
        pvLength[0] = 0;

        int score = Negamax(board, history, currentDepth, 0, -INFINITY_SCORE, INFINITY_SCORE, &result.nodes);
        
        // Save the best line to feed into the next iteration
        pvLineLength = pvLength[0];
        for (int i = 0; i < pvLineLength; i++) {
            pvLine[i] = pvArray[0][i];
        }

        result.score = score;
        if (pvLineLength > 0) {
            result.move = pvLine[0];
        }

        // Print engine thinking for this depth
        printf("info depth %d score cp %d nodes %llu pv ", currentDepth, score, result.nodes);
        for (int i = 0; i < pvLineLength; i++) {
            char pF = 'a' + pvLine[i].fromCol;
            char pR = '8' - pvLine[i].fromRow;
            char pTF = 'a' + pvLine[i].toCol;
            char pTR = '8' - pvLine[i].toRow;
            printf("%c%c%c%c ", pF, pR, pTF, pTR);
        }
        printf("\n");
    }

    printf("=========================================\n");
    return result;
}

// Finds, executes, and returns the best move for the current side.
SearchResult MakeEngineMove(Board *board,
                            History *history,
                            int depth){

    // Search for the best move.
    SearchResult result = SearchBestMove(board, history, depth);

    // No legal move (checkmate or stalemate)
    if (result.score <= -CHECKMATE_SCORE + 100 ||
        result.score == DRAW_SCORE){
        return result;
    }  

    // Play the selected move.
    MakeMove(board, history, &result.move);

    return result;
}

