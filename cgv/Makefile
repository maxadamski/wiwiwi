SRC = src
OUT = out
OBJ = out
EXE = out/game

SOURCES = $(wildcard $(SRC)/*.cc)
OBJECTS = $(patsubst $(SRC)/%.cc, $(OBJ)/%.o, $(SOURCES))

LIBS  = -lGL -lGLEW -lglfw -lGLU -lassimp -lIL
FLAGS = -Wall --std=c++17
CC    = g++ $(FLAGS)

.PHONY: clean

debug: FLAGS += -g -O0
debug: all

optim: FLAGS += -O3
optim: all

$(OBJ)/%.o: $(SRC)/%.cc
	@mkdir -p out
	$(CC) -I$(SRC) -c $< -o $@

all: $(OBJECTS)
	$(CC) $(LIBS) $^ -o $(EXE)

run: all
	$(EXE)

clean:
	rm -rf out
