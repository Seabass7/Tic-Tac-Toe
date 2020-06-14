# Tic-Tac-Toe
Tic-Tac-Toe game played against your computer

## Requirements
SDL2, to install using apt:
```bash
  apt-get install libsdl2-dev
```

## Running the game
Compile using the Makefile:
```bash
  make
```
Then start the game:
```bash
  ./tic-tac-toe
```

## Notes
The computer uses the Minimax algorithm to find suitable moves.
This algorithm finds the moves that makes the computer end up with a draw or a win,
but there are still some room for improvements as some win cases aren't reliably found.
