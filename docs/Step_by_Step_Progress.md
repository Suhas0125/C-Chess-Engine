# C Chess Engine — Full Chronological Development Log

A single consolidated record of every phase, step, file added/modified, and function introduced, in the order they were built.

**Stack:** C, Raylib (GUI), Makefile, Git/GitHub, UCI protocol
**Status at last update:** Phase 0 → 7 complete. Engine is UCI-compliant, tournament-tested against Stockfish via CuteChess, and playable via its own Raylib GUI.

---

## Phase 0 — Project Setup

| Step | Files added/modified | What was added |
|---|---|---|
| Environment setup | `.gitignore`, `Makefile`, `.gitkeep` (per empty folder), `.vscode/c_cpp_properties.json`, `main.c` | Raylib + GCC toolchain configured; IntelliSense set up for `raylib.h`; folder skeleton (`assets/`, `engine/`, `tests/`, `ui/`) created; Makefile automates `make` / `make run`; local Git repo initialized |

**Deliverable:** A clean, compiling C project with a "Hello Raylib!" window.

---

## Phase 1 — Board Representation

| Step | Files added/modified | Structs/Enums/Functions added |
|---|---|---|
| Core data model | `engine/board.h`, `engine/board.c` | Enum `Piece` (EMPTY, W_/B_ pieces), enum `Side`, struct `CastlingRights`, struct `Board` (squares, sideToMove, castling, enPassantSquare, halfmoveClock, fullmoveNumber) |
| Init & debug | `engine/board.c` | `Board_Init()`, `Board_Print()`, `IsWhitePiece()`, `IsBlackPiece()` |
| Build system | `Makefile` | Updated `SRC` to compile `main.c engine/board.c` |

**Deliverable:** Complete internal chess position representation, printable to terminal.

---

## Phase 2 — Rendering System

| Step | Files added/modified | Functions/features added |
|---|---|---|
| 1. Window setup | `main.c` | `InitWindow()`, `SetTargetFPS()` — 800×800 window, 100px tiles |
| 2. Game loop | `main.c` | `BeginDrawing()/EndDrawing()` render loop |
| 3. Board rendering | `main.c` | Nested-loop board draw with alternating tile colors |
| 4. Engine→renderer link | `main.c` | Renderer reads `board.squares[row][col]` directly; engine stays single source of truth |
| 5. Debug piece rendering | `main.c` | ASCII piece rendering (P N B R Q K / lowercase) to validate indexing before textures |
| 6. Texture pipeline validation | `main.c` | Procedural red-square test texture → single rook texture test → pawn row test → full dual-side pawn test, in that order, to isolate bugs incrementally |
| 7. Real asset pipeline | Inkscape (external tool) | SVG chess pieces batch-converted to PNG (`inkscape input.svg --export-type=png ...`) |
| 8. Asset integration | `assets/pieces/*.png` | 12 piece PNGs (`wP.png` … `bK.png`) placed in project |
| 9. Texture loading | `main.c` | 12× `LoadTexture()` calls for each piece |
| 10. `PieceTextures` struct | `main.c` → later `ui/renderer.h` | struct `PieceTextures { Texture2D wP, wN, ... bK; }` |
| 11. `DrawPiece()` | `main.c` → later `ui/renderer.c` | `DrawPiece(Piece p, int row, int col, int tileSize, PieceTextures tex)` |
| 12. Scaling | — | Switched to `DrawTexturePro()` for scaling control |
| 13. Centering fix | — | `x = col*tileSize + (tileSize-texSize)/2` (and y) to center pieces in squares |
| 14. Renderer refactor | `ui/renderer.h`, `ui/renderer.c` (new) | `InitRenderer()`, `UnloadRenderer()`, `DrawGame()` — rendering moved out of `main.c` entirely |
| 15. Resource management | `ui/renderer.c` | Enforced rule: every `LoadTexture` paired with `UnloadTexture` |
| 16. Mouse mapping | `main.c` | `GetMousePosition()` → `col = mouse.x/tileSize`, `row = mouse.y/tileSize` |
| 17. Click debugging | `main.c` | Print selected square + piece on click |
| 18. FEN system | `engine/fen.h`, `engine/fen.c` (new) | `Board_ToFEN()`, `PrintFEN()` |
| 19. Build update | `Makefile` | Compiles `main.c`, `engine/board.c`, `engine/fen.c`, `ui/renderer.c` |

