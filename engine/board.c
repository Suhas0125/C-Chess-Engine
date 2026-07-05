#include <stdio.h>

#include "board.h"

void Board_Init(Board *board){

    // 1. Empty board
    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){

            board->squares[row][col] = EMPTY;
        }
    }

    /*
                      col
                0 1 2 3 4 5 6 7
              +-----------------+
        row 0 | a8 b8 c8 d8 e8 f8 g8 h8
        row 1 | a7 b7 c7 d7 e7 f7 g7 h7
        row 2 | a6 ...
        ...
        row 6 | a2 ...
        row 7 | a1 b1 c1 d1 e1 f1 g1 h1
    */

    // 2. Black pieces
    board->squares[0][0] = B_ROOK;
    board->squares[0][1] = B_KNIGHT;
    board->squares[0][2] = B_BISHOP;
    board->squares[0][3] = B_QUEEN;
    board->squares[0][4] = B_KING;
    board->squares[0][5] = B_BISHOP;
    board->squares[0][6] = B_KNIGHT;
    board->squares[0][7] = B_ROOK;

    for(int col = 0; col < 8; col++){
        board->squares[1][col] = B_PAWN;
    }

    // 3. White pieces    
    for(int col = 0; col < 8; col++){
        board->squares[6][col] = W_PAWN;
    }

    board->squares[7][0] = W_ROOK;
    board->squares[7][1] = W_KNIGHT;
    board->squares[7][2] = W_BISHOP;
    board->squares[7][3] = W_QUEEN;
    board->squares[7][4] = W_KING;
    board->squares[7][5] = W_BISHOP;
    board->squares[7][6] = W_KNIGHT;
    board->squares[7][7] = W_ROOK;


    // 4. Initialize game state
    board->sideToMove = SIDE_WHITE;

    board->castling.whiteKingSide = 1;
    board->castling.whiteQueenSide = 1;
    board->castling.blackKingSide = 1;
    board->castling.blackQueenSide = 1;

    board->enPassantSquare = -1;

    board->halfmoveClock = 0;
    board->fullmoveNumber = 1;
}

void Board_Print(const Board *board){
    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){

            Piece p = board->squares[row][col];

            char symbol = '.';
            
            switch(p){
                case EMPTY:     symbol = '.'; break;

                case W_PAWN:    symbol = 'P'; break;
                case W_KNIGHT:  symbol = 'N'; break;
                case W_BISHOP:  symbol = 'B'; break;
                case W_ROOK:    symbol = 'R'; break;
                case W_QUEEN:   symbol = 'Q'; break;
                case W_KING:    symbol = 'K'; break;

                case B_PAWN:    symbol = 'p'; break;
                case B_KNIGHT:  symbol = 'n'; break;
                case B_BISHOP:  symbol = 'b'; break;
                case B_ROOK:    symbol = 'r'; break;
                case B_QUEEN:   symbol = 'q'; break;
                case B_KING:    symbol = 'k'; break;
            }

            printf("%c ", symbol);
        }
        printf("\n"); // newline when row is completed
    }
    printf("\n"); // newline after whole board
}