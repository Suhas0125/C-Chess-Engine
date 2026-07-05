#ifndef BOARD_H
#define BOARD_H


// Every possible piece that can occupy a square on the board
// -------------------PIECES----------------------
typedef enum{
    EMPTY,

    W_PAWN,
    W_KNIGHT,
    W_BISHOP,
    W_ROOK,
    W_QUEEN,
    W_KING,

    B_PAWN,
    B_KNIGHT,
    B_BISHOP,
    B_ROOK,
    B_QUEEN,
    B_KING

} Piece;


//-----------------WHOSE TURN--------------------
typedef enum{
    SIDE_WHITE,
    SIDE_BLACK
} Side;


//----------------CASTLING RIGHTS-----------------
typedef struct{

    int whiteKingSide;
    int whiteQueenSide;
    int blackKingSide;
    int blackQueenSide;

} CastlingRights;


//-----------------BOARD--------------------
typedef struct{
    Piece squares[8][8];

    Side sideToMove;

    CastlingRights castling;

    int enPassantSquare; // -1 = none

    int halfmoveClock;
    int fullmoveNumber;

} Board;


void Board_Init(Board *board);

void Board_Print(const Board *board);

#endif


/*

#ifndef  }
#define  } => are called an Include guard
#endif   }


As projects grow, multiple .c files may include the same header.
Without an include guard, the compiler could accidentally read the same header more than once, causing errors like:
redefinition of enum Piece

The include guard tells the compiler:
"If you've already included this file once, don't include it again."

*/