**Deliverable:** Fully playable graphical chessboard, engine-driven, FEN-capable. No move generation yet.

---

## Phase 3 — Pseudo-Legal Move Generation

| Step | Files added/modified | Structs/Functions added |
|---|---|---|
| 1. Move representation | `engine/move.h`, `engine/move.c` (new) | struct `Move` (fromRow/Col, toRow/Col, promotion, flags) |
| 2. Move flags | `engine/move.h` | enum `MoveFlags` (MOVE_NONE, MOVE_CAPTURE, MOVE_DOUBLE_PAWN, MOVE_EN_PASSANT, MOVE_CASTLING, MOVE_PROMOTION) — bit flags |
| 3. Move list | `engine/move.h/.c` | struct `MoveList` (moves[256], count); `MoveList_Add()` |
| 4. Movegen entry point | `engine/movegen.h`, `engine/movegen.c` (new) | `GenerateMoves(board, &list)` |
| 5. Pawn single push | `engine/movegen.c` | Forward one-square pawn move logic |
| 6. Pawn double push | `engine/movegen.c` | Two-square initial advance, tagged `MOVE_DOUBLE_PAWN` |
| 7. Pawn captures | `engine/movegen.c` | Diagonal captures, tagged `MOVE_CAPTURE` |
| 8. Pawn promotion | `engine/movegen.c` | Promotion moves storing `move.promotion` + `MOVE_PROMOTION` |
| 9. En passant generation | `engine/movegen.c` | En passant target-square capture logic, tagged `MOVE_EN_PASSANT` |
| 10. Knight moves | `engine/directions.h` (new), `engine/movegen.c` | `knightMoves[8][2]` lookup table + generator |
| 11. Bishop moves | `engine/directions.h`, `engine/movegen.c` | `bishopDirections[4][2]` + sliding-move generator |
| 12. Rook moves | `engine/directions.h`, `engine/movegen.c` | `rookDirections[4][2]` + sliding-move generator |
| 13. Queen moves | `engine/directions.h`, `engine/movegen.c` | `queenDirections` (rook+bishop combined) |
| 14. King moves | `engine/directions.h`, `engine/movegen.c` | `kingDirections[8][2]` — 8 adjacent squares |
| 15. Castling generation | `engine/movegen.c` | Pseudo-legal castling (rights + empty intermediate squares), tagged `MOVE_CASTLING` |
| 16. Central dispatch | `engine/movegen.c` | All piece generators unified under `GenerateMoves()` |
| 17. Side-to-move filtering | `engine/movegen.c` | Skip pieces not belonging to `board->sideToMove` |
| 18. Move execution | `engine/makemove.h`, `engine/makemove.c` (new) | `MakeMove(Board*, const Move*)` — basic piece relocation |
| 19. Game-state updates in MakeMove | `engine/makemove.c` | Side switching, halfmove clock, fullmove number, en passant target update |
| 20. Verification | — | Manual per-piece test positions (no new files) |
| 21. En passant execution | `engine/makemove.c` | Correct captured-pawn removal for `MOVE_EN_PASSANT` |
| 22. Promotion execution | `engine/makemove.c` | Replace pawn with `move->promotion` piece |
| 23. Castling execution | `engine/makemove.c` | Rook repositioning for all 4 castling cases |
| 24. Castling rights updates | `engine/makemove.c` | Rights revoked on king/rook move or rook capture |
| 25. Complete `MakeMove()` | `engine/makemove.c` | All of the above unified into one function |
| 26–29. Full verification & architecture | — | Isolated tests for every move type; finalized `Board → GenerateMoves() → MoveList → MakeMove() → Updated Board` pipeline |

