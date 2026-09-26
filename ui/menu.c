#include "menu.h"
#include "raylib.h"

// Predefined time control presets shown as buttons, in order.
static const TimeControlOption timeControls[] = {
    { "1+0 Bullet",     1 * 60 * 1000,      0 },
    { "3+2 Blitz",       3 * 60 * 1000,   2000 },
    { "5+0 Blitz",       5 * 60 * 1000,      0 },
    { "10+5 Rapid",     10 * 60 * 1000,   5000 },
    { "15+10 Rapid",    15 * 60 * 1000,  10000 },
    { "30+0 Classical", 30 * 60 * 1000,      0 },
};
#define TIME_CONTROL_COUNT (int)(sizeof(timeControls) / sizeof(timeControls[0]))

// Persisted selection state across frames (menu is only ever shown once
// per app launch, so file-scope statics are enough here).
static int selectedSide = -1;         // -1 = none, 0 = White, 1 = Black
static int selectedTimeControl = -1;  // -1 = none

// Draws one clickable button and reports whether it was clicked this frame.
static bool DrawButton(Rectangle rect, const char *text, bool selected) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);

    Color fill = selected ? GOLD : (hovered ? LIGHTGRAY : RAYWHITE);
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);

    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text,
             (int)(rect.x + (rect.width - textWidth) / 2.0f),
             (int)(rect.y + (rect.height - fontSize) / 2.0f),
             fontSize, BLACK);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool DrawMenuFrame(GameSetup *outSetup) {
    ClearBackground(RAYWHITE);

    DrawText("C Chess Engine", 230, 30, 32, BLACK);

    // --- Side selection ---
    DrawText("Choose your side", 60, 100, 22, BLACK);
    Rectangle whiteBtn = { 60, 135, 160, 50 };
    Rectangle blackBtn = { 240, 135, 160, 50 };

    if (DrawButton(whiteBtn, "White", selectedSide == 0)) selectedSide = 0;
    if (DrawButton(blackBtn, "Black", selectedSide == 1)) selectedSide = 1;

    // --- Time control selection ---
    DrawText("Choose time control", 60, 210, 22, BLACK);
    for (int i = 0; i < TIME_CONTROL_COUNT; i++) {
        Rectangle btn = { 60, 245 + i * 55, 300, 45 };
        if (DrawButton(btn, timeControls[i].label, selectedTimeControl == i)) {
            selectedTimeControl = i;
        }
    }

    // --- Start button ---
    bool canStart = (selectedSide != -1 && selectedTimeControl != -1);
    Rectangle startBtn = { 60, 245 + TIME_CONTROL_COUNT * 55 + 20, 300, 55 };

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, startBtn);
    Color fill = !canStart ? LIGHTGRAY : (hovered ? (Color){110, 220, 110, 255} : (Color){150, 230, 150, 255});
    DrawRectangleRec(startBtn, fill);
    DrawRectangleLinesEx(startBtn, 2, DARKGRAY);

    const char *label = "Start Game";
    int fontSize = 22;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label,
             (int)(startBtn.x + (startBtn.width - textWidth) / 2.0f),
             (int)(startBtn.y + (startBtn.height - fontSize) / 2.0f),
             fontSize, BLACK);

    if (!canStart) {
        DrawText("Pick a side and a time control to continue",
                  60, (int)(startBtn.y + startBtn.height + 15), 16, GRAY);
        return false;
    }

    if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        outSetup->humanSide = (selectedSide == 0) ? SIDE_WHITE : SIDE_BLACK;
        outSetup->initialMs = timeControls[selectedTimeControl].initialMs;
        outSetup->incrementMs = timeControls[selectedTimeControl].incrementMs;
        return true;
    }

    return false;
}