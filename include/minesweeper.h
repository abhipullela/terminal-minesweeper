#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>


// color macros
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define CLEAR "\033[2J\033[H"

// these store the maxinmum possible board size
#define ROW_MAX 20
#define COL_MAX 20

#define NAME_BUF 64

typedef struct
{
    int isMine; // stores whether the cell is mined
    int isRevealed; // stores whether the cell is revealed
    int adjacentMines; // stores the count of adjacent mines
    int isFlagged;      // NEW: track flagged cells
} Cell;

typedef struct
{
    Cell grid[ROW_MAX][COL_MAX];
    int rows; // the row size of board
    int cols; // the col size of board
    int totalMines; // the count of mines on the bpard
} Board;

typedef struct
{
    char name[NAME_BUF];
} Player;

void initializeBoard(Board *b, int rows, int cols, int mines);
void displayBoard(Board *b);
int revealCell(Board *b, int r, int c);
void revealEmptyCells(Board *b, int r, int c);
int toggleFlag(Board *b, int r, int c);
int save_user_game(const char *username, Board *b, Player *p);
int load_user_game(const char *username, Board *b, Player *p);
int add_user_to_index(const char *username);
int list_saved_users(void);
int clear_user_game(const char *username_raw, Board *b, Player *p);
#endif