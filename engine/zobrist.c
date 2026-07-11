#include "zobrist.h"

// Tables holding random 64-bit numbers for every feature
static uint64_t pieceKeys[13][64]; // 13 pieces (EMPTY to B_KING), 64 squares
static uint64_t sideKey;           // XORed if it is Black's turn
static uint64_t castleKeys[16];    // 16 possible castling combinations (0000 to 1111)
static uint64_t epKeys[64];        // 64 possible en passant squares

// Simple 64-bit Pseudo-Random Number Generator (PRNG)
static uint64_t prng_state = 1070372ULL;

static uint64_t random64() {
    prng_state ^= prng_state >> 12;
    prng_state ^= prng_state << 25;
    prng_state ^= prng_state >> 27;
    return prng_state * 2685821657736338717ULL;
}

void Zobrist_Init(void) {
    // Fill piece keys
    for (int p = 0; p < 13; p++) {
        for (int sq = 0; sq < 64; sq++) {
            pieceKeys[p][sq] = random64();
        }
    }
    // Fill side key
    sideKey = random64();
    
    // Fill castling keys
    for (int i = 0; i < 16; i++) {
        castleKeys[i] = random64();
    }
    
    // Fill en passant keys
    for (int i = 0; i < 64; i++) {
        epKeys[i] = random64();
    }
}

unsigned long long Zobrist_GenerateKey(const Board *board) {
    uint64_t finalKey = 0;

    // 1. Pieces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece piece = board->squares[row][col];
            if (piece != EMPTY) {
                int sq = row * 8 + col;
                finalKey ^= pieceKeys[piece][sq];
            }
        }
    }

    // 2. Side to move (Hash only if it's Black's turn)
    if (board->sideToMove == SIDE_BLACK) {
        finalKey ^= sideKey;
    }

    // 3. Castling rights (convert 4 ints into a 4-bit index 0-15)
    int castleIndex = 0;
    if (board->castling.whiteKingSide)  castleIndex |= 1;
    if (board->castling.whiteQueenSide) castleIndex |= 2;
    if (board->castling.blackKingSide)  castleIndex |= 4;
    if (board->castling.blackQueenSide) castleIndex |= 8;
    
    finalKey ^= castleKeys[castleIndex];

    // 4. En Passant square
    if (board->enPassantSquare != -1) {
        finalKey ^= epKeys[board->enPassantSquare];
    }

    return finalKey;
}