**Deliverable:** Complete pseudo-legal move generator + full move execution (no king-safety checks yet).

---

## Phase 4 — Legal Move Generation

| Step | Files added/modified | Structs/Functions added |
|---|---|---|
| 1. Board copying | `engine/board.c/.h` | `Board_Copy(Board *dest, const Board *src)` |
| 2.1 History module | `engine/history.h`, `engine/history.c` (new) | struct `History`; `History_Init()`, `History_Push()`, `History_Pop()` |
| 2.2 Save history on move | `engine/makemove.c` | `History_Push()` called at start of `MakeMove()`; `History` param added |
| 2.3 Undo | `engine/history.c` | `UndoMove(Board*, History*)` |
| 2.4 Improved debug print | `engine/board.c` | `Board_Print()` extended with side to move, castling rights, en passant square, clocks |
| 4B.1 Attack module | `engine/attack.h`, `engine/attack.c` (new) | Module scaffold for attack detection |
| 4B.2–4B.7 Attack detection per piece | `engine/attack.c` | Pawn, knight, bishop (ray), rook (ray), queen (8-dir ray), king attack detection — all feeding `IsSquareAttacked()` |
| 4B.8 Check detection | `engine/attack.c` | `IsKingInCheck()` |
| 4C.1 Legal move module | `engine/legalmove.h`, `engine/legalmove.c` (new) | Module scaffold |
| 4C.2 Legal filtering | `engine/legalmove.c` | `GenerateLegalMoves()` — make move, test check, undo, keep if king safe |
| 4C.3 Move list printer | `engine/move.c` | `MoveList_Print()` |
| 4C.4–4C.5 Verification | — | Pinned-piece test, in-check test positions |
| 4D.1 Checkmate detection | `engine/gamestate.h`, `engine/gamestate.c` (new) | `IsCheckmate()` |
| 4D.2 Verification | — | g6/g7/h8 test position |
| 4D.3 Stalemate detection | `engine/gamestate.c` | `IsStalemate()` |
| 4E.1–4E.2 Castling legality | `engine/movegen.c` | Forbid castling while in check, through check, or into check |
| 4F.1 Fifty-move rule | `engine/gamestate.c` | `IsDrawByFiftyMoveRule()` |
| 4G.1 Position comparison | `engine/history.c` | `PositionEquals()` (pieces, side, castling, en passant — excludes clocks) |
| 4G.2 Threefold repetition | `engine/gamestate.c` | `IsDrawByThreefoldRepetition()` |

**Deliverable:** Fully legal move generation — self-checks, pins, castling rules, checkmate/stalemate/fifty-move/threefold all handled. Insufficient-material draw intentionally deferred.

---

## Phase 5 — Search & Evaluation

