# Compiling Tic-Tac-Toe
OBJS = main.c
CC = g++

COMPILER_FLAGS = -O2
LINKER_FLAGS = -lSDL2

OBJ_NAME = tic-tac-toe

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)