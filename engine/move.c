#include "move.h"

// Clears move list
void MoveList_Init(MoveList* list){
    list->count = 0;
}

// Adds a move
void MoveList_Add(MoveList* list, int fromRow, int fromCol, int toRow, int toCol, Piece promotion, int flags){
    
    if(list->count >= MAX_MOVES) return;

    Move *move = &list->moves[list->count];

    move->fromRow = fromRow;
    move->fromCol = fromCol;

    move->toRow = toRow;
    move->toCol = toCol;

    move->promotion = promotion;
    move->flags = flags;

    list->count++;
}