| Step | Files added/modified | Functions added |
|---|---|---|
| 1. Evaluation framework | `engine/evaluate.h`, `engine/evaluate.c` (new) | `GetPieceValue()` (private), `EvaluatePosition()` — material-only |
| 2.1 PST module | `engine/pst.h`, `engine/pst.c` (new) | Knight Piece-Square Table |
| 2.2 PST integration | `engine/evaluate.c` | `GetPieceSquareValue()` (private) — mirrors row index for Black |
| 3.1 Search module | `engine/search.h`, `engine/search.c` (new) | `Negamax()`, `SearchBestMove()` scaffolds |
| 3.2 Base case | `engine/search.c` | `Negamax()` returns `EvaluatePosition()` at depth 0 |
| 3.3 One-ply search | `engine/search.c` | `Negamax()` iterates legal moves, make/evaluate/undo |
| 3.4 Recursive Negamax | `engine/search.c` | `Negamax()` recurses to arbitrary depth, negating score per ply |
| 3.5 Terminal handling | `engine/search.c` | Checkmate → large negative score; stalemate → 0 |
| 3.6 Root search | `engine/search.c` | `SearchBestMove()` selects highest-scoring root move |
| 4.1–4.3 Alpha-Beta pruning | `engine/search.c` | `Negamax()`/`SearchBestMove()` extended with alpha/beta window; cutoff when `alpha >= beta` |
| 5.1 Node counter | `engine/search.c/.h` | Node count tracked per search |
| 5.2 Best evaluation reporting | `engine/search.c/.h` | Best move's score recorded |
| 5.3 `SearchResult` | `engine/search.h/.c` | struct `SearchResult` (move, eval, nodes) — `SearchBestMove()` now returns it |
| 5.4 Remove global state | `engine/search.c` | Node counter passed through recursion instead of global |
| 6.1 Engine auto-play | `engine/search.c` | `MakeEngineMove()` — search + execute in one call |
| 6.2 Engine response in UI | `main.c` | Engine replies automatically after human move |
| 6.3 No-legal-move safety | `engine/search.c` | Guard against executing invalid move at checkmate/stalemate |
| 6.4 Terminal evaluation constants | `engine/evaluate.h`, `engine/search.c` | `CHECKMATE_SCORE`, `DRAW_SCORE` used at terminal nodes |
| 7.1–7.4 Human move input | `main.c` | Selected-square tracking; restrict selection to human's pieces; generate legal moves for selected piece; execute move on destination click → triggers `MakeEngineMove()` |

**Deliverable:** Engine evaluates positions, searches multiple plies with alpha-beta, and plays complete Human vs Engine games via the GUI.

---

## Phase 5.5 — Verification & Debugging Infrastructure

| Step | Files added/modified | Functions added |
|---|---|---|
| 1. FEN import | `engine/fen.h/.c` | `Board_FromFEN()`, `CharToPiece()` (private), `algebraicToSquare()` (private) |
| 2. FEN export utility | `engine/fen.c` | `PrintFEN()` |
| 3. FEN round-trip test | — | `FEN → Board → FEN` verified via `strcmp()` on multiple positions |
| 4. Debug utilities | `engine/debug.h`, `engine/debug.c` (new) | `Debug_PrintBoardState()`, `Debug_PrintLegalMoves()`, `Debug_PrintGameState()` |
| 5. Perft | `engine/perft.h`, `engine/perft.c` (new) | `Perft()`, `PerftDivide()` |
| 6. Automated Perft suite | `engine/tests.h`, `engine/tests.c` (new) | `RunAutomatedPerftSuite()` — verifies Start, Kiwipete, Positions 3–5 |

**Deliverable:** Mathematically verified legal move generator — 6.8M+ leaf nodes computed, 5/5 Perft positions passed. Move generation certified bug-free.

---

## Phase 6 — Search Optimization & Heuristics

| Step | Files added/modified | Functions/data added |
|---|---|---|
| 6A. Quiescence Search | `engine/search.c` | `QuiescenceSearch()` (captures-only at depth 0); `Negamax()` calls it instead of static eval; `INFINITY_SCORE` fixes root alpha/beta overflow |
| 6B. MVV-LVA move ordering | `engine/evaluate.h/.c`, `engine/search.c` | `GetPieceValue()` exposed globally; `SortMoves()` — fixed the freeze caused by unordered Q-Search explosion |
| 6C. Iterative deepening & PV | `engine/search.h/.c` | Triangular `pvArray`/`pvLength`; `Negamax()` gains `ply` param; `SearchBestMove()` loops depth 1→max, feeding prior PV forward; UCI-style `info depth... pv...` output |
| 6D. Zobrist hashing | `engine/zobrist.h`, `engine/zobrist.c` (new), `engine/board.h`, `engine/makemove.c` | `Zobrist_Init()`, `Zobrist_GenerateKey()`; `Board.hashKey` field; `MakeMove()` updates hash incrementally |
| 6E. Transposition tables | `engine/tt.h`, `engine/tt.c` (new), `engine/search.c` | `TT_Init()`, `TT_Store()`, `TT_Probe()` (EXACT/ALPHA/BETA bounds); `Negamax()` probes TT for early cutoffs |
| 6F. Killer & history heuristics | `engine/search.c` | `killerMoves[MAX_PLY][2]`, `historyTable[side][from][to]`; `MovesEqual()`, `IsKillerMove()`; `SortMoves()` overhauled to: PV/TT → MVV-LVA captures → promotions → killers → history |

