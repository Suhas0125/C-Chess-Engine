#include "movegen.h"

// All possible knight movement offsets.
static const int knightMoves[8][2] = {
    {-2, -1}, {-2,  1},
    {-1, -2}, {-1,  2},
    { 1, -2}, { 1,  2},
    { 2, -1}, { 2,  1}
};

// All bishop movement directions.
static const int bishopDirections[4][2] = {
    {-1, -1},
    {-1,  1},
    { 1, -1},
    { 1,  1}
};

// All rook movement directions.
static const int rookDirections[4][2] = {
    {-1,  0},
    { 1,  0},
    { 0, -1},
    { 0,  1}
};

// All queen movement directions.
static const int queenDirections[8][2] = {
    {-1, -1}, {-1,  1},
    { 1, -1}, { 1,  1},
    {-1,  0}, { 1,  0},
    { 0, -1}, { 0,  1} // all 8 directions
};

// All king movement directions.
static const int kingDirections[8][2] = {
    {-1,-1}, {-1,0}, {-1,1},
    { 0,-1},         { 0,1},
    { 1,-1}, { 1,0}, { 1,1}
};

// Generates all pseudo-legal moves.
void GenerateMoves(const Board *board, MoveList *list){
    MoveList_Init(list);

    GeneratePawnMoves(board, list);
    GenerateEnPassantMoves(board, list);

    GenerateKnightMoves(board, list);
    GenerateBishopMoves(board, list);
    GenerateRookMoves(board, list);
    GenerateQueenMoves(board, list);
    GenerateKingMoves(board, list);
}

