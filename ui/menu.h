#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include "../engine/board.h"

// One selectable time control preset.
typedef struct {
    const char *label;      // e.g. "5+3 Blitz"
    long long initialMs;    // starting time per side, in milliseconds
    long long incrementMs;  // increment added per move, in milliseconds
} TimeControlOption;

// Result of a fully confirmed menu selection.
typedef struct {
    Side humanSide;
    long long initialMs;
    long long incrementMs;
} GameSetup;

// Draws one frame of the pre-game menu. Must be called between
// BeginDrawing()/EndDrawing(), once per frame, until it returns true.
// Returns true only on the frame the player presses "Start Game" with
// both a side and a time control already selected; *outSetup is then
// filled with the chosen settings.
bool DrawMenuFrame(GameSetup *outSetup);

#endif