**Result:** Node counts dropped substantially at every depth (e.g. ~30% at depth 5 from TT; ~20% at depth 2 from killer/history); engine reaches depth 6+ searching only ~67k nodes.

---

## Phase 7 — UCI & Engine Polish

| Step | Files added/modified | Functions/features added |
|---|---|---|
| 7A. UCI communication loop | `engine/uci.h`, `engine/uci.c` (new), `main.c` | `UCI_Loop()`; handshake commands `uci`/`isready`/`quit`; `main.c` branches into headless UCI mode vs Raylib GUI mode based on argv |
| 7B. Position & Go commands | `engine/uci.c` | `ParsePosition()` (startpos/FEN + move history), `ParseAndMakeMove()` (UCI string → `Move`), `ParseGo()` (depth extraction, triggers `SearchBestMove()`, emits `bestmove`) |
| 7C. Time management | `engine/time_utils.h`, `engine/time_utils.c` (new), `engine/search.h/.c`, `engine/uci.c` | `GetTimeMs()` (cross-platform); `CheckTime()` hook probed every 2048 nodes; `searchStopped` flag in `SearchBestMove()`; `ParseGo()` parses `wtime/btime/winc/binc` |
| 7D. UI polish | `ui/renderer.h/.c`, `main.c` | Drag-and-drop + click-to-move; capture-ring/dot move indicators; last-move highlighting; right-click square/arrow annotation; fixed `TT_Init()/TT_Free()` lifecycle crash |
| 7E. Engine validation | External: CuteChess + Stockfish | 1+1 match vs Stockfish; survived 37 moves before hitting the **Root TT Cutoff bug** (`a8a8` illegal move from an empty-initialized `Move{0}` when the TT short-circuited the root node) |
| 7E fix | `engine/search.c` | `Negamax()` changed to forbid TT cutoffs when `ply == 0`, guaranteeing the root always completes its move-generation loop |
| 7E re-validation | — | Multiple full games vs Stockfish completed with no crashes, no illegal moves, correct terminal-score detection |

**Deliverable:** A complete, standalone, UCI-compliant chess engine — playable in Arena/CuteChess/Banksia or via its own Raylib GUI, tournament-stress-tested against Stockfish.

---

## Overall File Map (final state)

```
C_Chess_Engine/
├── assets/
│   ├── fonts/
│   └── pieces/            (12 piece PNGs, converted from SVG)
├── engine/
│   ├── attack.c/.h        Attack & check detection
│   ├── board.c/.h         Board state & representation
│   ├── debug.c/.h         Centralized debugging tools
│   ├── directions.h       Pre-calculated movement offsets
│   ├── evaluate.c/.h      Static position evaluation
│   ├── fen.c/.h           FEN import/export
│   ├── gamestate.c/.h     Mate & draw detection
│   ├── history.c/.h       Game state history stack
│   ├── legalmove.c/.h     Legal move filtering
│   ├── makemove.c/.h      Move execution
│   ├── move.c/.h          Move definitions & lists
│   ├── movegen.c/.h       Pseudo-legal move generation
│   ├── perft.c/.h         Recursive move tree testing
│   ├── pst.c/.h           Piece-Square Tables
│   ├── search.c/.h        Negamax, Alpha-Beta, PV, TT probing
│   ├── tests.c/.h         Automated Perft verification
│   ├── time_utils.c/.h    Cross-platform clock
│   ├── tt.c/.h            Transposition table
│   ├── uci.c/.h           UCI protocol
│   └── zobrist.c/.h       Zobrist hashing
├── ui/
│   └── renderer.c/.h      Raylib graphics & UI
├── main.c
└── Makefile
```