// Generates all pawn moves.
void GeneratePawnMoves(const Board *board, MoveList *list){

    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            Piece piece = board->squares[row][col];

            // Only generate moves for the side to move.
            if (board->sideToMove == SIDE_WHITE && piece != W_PAWN)
                continue;

            if (board->sideToMove == SIDE_BLACK && piece != B_PAWN)
                continue;

            // White pawn moves upward.
            if (piece == W_PAWN){
                int nextRow = row - 1;

                // Single forward move.
                if (nextRow >= 0 &&
                    board->squares[nextRow][col] == EMPTY){
                    // Generate promotion moves.
                    if (nextRow == 0){
                        MoveList_Add(list, row, col, nextRow, col, W_QUEEN, MOVE_PROMOTION);
                        MoveList_Add(list, row, col, nextRow, col, W_ROOK, MOVE_PROMOTION);
                        MoveList_Add(list, row, col, nextRow, col, W_BISHOP, MOVE_PROMOTION);
                        MoveList_Add(list, row, col, nextRow, col, W_KNIGHT, MOVE_PROMOTION);
                    }
                    else{
                        MoveList_Add(list, row, col, nextRow, col, EMPTY, MOVE_NONE);
                    }
                }

                // Double forward move from the starting rank.
                if (row == 6 &&
                    board->squares[5][col] == EMPTY &&
                    board->squares[4][col] == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        4, col,
                        EMPTY,
                        MOVE_DOUBLE_PAWN
                    );
                }

                // Capture left
                if (nextRow >= 0 && col - 1 >= 0){
                    Piece target = board->squares[nextRow][col - 1];

                    if (IsBlackPiece(target)){
                        // Generate promotion capture moves.
                        if (nextRow == 0){
                            MoveList_Add(list, row, col, nextRow, col - 1, W_QUEEN,  MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col - 1, W_ROOK,   MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col - 1, W_BISHOP, MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col - 1, W_KNIGHT, MOVE_CAPTURE | MOVE_PROMOTION);
                        }
                        else{
                            MoveList_Add(list, row, col, nextRow, col - 1, EMPTY, MOVE_CAPTURE);
                        }
                    }
                }

                // Capture right.
                if (nextRow >= 0 && col + 1 < 8){
                    Piece target = board->squares[nextRow][col + 1];

                    if (IsBlackPiece(target)){
                        // Generate promotion capture moves.
                        if (nextRow == 0){
                            MoveList_Add(list, row, col, nextRow, col + 1, W_QUEEN,  MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col + 1, W_ROOK,   MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col + 1, W_BISHOP, MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col + 1, W_KNIGHT, MOVE_CAPTURE | MOVE_PROMOTION);
                        }
                        else{
                            MoveList_Add(list, row, col, nextRow, col + 1, EMPTY, MOVE_CAPTURE);
                        }
                    }
                }
            }

            // Black pawn moves downward.
            if (piece == B_PAWN){
                int nextRow = row + 1;

                // Single forward move.
                if (nextRow < 8 &&
                    board->squares[nextRow][col] == EMPTY){
                    // Generate promotion moves.
                    if (nextRow == 7){
                        MoveList_Add(list, row, col, nextRow, col, B_QUEEN, MOVE_PROMOTION);
                        MoveList_Add(list, row, col, nextRow, col, B_ROOK, MOVE_PROMOTION);
                        MoveList_Add(list, row, col, nextRow, col, B_BISHOP, MOVE_PROMOTION);
                        MoveList_Add(list, row, col, nextRow, col, B_KNIGHT, MOVE_PROMOTION);
                    }
                    else{
                        MoveList_Add(list, row, col, nextRow, col, EMPTY, MOVE_NONE);
                    }
                }

                // Double forward move from the starting rank.
                if (row == 1 &&
                    board->squares[2][col] == EMPTY &&
                    board->squares[3][col] == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        3, col,
                        EMPTY,
                        MOVE_DOUBLE_PAWN
                    );
                }

                // Capture left.
                if (nextRow < 8 && col - 1 >= 0){
                    Piece target = board->squares[nextRow][col - 1];

                    if (IsWhitePiece(target)){
                        // Generate promotion capture moves.
                        if (nextRow == 7){
                            MoveList_Add(list, row, col, nextRow, col - 1, B_QUEEN,  MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col - 1, B_ROOK,   MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col - 1, B_BISHOP, MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col - 1, B_KNIGHT, MOVE_CAPTURE | MOVE_PROMOTION);
                        }
                        else{
                            MoveList_Add(list, row, col, nextRow, col - 1, EMPTY, MOVE_CAPTURE);
                        }
                    }
                }

                // Capture right.
                if (nextRow < 8 && col + 1 < 8){
                    Piece target = board->squares[nextRow][col + 1];

                    if (IsWhitePiece(target)){
                        // Generate promotion capture moves.
                        if (nextRow == 7){
                            MoveList_Add(list, row, col, nextRow, col + 1, B_QUEEN,  MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col + 1, B_ROOK,   MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col + 1, B_BISHOP, MOVE_CAPTURE | MOVE_PROMOTION);
                            MoveList_Add(list, row, col, nextRow, col + 1, B_KNIGHT, MOVE_CAPTURE | MOVE_PROMOTION);
                        }
                        else{
                            MoveList_Add(list, row, col, nextRow, col + 1, EMPTY, MOVE_CAPTURE);
                        }
                    }
                }
            }
        }
    }
}

// Generates all knight pseudo-legal moves.
void GenerateKnightMoves(const Board *board, MoveList *list){
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            Piece piece = board->squares[row][col];

            // Ignore non-knights.
            if (piece != W_KNIGHT && piece != B_KNIGHT)
                continue;

            // Only generate moves for the side to move.
            if (board->sideToMove == SIDE_WHITE && piece != W_KNIGHT)
                continue;

            if (board->sideToMove == SIDE_BLACK && piece != B_KNIGHT)
                continue;

            bool whiteKnight = IsWhitePiece(piece);

            // Check all 8 possible moves.
            for (int i = 0; i < 8; i++){
                int newRow = row + knightMoves[i][0];
                int newCol = col + knightMoves[i][1];

                // Ignore moves outside the board.
                if (newRow < 0 || newRow > 7 || newCol < 0 || newCol > 7) continue;

                Piece target = board->squares[newRow][newCol];

                // Empty square.
                if (target == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        newRow, newCol,
                        EMPTY,
                        MOVE_NONE
                    );
                }

                // Capture enemy piece.
                else if ((whiteKnight && IsBlackPiece(target)) || (!whiteKnight && IsWhitePiece(target))){
                    MoveList_Add(
                        list,
                        row, col,
                        newRow, newCol,
                        EMPTY,
                        MOVE_CAPTURE
                    );
                }
            }
        }
    }
}

