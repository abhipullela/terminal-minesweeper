#include "minesweeper.h"

void initializeBoard(Board *b, int rows, int cols, int mines)
{
    srand(time(NULL)); // to randomize seed for rand func
    b->rows = rows; // defines row size of the board
    b->cols = cols; // defines col size of the board
    b->totalMines = mines; // store the total number of mines in the board

    // this loop initializes the values for each cell
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++) 
        {
            b->grid[i][j].isMine = 0;
            b->grid[i][j].isRevealed = 0;
            b->grid[i][j].adjacentMines = 0;
            b->grid[i][j].isFlagged = 0;
        }
    }
    // Randomly place mines
    for (int m = 0; m < mines;) 
    {
        int r = rand() % rows;
        int c = rand() % cols;
        // we place a mine in a randomized cell only if it is not already a mine
        if (!b->grid[r][c].isMine) 
        {
            b->grid[r][c].isMine = 1;
            m++;
        }
    }

    // Calculate adjacent mines
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (!b->grid[i][j].isMine) // calculating the mines for a mined cell is pointless so we don't consider it
            {
                // the following loop helps us iterate thorugh the adjacent grid of our selected cell
                for (int box_row = -1; box_row <= 1; box_row++)
                {
                    for (int box_col = -1; box_col <= 1; box_col++) 
                    {
                        int adj_row = i + box_row, adj_col = j + box_col;
                        // we will count the mines only if the selected cell in adjacent grid is inside the board and it is a mine
                        if (adj_row >= 0 && adj_row < rows && adj_col >= 0 && adj_col < cols && b->grid[adj_row][adj_col].isMine)
                            b->grid[i][j].adjacentMines++;
                    }
                }
            }
        }
    }
}

void displayBoard(Board *b)
{
    printf("\n   ");
    for(int i = 0; i < b->cols; i++) printf(WHITE"%d   "RESET, i); // print col nos.

    int c = 0; // to store the count of mines in revealed cells encountered, This helps us print all the mines cell if even one of the mined cell is selected by user
    for(int i = 0; i < b->rows; i++){
        for(int j = 0; j < b->cols; j++){
            if(b->grid[i][j].isRevealed && b->grid[i][j].isMine)
            {
                c=1;
                break;
            }
        }
    }


    for(int i = 0 ; i < b->rows; i++)
    {
        printf("\n  ");
        for(int j = 0; j < b->cols; j++) printf(WHITE"--- "RESET);
        printf("\n");
        printf(WHITE"%d"RESET,i); // print row no.
        for(int j = 0; j < b->cols; j++)
        {
            // we print only if the cell is revealed already or if a mine was earlier encountered in a revealed cell and the current cell is a mineed
            if(b->grid[i][j].isRevealed || (b->grid[i][j].isMine)*c)
            {
                if(b->grid[i][j].isMine)
                {
                    printf(WHITE"|"RESET);
                    printf(RED" * "RED);
                }
                else
                {
                    printf(WHITE"|"RESET);
                    int n = b->grid[i][j].adjacentMines;
                    if(n == 0)
                        printf(WHITE " . " RESET);
                    else if(n == 1)
                        printf(BLUE " 1 " RESET);
                    else if(n == 2)
                        printf(GREEN " 2 " RESET);
                    else if(n == 3)
                        printf(CYAN " 3 " RESET);
                    else
                        printf(MAGENTA " %d " RESET, n);
                    // printf("| %d ", b->grid[i][j].adjacentMines);
                }
            }
            else
            {
                printf(WHITE"|"RESET);
                if (b->grid[i][j].isFlagged) {
                    printf(YELLOW " F " RESET);
                } else {
                    printf(WHITE " # " RESET);
                } 
            }
        }
        printf(WHITE"|"RESET);
    }
    printf("\n  ");
    for(int i = 0; i < b->cols; i++)
    {
        printf(WHITE"--- "RESET);
    }
    printf("\n");

}


// this function tales the input of the selected cell of user and returns 1 if the board is successfully solved (player won) , -1 if the mine is encountered (player lost) and 0 (continue) otherwise
int revealCell(Board *b, int r, int c) 
{
    if (r < 0 || r >= b->rows || c < 0 || c >= b->cols) return -2;

    if (b->grid[r][c].isFlagged || b->grid[r][c].isRevealed) return 0;

    

    if (b->grid[r][c].isMine){
        b->grid[r][c].isRevealed = 1;
        return -1;
    }

    revealEmptyCells(b, r, c);
    // Win condition check
    int unrevealed = 0;
    for (int i = 0; i < b->rows; i++){
        for (int j = 0; j < b->cols; j++){
            if ((!b->grid[i][j].isRevealed) && (!b->grid[i][j].isMine)) // we count the cells which are not revealed and are not a mine
                unrevealed++;
        }
    }
    return (unrevealed == 0 ? 1 : 0);
}

// this recursive function helps us reveal all the cells which have a mine adjacent to them, when a cell with no adjacent cells is selected
void revealEmptyCells(Board *b, int r, int c) 
{
    if (r < 0 || r >= b->rows || c < 0 || c >= b->cols)
        return;
    if (b->grid[r][c].isRevealed || b->grid[r][c].isMine){
        return;
    }

    b->grid[r][c].isRevealed = 1;

    if (b->grid[r][c].adjacentMines == 0) 
    {
        for (int box_row = -1; box_row <= 1; box_row++) 
        {
            for (int box_col = -1; box_col <= 1; box_col++) 
            {
                if (box_row != 0 || box_col != 0)
                    revealEmptyCells(b, r + box_row, c + box_col);
            }
        }
    }
}

int toggleFlag(Board *b, int r, int c) 
{
    if (r < 0 || r >= b->rows || c < 0 || c >= b->cols)
        return -1; // invalid coords

    // cannot flag a revealed cell
    if (b->grid[r][c].isRevealed)
        return 0;

    b->grid[r][c].isFlagged = !b->grid[r][c].isFlagged;
    return 1;
}

