#include "makemove.h"

// Executes a move on the board.
void MakeMove(Board *board,History *history, const Move *move){
    // Save the current position before modifying it.
    History_Push(history, board);
    
    // Get the moving piece.
    Piece piece = board->squares[move->fromRow][move->fromCol];

    // Get the captured piece.
    Piece captured = board->squares[move->toRow][move->toCol];

    // Remove the piece from its starting square.
    board->squares[move->fromRow][move->fromCol] = EMPTY;

    //--------------PROMOTION------------
    // Handle pawn promotion.
    if (move->flags & MOVE_PROMOTION){
        board->squares[move->toRow][move->toCol] = move->promotion;
    }
    else{
        // Normal move.
        board->squares[move->toRow][move->toCol] = piece;
    }
    
    //------------------EN PASSANT--------------------
    // Remove the captured pawn during an en passant capture.
    if (move->flags & MOVE_EN_PASSANT){
        // White captured en passant.
        if (piece == W_PAWN){
            board->squares[move->toRow + 1][move->toCol] = EMPTY;
        }

        // Black captured en passant.
        else if (piece == B_PAWN){
            board->squares[move->toRow - 1][move->toCol] = EMPTY;
        }
    }

    // Clear the en passant square.
    board->enPassantSquare = -1;

    // Set the en passant target square after a double pawn push.
    if (move->flags & MOVE_DOUBLE_PAWN){
        // White pawn moved two squares.
        if (piece == W_PAWN){
            board->enPassantSquare = (move->fromRow - 1) * 8 + move->fromCol;
        }

        // Black pawn moved two squares.
        else if (piece == B_PAWN){
            board->enPassantSquare = (move->fromRow + 1) * 8 + move->fromCol;
        }
    }

    //-------------------CASTLING--------------------
    // Move the rook during castling.
    if (move->flags & MOVE_CASTLING){

        // White kingside: e1 -> g1
        if (piece == W_KING && move->toCol == 6){
            board->squares[7][5] = W_ROOK;
            board->squares[7][7] = EMPTY;
        }

        // White queenside: e1 -> c1
        else if (piece == W_KING && move->toCol == 2){
            board->squares[7][3] = W_ROOK;
            board->squares[7][0] = EMPTY;
        }

        // Black kingside: e8 -> g8
        else if (piece == B_KING && move->toCol == 6){
            board->squares[0][5] = B_ROOK;
            board->squares[0][7] = EMPTY;
        }

        // Black queenside: e8 -> c8
        else if (piece == B_KING && move->toCol == 2){
            board->squares[0][3] = B_ROOK;
            board->squares[0][0] = EMPTY;
        }
    }

    // ----Update castling rights----

    // White king moved.
    if (piece == W_KING){
        board->castling.whiteKingSide = 0;
        board->castling.whiteQueenSide = 0;
    }

    // Black king moved.
    else if (piece == B_KING){
        board->castling.blackKingSide = 0;
        board->castling.blackQueenSide = 0;
    }

    // White rook moved.
    else if (piece == W_ROOK){
        if (move->fromRow == 7 && move->fromCol == 0){
            board->castling.whiteQueenSide = 0;
        }

        if (move->fromRow == 7 && move->fromCol == 7){
            board->castling.whiteKingSide = 0;
        }
    }

    // Black rook moved.
    else if (piece == B_ROOK){
        if (move->fromRow == 0 && move->fromCol == 0){
            board->castling.blackQueenSide = 0;
        }

        if (move->fromRow == 0 && move->fromCol == 7){
            board->castling.blackKingSide = 0;
        }
    }

    // Captured white rook.
    if (captured == W_ROOK){
        if (move->toRow == 7 && move->toCol == 0){
            board->castling.whiteQueenSide = 0;
        }

        if (move->toRow == 7 && move->toCol == 7){
            board->castling.whiteKingSide = 0;
        }
    }

    // Captured black rook.
    else if (captured == B_ROOK){
        if (move->toRow == 0 && move->toCol == 0){
            board->castling.blackQueenSide = 0;
        }

        if (move->toRow == 0 && move->toCol == 7){
            board->castling.blackKingSide = 0;
        }
    }

    // Reset halfmove clock after pawn move or capture.
    if (piece == W_PAWN || piece == B_PAWN || captured != EMPTY){
        board->halfmoveClock = 0;
    }
    else{
        board->halfmoveClock++;
    }

    // Increase fullmove number after Black moves.
    if (board->sideToMove == SIDE_BLACK) board->fullmoveNumber++;

    // Switch side to move.
    board->sideToMove = (board->sideToMove == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;
}

// Restores the previous board state.
int UndoMove(Board *board, History *history){
    return History_Pop(history, board);
}