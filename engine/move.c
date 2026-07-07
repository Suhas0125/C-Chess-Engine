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

// Prints every move in a MoveList (for debugging).
void MoveList_Print(const MoveList *list){
    printf("Move count: %d\n\n", list->count);

    for (int i = 0; i < list->count; i++){
        const Move *move = &list->moves[i];

        char fromFile = 'a' + move->fromCol;
        char fromRank = '8' - move->fromRow;

        char toFile = 'a' + move->toCol;
        char toRank = '8' - move->toRow;

        printf("%2d. %c%c -> %c%c",
               i + 1,
               fromFile, fromRank,
               toFile, toRank);

        if (move->flags & MOVE_PROMOTION){
            printf(" (promotion)");
        }

        if (move->flags & MOVE_CASTLING){
            printf(" (castling)");
        }

        if (move->flags & MOVE_EN_PASSANT){
            printf(" (en passant)");
        }

        printf("\n");
    }
}

