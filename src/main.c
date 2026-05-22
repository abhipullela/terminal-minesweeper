#include "minesweeper.h"

void displayGameName(){
    printf("\n\n");
    printf("\033[1m");
    printf(GREEN"███╗   ███╗██╗███╗   ██╗███████╗███████╗██╗    ██╗███████╗███████╗██████╗ ███████╗██████╗ \n");
    printf(     "████╗ ████║██║████╗  ██║██╔════╝██╔════╝██║    ██║██╔════╝██╔════╝██╔══██╗██╔════╝██╔══██╗\n");
    printf(     "██╔████╔██║██║██╔██╗ ██║█████╗  ███████╗██║ █╗ ██║█████╗  █████╗  ██████╔╝█████╗  ██████╔╝\n");
    printf(     "██║╚██╔╝██║██║██║╚██╗██║██╔══╝  ╚════██║██║███╗██║██╔══╝  ██╔══╝  ██╔═══╝ ██╔══╝  ██╔══██╗\n");
    printf(     "██║ ╚═╝ ██║██║██║ ╚████║███████╗███████║╚███╔███╔╝███████╗███████╗██║     ███████╗██║  ██║\n");
    printf(     "╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝ ╚══╝╚══╝ ╚══════╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝\n"RESET);
}


int valid_username(const char *u) 
{
    int len = strlen(u);
    if (len < 3 || len > 20) 
        return 0;
    if (!isalpha(u[0])) 
        return 0;
    for (int i = 1; i < len; i++) 
    {
        if (!isalnum(u[i])) 
            return 0;
    }
    return 1;
}

int main() 
{
    displayGameName();

    Board board;
    Player player;

    printf("\nMenu:\n");
    printf("1: Sign Up/Login\n");
    printf("0: Exit\n");
    printf("Choose: ");
    int choice;
    scanf("%d", &choice);
    switch(choice)
    {
        case 1:
            while (1) 
            {
                printf("Enter username: ");
                scanf("%s", player.name);

                if (!valid_username(player.name)) 
                {
                    printf(RED "Invalid username!\n" RESET);
                    printf("Rules: 3–20 characters, start with a letter, only letters/numbers.\n");
                    continue;
                }
                else
                {
                    if(add_user_to_index(player.name) == -1){
                        printf("Login failure :(\n");
                    }
                    else{
                        printf("Login Successful\n");
                    }
                }

                break;
            }
            break;
        case 0:
            printf("You have opted to exit from the game.\n");
            printf(BLUE"Thanks for using Minesweeper :)\n"RESET);
            exit(0);
        default:
            printf("Invalid choice :(\n");
            exit(0);
    }



    printf("Welcome %s!\n", player.name);
    printf("1: Start new game\n");
    printf("2: Load previous game\n");
    printf("0: Exit\n");
    printf("Choice:\n");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1:
            initializeBoard(&board, 10, 10, 20);
            clear_user_game(player.name, &board, &player);
            break;
        case 2:
            if(load_user_game(player.name, &board, &player) == 0)
            {
                printf("Game loaded successfully.\n");
            }
            else
            {
                printf("Could not load the game.\n");
                return 0;
            }
            break;
        case 0:
            printf("You have opted to exit from the game.\n");
            printf(BLUE"Thanks for using Minesweeper :)\n"RESET);
            exit(0);
        default:
            printf("Invalid choice :(\n");
            exit(0);
    }



    printf(CLEAR);

    int row, col, result;
    char cmd;
    while (1) 
    {
        displayGameName();
        displayBoard(&board);
        printf("\n");
        printf("Enter cell to reveal (cmd row col) \n");
        printf("[cmd =  write r for reveal and f for flag followed by cell coordinates(like - r 0 1); write 'e 0 0' to exit; 's 0 0' to save the game]: \n");
        if (scanf(" %c %d %d", &cmd, &row, &col) != 3) 
        {
            printf("Invalid input.\n");
            printf("Please enter: <cmd> <row> <col>\n");
            continue;
        }

        if (cmd == 'r' || cmd == 'R') {
            result = revealCell(&board, row, col);

            if (result == -1) 
            {
                printf(RED"Boom! Game Over!\n"RESET);
                displayBoard(&board);
                printf(BLUE"Thanks for using Minesweeper :)\n"RESET);
                clear_user_game(player.name, &board, &player);
                return 0;
            } 
            else if (result == 1) 
            {
                printf(GREEN"You cleared all safe cells! You win!\n"RESET);
                displayBoard(&board);
                clear_user_game(player.name, &board, &player);
                return 0;
            }
            else if(result == -2) 
            {
                printf("Invalid cell coordinates.\n");
            }
            else
            {
                printf("Cell already flagged/revealed.\n");
            }
        }
 

        else if(cmd == 'f' || cmd == 'F')
        {
            int t = toggleFlag(&board, row, col);
            if(t == -1)
            {
                printf("Inavild cell entered.");
            }
            else if(t == 0)
            {
                printf("Cannot flag revelaed cell.");
            }

            printf(CLEAR);
            fflush(stdout);
            continue;
        }
        else if(cmd == 'e' || cmd == 'E')
        {
            printf("You have opted to exit from the game.\n");
            printf(BLUE"Thanks for using Minesweeper :)\n"RESET);
            return 0;   
        }
        else if(cmd == 's' || cmd == 'S')
        {
            save_user_game(player.name, &board, &player);
            printf(GREEN"Game saved successfully.\n"RESET);
            printf(BLUE"Thanks for using Minesweeper :)\n"RESET);
            return 0;
        }
        else
        {
            printf("Unknown command '%c'. Use 'r' to reveal, 'f' to toggle flag.\n", cmd);
        }

        if( revealCell(&board, row, col) == 1)
        {
            printf(GREEN"You cleared all safe cells! You win!\n"RESET);
            displayBoard(&board);
            return 0;
        }

        printf(CLEAR);
        fflush(stdout);
    }
    save_user_game(player.name, &board, &player);
    return 0;
}