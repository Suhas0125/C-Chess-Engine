CC = "C:/Users/Suhas Gundla/OneDrive/Desktop/Coding/DevTools/w64DevKit/w64devkit/bin/gcc.exe"

RAYLIB = "C:/Users/Suhas Gundla/OneDrive/Desktop/Coding/DevTools/Raylib"

CFLAGS = -I$(RAYLIB)/include
LDFLAGS = -L$(RAYLIB)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = main.c engine/board.c engine/fen.c engine/move.c ui/renderer.c
OUT = game.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run:
	./$(OUT)

clean:
	del $(OUT)