// Generates sliding piece moves.
void GenerateSlidingMoves(const Board *board, MoveList *list, int row, int col, const int directions[][2], int directionCount){

    Piece piece = board->squares[row][col];
    bool whitePiece = IsWhitePiece(piece);

    for (int dir = 0; dir < directionCount; dir++){
        int dr = directions[dir][0]; // row direction
        int dc = directions[dir][1]; // col direction

        int newRow = row + dr;
        int newCol = col + dc;

        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8){

            Piece target = board->squares[newRow][newCol];

            // Empty square.
            if (target == EMPTY){
                MoveList_Add(
                    list,
                    row, col,
                    newRow, newCol,
                    EMPTY,
                    MOVE_NONE
                );
            }

            // Enemy piece.
            else if ((whitePiece && IsBlackPiece(target)) || (!whitePiece && IsWhitePiece(target))){
                MoveList_Add(
                    list,
                    row, col,
                    newRow, newCol,
                    EMPTY,
                    MOVE_CAPTURE
                );
                break;
            }
            
            // Friendly piece.
            else{
                break;
            }

            newRow += dr;
            newCol += dc;
        }
    }
}

// Generates bishop pseudo-legal moves.
void GenerateBishopMoves(const Board *board, MoveList *list){
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){

            Piece piece = board->squares[row][col];

            // Ignore non-bishops.
            if (piece != W_BISHOP && piece != B_BISHOP)
                continue;

            // Only generate moves for the side to move.
            if (board->sideToMove == SIDE_WHITE && piece != W_BISHOP)
                continue;

            if (board->sideToMove == SIDE_BLACK && piece != B_BISHOP)
                continue;

            GenerateSlidingMoves(
                board,
                list,
                row,
                col,
                bishopDirections,
                4
            );
        }
    }
}

// Generates rook pseudo-legal moves.
void GenerateRookMoves(const Board *board, MoveList *list){
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){

            Piece piece = board->squares[row][col];

            // Ignore non-rooks.
            if (piece != W_ROOK && piece != B_ROOK)
                continue;

            // Only generate moves for the side to move.
            if (board->sideToMove == SIDE_WHITE && piece != W_ROOK)
                continue;

            if (board->sideToMove == SIDE_BLACK && piece != B_ROOK)
                continue;

            GenerateSlidingMoves(
                board,
                list,
                row,
                col,
                rookDirections,
                4
            );
        }
    }
}

// Generates queen pseudo-legal moves.
void GenerateQueenMoves(const Board *board, MoveList *list){
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){

            Piece piece = board->squares[row][col];

            // Ignore non-queens.
            if (piece != W_QUEEN && piece != B_QUEEN)
                continue;

            // Only generate moves for the side to move.
            if (board->sideToMove == SIDE_WHITE && piece != W_QUEEN)
                continue;

            if (board->sideToMove == SIDE_BLACK && piece != B_QUEEN)
                continue;

            GenerateSlidingMoves(
                board,
                list,
                row,
                col,
                queenDirections,
                8
            );
        }
    }
}

