#include <stdio.h>

#include "debug.h"

#include "fen.h"
#include "legalmove.h"
#include "attack.h"
#include "gamestate.h"

void Debug_PrintBoardState(const Board *board){
    printf("\n========================================\n");

    Board_Print(board);

    printf("\n");

    PrintFEN(board);

    printf("Side to move : %s\n",
           board->sideToMove == SIDE_WHITE ?
           "WHITE" : "BLACK");

    printf("Castling : ");

    if (board->castling.whiteKingSide)  printf("K");
    if (board->castling.whiteQueenSide) printf("Q");
    if (board->castling.blackKingSide)  printf("k");
    if (board->castling.blackQueenSide) printf("q");

    if (!board->castling.whiteKingSide &&
        !board->castling.whiteQueenSide &&
        !board->castling.blackKingSide &&
        !board->castling.blackQueenSide)
    {
        printf("-");
    }

    printf("\n");

    printf("Halfmove : %d\n", board->halfmoveClock);
    printf("Fullmove : %d\n", board->fullmoveNumber);

    printf("========================================\n");
}

void Debug_PrintLegalMoves(Board *board, History *history){
    MoveList moves;
    MoveList_Init(&moves);

    GenerateLegalMoves(board, history, &moves);

    printf("\nLegal moves (%d)\n", moves.count);

    MoveList_Print(&moves);
}

void Debug_PrintGameState(Board *board, History *history){
    printf("\nGame State\n");

    if (IsKingInCheck(board, board->sideToMove))
        printf("Check : YES\n");
    else
        printf("Check : NO\n");

    MoveList moves;
    MoveList_Init(&moves);

    GenerateLegalMoves(board, history, &moves);

    if (moves.count == 0){
        if (IsKingInCheck(board, board->sideToMove))
            printf("Result : CHECKMATE\n");
        else
            printf("Result : STALEMATE\n");
    }
    else{
        printf("Result : Game continues\n");
    }
}

