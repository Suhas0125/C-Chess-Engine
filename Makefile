CC = "C:/Users/Suhas Gundla/OneDrive/Desktop/Coding/DevTools/w64DevKit/w64devkit/bin/gcc.exe"

RAYLIB = "C:/Users/Suhas Gundla/OneDrive/Desktop/Coding/DevTools/Raylib"

CFLAGS = -I$(RAYLIB)/include
LDFLAGS = -L$(RAYLIB)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = main.c engine/board.c engine/fen.c engine/move.c engine/movegen.c engine/makemove.c engine/history.c engine/attack.c engine/legalmove.c engine/gamestate.c engine/evaluate.c engine/pst.c engine/search.c engine/debug.c engine/perft.c engine/tests.c engine/zobrist.c engine/tt.c engine/uci.c engine/time_utils.c ui/renderer.c ui/menu.c
OUT = game.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run:
	./$(OUT)

clean:
	del $(OUT)