// Generates king pseudo-legal moves.
void GenerateKingMoves(const Board *board, MoveList *list){
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){

            Piece piece = board->squares[row][col];

            // Ignore non-kings.
            if (piece != W_KING && piece != B_KING)
                continue;

            // Only generate moves for the side to move.
            if (board->sideToMove == SIDE_WHITE && piece != W_KING)
                continue;

            if (board->sideToMove == SIDE_BLACK && piece != B_KING)
                continue;
            
            bool whiteKing = IsWhitePiece(piece);

            // Check all adjacent squares.
            for (int i = 0; i < 8; i++){
                int newRow = row + kingDirections[i][0];
                int newCol = col + kingDirections[i][1];

                // Ignore moves outside the board.
                if (newRow < 0 || newRow > 7 || newCol < 0 || newCol > 7){
                    continue;
                }

                Piece target = board->squares[newRow][newCol];

                // Empty square.
                if (target == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        newRow, newCol,
                        EMPTY,
                        MOVE_NONE
                    );
                }

                // Capture enemy piece.
                else if ((whiteKing && IsBlackPiece(target)) || (!whiteKing && IsWhitePiece(target))){
                    MoveList_Add(
                        list,
                        row, col,
                        newRow, newCol,
                        EMPTY,
                        MOVE_CAPTURE
                    );
                }
            }

            // Generate white castling moves.
            if (piece == W_KING){
                // Kingside castling.
                if (board->castling.whiteKingSide && board->squares[7][5] == EMPTY && board->squares[7][6] == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        row, col + 2,
                        EMPTY,
                        MOVE_CASTLING
                    );
                }

                // Queenside castling.
                if (board->castling.whiteQueenSide && board->squares[7][3] == EMPTY && board->squares[7][2] == EMPTY &&
                    board->squares[7][1] == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        row, col - 2,
                        EMPTY,
                        MOVE_CASTLING
                    );
                }
            }

            // Generate black castling moves.
            if (piece == B_KING){
                // Kingside castling.
                if (board->castling.blackKingSide &&
                    board->squares[0][5] == EMPTY &&
                    board->squares[0][6] == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        row, col + 2,
                        EMPTY,
                        MOVE_CASTLING
                    );
                }

                // Queenside castling.
                if (board->castling.blackQueenSide &&
                    board->squares[0][3] == EMPTY &&
                    board->squares[0][2] == EMPTY &&
                    board->squares[0][1] == EMPTY){
                    MoveList_Add(
                        list,
                        row, col,
                        row, col - 2,
                        EMPTY,
                        MOVE_CASTLING
                    );
                }
            }
        }
    }
}

// Generates pseudo-legal en passant moves.
void GenerateEnPassantMoves(const Board *board, MoveList *list){
    // No en passant available.
    if (board->enPassantSquare == -1)
        return;

    int epRow = board->enPassantSquare / 8;
    int epCol = board->enPassantSquare % 8;

    // White to move.
    if (board->sideToMove == SIDE_WHITE){
        int pawnRow = epRow + 1;

        if (pawnRow <= 7){
            // Pawn on the left.
            if (epCol > 0 && board->squares[pawnRow][epCol - 1] == W_PAWN){
                MoveList_Add(
                    list,
                    pawnRow, epCol - 1,
                    epRow, epCol,
                    EMPTY,
                    MOVE_EN_PASSANT
                );
            }

            // Pawn on the right.
            if (epCol < 7 && board->squares[pawnRow][epCol + 1] == W_PAWN){
                MoveList_Add(
                    list,
                    pawnRow, epCol + 1,
                    epRow, epCol,
                    EMPTY,
                    MOVE_EN_PASSANT
                );
            }
        }
    }

    // Black to move.
    else{
        int pawnRow = epRow - 1;

        if (pawnRow >= 0){
            // Pawn on the left.
            if (epCol > 0 && board->squares[pawnRow][epCol - 1] == B_PAWN){
                MoveList_Add(
                    list,
                    pawnRow, epCol - 1,
                    epRow, epCol,
                    EMPTY,
                    MOVE_EN_PASSANT
                );
            }

            // Pawn on the right.
            if (epCol < 7 && board->squares[pawnRow][epCol + 1] == B_PAWN){
                MoveList_Add(
                    list,
                    pawnRow, epCol + 1,
                    epRow, epCol,
                    EMPTY,
                    MOVE_EN_PASSANT
                );
            }
        }
    }
}

