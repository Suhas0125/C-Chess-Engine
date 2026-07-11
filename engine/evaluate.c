#include "evaluate.h"
#include "pst.h"

// Returns the material value of a piece.
// This function is private to evaluate.c.
int GetPieceValue(Piece piece)
{
    switch (piece)
    {
        case W_PAWN:
        case B_PAWN:
            return 100;

        case W_KNIGHT:
        case B_KNIGHT:
            return 320;

        case W_BISHOP:
        case B_BISHOP:
            return 330;

        case W_ROOK:
        case B_ROOK:
            return 500;

        case W_QUEEN:
        case B_QUEEN:
            return 900;

        case W_KING:
        case B_KING:
            return 0;

        default:
            return 0;
    }
}

// Returns the positional bonus for a piece.
static int GetPieceSquareValue(Piece piece, int row, int col){

    const int (*table)[8] = NULL;

    switch (piece){

        case W_PAWN:
        case B_PAWN:
            table = PawnPST;
            break;

        case W_KNIGHT:
        case B_KNIGHT:
            table = KnightPST;
            break;

        case W_BISHOP:
        case B_BISHOP:
            table = BishopPST;
            break;

        case W_ROOK:
        case B_ROOK:
            table = RookPST;
            break;

        case W_QUEEN:
        case B_QUEEN:
            table = QueenPST;
            break;

        case W_KING:
        case B_KING:
            table = KingPST;
            break;

        default:
            return 0;
    }

    // White uses the table directly.
    if (IsWhitePiece(piece)){
        return table[row][col];
    }

    // Black uses the mirrored table.
    return table[7 - row][col];
}

// Evaluates the current board position using material and piece-square tables.
int EvaluatePosition(const Board *board){
    int score = 0;

    // Scan every square on the board.
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            Piece piece = board->squares[row][col];

            // Add material and positional bonus for White pieces.
            if (IsWhitePiece(piece)){
                score += GetPieceValue(piece);
                score += GetPieceSquareValue(piece, row, col);
            }

            // Subtract material and positional bonus for Black pieces.
            else if (IsBlackPiece(piece)){
                score -= GetPieceValue(piece);
                score -= GetPieceSquareValue(piece, row, col);
            }
        }
    }

    // Return the evaluation from the perspective of the side to move.
    return (board->sideToMove == SIDE_WHITE) ? score : -score;
}