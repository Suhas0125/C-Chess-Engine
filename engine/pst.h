#ifndef PST_H
#define PST_H

// Piece-Square Tables.
//
// Values are from White's perspective.
// Black pieces use the mirrored table.

extern const int PawnPST[8][8];
extern const int KnightPST[8][8];
extern const int BishopPST[8][8];
extern const int RookPST[8][8];
extern const int QueenPST[8][8];
extern const int KingPST[8][8];

    // extern tells the compiler:
    // "The table exists somewhere else."

#endif