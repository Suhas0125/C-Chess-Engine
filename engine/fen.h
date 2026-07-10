#ifndef FEN_H
#define FEN_H

#include "board.h"

// given board, construct fen
void Board_ToFEN(const Board *board, char* fen);

// given fen, construct board
int Board_FromFEN(Board* board, const char* fen);

// print fen string of the current board
void PrintFEN(const Board *board);

#endif