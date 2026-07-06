#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "move.h"

// Generates all pseudo legal moves
void GenerateMoves(const Board *board, MoveList *list);

// Generates all pawn moves
void GeneratePawnMoves(const Board *board, MoveList *list);

// Generates en passant moves.
void GenerateEnPassantMoves(const Board *board, MoveList *list);

// Generate all knight moves
void GenerateKnightMoves(const Board *board, MoveList *list);

// Generates sliding moves in the given directions.
void GenerateSlidingMoves(
    const Board *board,
    MoveList *list,
    int row,
    int col,
    const int directions[][2],
    int directionCount
);

// Generates bishop moves.
void GenerateBishopMoves(const Board *board, MoveList *list);

// Generates rook moves.
void GenerateRookMoves(const Board *board, MoveList *list);

// Generates queen moves.
void GenerateQueenMoves(const Board *board, MoveList *list);

// Generates king moves.
void GenerateKingMoves(const Board *board, MoveList *list);

#endif