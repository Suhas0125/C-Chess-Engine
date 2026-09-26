# Phase 8 – Pre-Game Menu, Time Controls & Post-Game Flow

## Objective

Add a pre-game screen (side + time control), real per-side clocks with
flag-fall, and a proper end-of-game screen that lets the player start
another match or quit — no core engine logic touched.

---

## Step 1 — Pre-Game Menu

**Files added:** `ui/menu.h`, `ui/menu.c`

**Added:** struct `GameSetup` (humanSide, initialMs, incrementMs); struct
`TimeControlOption`; function `DrawMenuFrame(GameSetup*)` — draws side
buttons (White/Black), 6 time-control buttons (1+0 → 30+0), and a Start
button; returns `true` once both are picked and Start is clicked.

**Result:** A standalone pre-game screen, separate from the game loop.

---

## Step 2 — Board Flip, Clocks & Game-Over Screen (Renderer)

**Files modified:** `ui/renderer.h`, `ui/renderer.c`

**Changed:** `DrawGame()` gained a `bool flipped` parameter. All board
coordinates passed in stay true board coordinates everywhere else in the
program (row 0 = rank 8) — only `renderer.c` converts to screen space,
via new private helpers `ToScreenRow()`/`ToScreenCol()`.

**Added:**
- `DrawClocks(whiteTimeMs, blackTimeMs, sideToMove, flipped)` — places
  each clock on the screen edge matching that color's home rank, so it's
  correct whether or not the board is flipped.
- `GameOverAction DrawGameOver(message)` — banner plus **Play Again** /
  **Quit** buttons; returns which one (if any) was clicked this frame.

**Result:** Board orientation follows the chosen side, clocks render
correctly either way, and the game-over screen is now interactive instead
of a dead end.

---

## Step 3 — Menu Flow, Clocks & Post-Game Handling (`main.c`)

**Files modified:** `main.c`

**Added:** enum `AppState` (`APP_MENU`, `APP_PLAYING`, `APP_GAMEOVER`);
functions `ComputeTimeBudget()` (`remaining/30 + increment/2`, same
formula as `ParseGo()`'s UCI time management) and `PlayEngineMove()`
(runs a timed search, executes it, settles that side's clock).

**Behavior:**
- Board/History only initialize after the menu confirms.
- If human picks Black, engine auto-plays White's first move.
- Mouse clicks convert to true board coordinates via the same flip used
  by the renderer; left-click input is gated behind `isHumanTurn`.
- Clocks tick down every frame via `GetFrameTime()`; hitting `0` ends the
  game on time. Checkmate/stalemate are also checked explicitly after
  every move.
- **New:** `APP_GAMEOVER` no longer just sits there — `DrawGameOver()`'s
  return value is checked each frame: `GAMEOVER_PLAY_AGAIN` sends the
  loop back to `APP_MENU` (fresh menu, same window); `GAMEOVER_QUIT` sets
  a `shouldQuit` flag that the main `while` loop condition now checks,
  which reaches the same cleanup path (`UnloadRenderer` → `CloseWindow`
  → `TT_Free`) as closing the window normally.

**Result:** A full menu → play → game-over → (menu again or clean exit)
loop, with no need to relaunch the `.exe` between matches.

---

## Step 4 — Makefile

Add `ui/menu.c` to your build sources alongside `ui/renderer.c` (skip
this if your `Makefile` already uses a wildcard like
`$(wildcard ui/*.c)`).

---

## Testing Checklist

1. Build and confirm the menu appears on launch.
2. Play a full game to checkmate (or let a clock hit `00:00`) — confirm
   the correct message shows with two buttons instead of a frozen screen.
3. Click **Play Again** — confirm it returns to the same menu, and a new
   match can be set up and played (try the opposite side this time).
4. Click **Quit** — confirm the window closes cleanly, same as the `X`
   button or `WindowShouldClose()`.