#include "board.h"
#include "attack.h"
#include "directions.h"

// Returns 1 if the given square is attacked by the specified side.
int IsSquareAttacked(const Board *board, int row, int col, Side attacker){
    // ---------- White pawn attacks ----------
    if (attacker == SIDE_WHITE){
        // Using reverse lookups:
        // if a white pawn attacks (row, col), it must be located at:
        // (row + 1, col - 1)
        // (row + 1, col + 1)
        if (row + 1 < 8){
            if (col - 1 >= 0 && board->squares[row + 1][col - 1] == W_PAWN){
                return 1;
            }

            if (col + 1 < 8 && board->squares[row + 1][col + 1] == W_PAWN){
                return 1;
            }
        }
    }

    // ---------- Black pawn attacks ----------
    else{
        // if a black pawn attacks (row, col), it must be located at:
        // (row - 1, col - 1)
        // (row - 1, col + 1)
        if (row - 1 >= 0){
            if (col - 1 >= 0 && board->squares[row - 1][col - 1] == B_PAWN){
                return 1;
            }

            if (col + 1 < 8 && board->squares[row - 1][col + 1] == B_PAWN)
            {
                return 1;
            }
        }
    }

        // ---------- Knight attacks ----------
    Piece knight = (attacker == SIDE_WHITE) ? W_KNIGHT : B_KNIGHT;

    for (int i = 0; i < 8; i++){
        int r = row + knightMoves[i][0];
        int c = col + knightMoves[i][1];

        if (r >= 0 && r < 8 && c >= 0 && c < 8 && board->squares[r][c] == knight){
            return 1;
        }
    }

    // ---------- Bishop attacks ----------
    Piece bishop = (attacker == SIDE_WHITE) ? W_BISHOP : B_BISHOP;

    for (int dir = 0; dir < 4; dir++){
        int r = row + bishopDirections[dir][0];
        int c = col + bishopDirections[dir][1];

        while (r >= 0 && r < 8 && c >= 0 && c < 8){
            Piece piece = board->squares[r][c];

            if (piece != EMPTY){
                if (piece == bishop){
                    return 1;
                }

                // Any piece blocks further attacks.
                break;
            }

            r += bishopDirections[dir][0];
            c += bishopDirections[dir][1];
        }
    }

    // ---------- Rook attacks ----------
    Piece rook = (attacker == SIDE_WHITE) ? W_ROOK : B_ROOK;

    for (int dir = 0; dir < 4; dir++){
        int r = row + rookDirections[dir][0];
        int c = col + rookDirections[dir][1];

        while (r >= 0 && r < 8 && c >= 0 && c < 8){
            Piece piece = board->squares[r][c];

            if (piece != EMPTY){
                if (piece == rook){
                    return 1;
                }

                // Any piece blocks further attacks.
                break;
            }

            r += rookDirections[dir][0];
            c += rookDirections[dir][1];
        }
    }

    // ---------- Queen attacks ----------
    Piece queen = (attacker == SIDE_WHITE) ? W_QUEEN : B_QUEEN;

    for (int dir = 0; dir < 8; dir++){
        int r = row + queenDirections[dir][0];
        int c = col + queenDirections[dir][1];

        while (r >= 0 && r < 8 && c >= 0 && c < 8){
            Piece piece = board->squares[r][c];

            if (piece != EMPTY){
                if (piece == queen){
                    return 1;
                }

                // Any piece blocks further attacks.
                break;
            }

            r += queenDirections[dir][0];
            c += queenDirections[dir][1];
        }
    }

    // ---------- King attacks ----------
    Piece king = (attacker == SIDE_WHITE) ? W_KING : B_KING;

    for (int i = 0; i < 8; i++){
        int r = row + kingDirections[i][0];
        int c = col + kingDirections[i][1];

        if (r >= 0 && r < 8 && c >= 0 && c < 8 && board->squares[r][c] == king){
            return 1;
        }
    }

    return 0;
}

// Returns 1 if the specified side's king is currently in check.
int IsKingInCheck(const Board *board, Side side){
    Piece king = (side == SIDE_WHITE) ? W_KING : B_KING;

    int kingRow = -1;
    int kingCol = -1;

    // Find the king.
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            if (board->squares[row][col] == king){
                kingRow = row;
                kingCol = col;
                break;
            }
        }

        if (kingRow != -1){
            break;
        }
    }

    // Safety check (should never happen in a valid game).
    if (kingRow == -1){
        return 0;
    }

    Side opponent = (side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;

    return IsSquareAttacked(board, kingRow, kingCol, opponent);
}