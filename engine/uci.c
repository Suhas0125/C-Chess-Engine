#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "uci.h"
#include "board.h"
#include "fen.h"
#include "makemove.h"
#include "legalmove.h"
#include "search.h"
#include "history.h"
#include "tt.h"
#include "time_utils.h"

#define INPUT_BUFFER_SIZE 2048

// Helper to convert UCI move string (e.g., "e2e4" or "e7e8q") to an internal Move struct
static bool ParseAndMakeMove(Board *board, History *history, char *moveStr) {
    MoveList list;
    MoveList_Init(&list);
    GenerateLegalMoves(board, history, &list);

    int fromCol = moveStr[0] - 'a';
    int fromRow = 7 - (moveStr[1] - '1');
    int toCol   = moveStr[2] - 'a';
    int toRow   = 7 - (moveStr[3] - '1');
    
    char promotedPiece = '\0';
    if (strlen(moveStr) >= 5) {
        promotedPiece = moveStr[4];
    }

    for (int i = 0; i < list.count; i++) {
        Move m = list.moves[i];
        if (m.fromRow == fromRow && m.fromCol == fromCol &&
            m.toRow == toRow && m.toCol == toCol) {
            
            // Match the requested promotion piece
            if (promotedPiece != '\0') {
                if (promotedPiece == 'q' && m.promotion != W_QUEEN && m.promotion != B_QUEEN) continue;
                if (promotedPiece == 'r' && m.promotion != W_ROOK && m.promotion != B_ROOK) continue;
                if (promotedPiece == 'b' && m.promotion != W_BISHOP && m.promotion != B_BISHOP) continue;
                if (promotedPiece == 'n' && m.promotion != W_KNIGHT && m.promotion != B_KNIGHT) continue;
            }
            
            MakeMove(board, history, &m);
            return true;
        }
    }
    return false;
}

// Parses "position startpos moves e2e4 e7e5" or "position fen <fen_string> moves <moves>"
static void ParsePosition(char *command, Board *board, History *history) {
    command += 9; // Skip "position "
    char *current = command;

    if (strncmp(current, "startpos", 8) == 0) {
        Board_FromFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        History_Init(history);
        current += 8;
    } else if (strncmp(current, "fen", 3) == 0) {
        current += 4; // Skip "fen "
        Board_FromFEN(board, current);
        History_Init(history);
        
        while (*current && strncmp(current, "moves", 5) != 0) {
            current++;
        }
    }

    char *movesPos = strstr(current, "moves");
    if (movesPos != NULL) {
        movesPos += 6; // Skip "moves "
        char *token = strtok(movesPos, " ");
        while (token != NULL) {
            ParseAndMakeMove(board, history, token);
            token = strtok(NULL, " ");
        }
    }
}

// Updated ParseGo with Time Management
static void ParseGo(char *command, Board *board, History *history) {
    int depth = 64; // Default to deep search if using time controls
    long long wtime = -1, btime = -1, winc = 0, binc = 0;
    
    char *ptr = NULL;
    
    if ((ptr = strstr(command, "depth")) != NULL) depth = atoi(ptr + 6);
    
    if ((ptr = strstr(command, "wtime")) != NULL) wtime = atoi(ptr + 6);
    if ((ptr = strstr(command, "btime")) != NULL) btime = atoi(ptr + 6);
    if ((ptr = strstr(command, "winc")) != NULL) winc = atoi(ptr + 5);
    if ((ptr = strstr(command, "binc")) != NULL) binc = atoi(ptr + 5);

    long long allottedTime = -1;

    // Calculate time to think (Time remaining / 30) + (increment / 2)
    if (board->sideToMove == SIDE_WHITE && wtime != -1) {
        allottedTime = (wtime / 30) + (winc / 2);
    } else if (board->sideToMove == SIDE_BLACK && btime != -1) {
        allottedTime = (btime / 30) + (binc / 2);
    }
    
    // Safety buffer to prevent timeout
    if (allottedTime > 50) allottedTime -= 50;

    // Pass the calculated time to the search function
    SearchResult result = SearchBestMove(board, history, depth, allottedTime);

    char uciMove[6];
    uciMove[0] = 'a' + result.move.fromCol;
    uciMove[1] = '1' + (7 - result.move.fromRow);
    uciMove[2] = 'a' + result.move.toCol;
    uciMove[3] = '1' + (7 - result.move.toRow);
    uciMove[4] = '\0'; 
    
    if (result.move.promotion != EMPTY) {
        if (result.move.promotion == W_QUEEN || result.move.promotion == B_QUEEN) uciMove[4] = 'q';
        else if (result.move.promotion == W_ROOK || result.move.promotion == B_ROOK) uciMove[4] = 'r';
        else if (result.move.promotion == W_BISHOP || result.move.promotion == B_BISHOP) uciMove[4] = 'b';
        else if (result.move.promotion == W_KNIGHT || result.move.promotion == B_KNIGHT) uciMove[4] = 'n';
        uciMove[5] = '\0';
    }

    printf("bestmove %s\n", uciMove);
}

void UCI_Loop(void) {
    char line[INPUT_BUFFER_SIZE];
    
    Board board;
    Board_Init(&board);
    History history;
    History_Init(&history);

    while (fgets(line, INPUT_BUFFER_SIZE, stdin) != NULL) {
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "uci") == 0) {
            printf("id name C Chess Engine\n");
            printf("id author Your Name\n");
            printf("uciok\n");
        }
        else if (strcmp(line, "isready") == 0) {
            printf("readyok\n");
        }
        else if (strcmp(line, "ucinewgame") == 0) {
            TT_Clear();
            History_Init(&history);
        }
        else if (strncmp(line, "position", 8) == 0) {
            ParsePosition(line, &board, &history);
        }
        else if (strncmp(line, "go", 2) == 0) {
            ParseGo(line, &board, &history);
        }
        else if (strcmp(line, "quit") == 0) {
            break;
        }

        fflush(stdout);
    }
}