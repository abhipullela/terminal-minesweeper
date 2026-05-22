## Minesweeper — Terminal Game (C Project) ##


# Overview:

This is a command-line version of the classic Minesweeper game written in the C programming language.
The game provides an interactive experience where the player must uncover safe cells on a minefield while avoiding hidden mines.


# The project includes the following major features:

1. Fully playable Minesweeper game in the terminal
 
2. Save and load game progress using text files

3. Automatic reveal of surrounding empty cells

4. Flagging system to mark suspected mines

5. Username-based save management

6. Game win and game loss detection

7. Works on Linux, macOS, and Windows terminals with ANSI support


# This project demonstrates knowledge of:

1. Structures (struct)

2. Dynamic memory allocation

3. File handling (I/O operations)

4. Modular programming (multiple source files)

5. Basic error handling and input validation


# Project Structure:

    MINESWEEPER/
    ├── src/
        ├── main.c
        ├── game.c
        ├── storage.c
    ├── include/
        ├── minesweeper.h
    ├── saves/
        ├── (saved game files appear here)
        ├── index.txt
    ├── Makefile
    ├── Ctrl+Win.txt
    └── README.md


# Compilation Instructions:

This project uses a Makefile for easy compilation.

To build the game, Run:
make

This will generate an executable named:
minesweeper

To clean build files, run:
make clean

=> Running the Game:
Once compiled, run the following command:
./minesweeper


# Gameplay Instructions:

<!-- Command				     Format			       Description -->
    Reveal a cell			    r <row> <col>	    Attempts to uncover a cell
    Flag a cell				    f <row> <col>	    Marks / unmarks a cell as a suspected mine
    Save & Exit				    s 0 0			    Saves your progress and exits the game
    Load previous save		    Automatic		    Loads based on username

Example:

    r 2 3
    f 4 1
    s 0 0

- Winning the Game

    You win when all non-mine cells are revealed successfully.
    The program will display a celebration message when cleared.

- Losing the Game

    Revealing a mine results in an immediate game over and mine positions are shown.

- Save File Format

    Save files are plain text and stored in the saves/ folder.

    They contain:
        Board size
        Mine locations and reveal state
        Player username
        This allows resuming exactly from where you left off.


# Credits

Developed as part of an academic assignment to practice:

    System-level programming in C

    Terminal-based UI design

    Game state management through files


# Authored by:

Y B Siddharth

Abhinav P


# Code Structure

| File Name                 | Type        | Functionality                              |
| ------------------------- | ----------- | ------------------------------------------ |
| `src/main.c`              | Source File | Game flow, user input/output               |
| `src/game.c`              | Source File | Core gameplay logic                        |
| `src/storage.c`           | Source File | File I/O and persistence                   |
| `include/minesweeper.h`   | Header File | Structs, constants, prototypes             |
| `Makefile`                | Build File  | Compilation, execution, cleanup automation |


src/main.c - Contains the main() function, handles user interaction, username input, main menu, and the core game loop. It calls the game logic (game.c) and save/load functions (storage.c).

src/game.c - Implements the core Minesweeper logic: board initialization, random mine placement, counting adjacent mines, revealing cells (including recursive empty-area reveal), toggling flags, win/lose detection, and board display.

src/storage.c - Handles file-based persistence: creating the saves/ directory, sanitizing usernames, building save file paths, saving the current board and player to a text file, loading an existing game for a user, and maintaining an index of saved users.

include/minesweeper.h - Header file with shared definitions: color macros, constants (e.g., max rows/columns), the Cell, Board, and Player structs, and function prototypes for all functions implemented in game.c, storage.c, and used in main.c.

Makefile - Build script that compiles the source files in src/ into the minesweeper executable and provides a clean target to remove object files and the binary. 