#ifndef DIRECTIONS_H
#define DIRECTIONS_H

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

#endif