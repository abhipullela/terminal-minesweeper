/* storage_basic.c
 *
 * Simple per-user text saves using only standard C library calls.
 * - saves/<username>.txt holds the last saved game for that username
 * - saves/index.txt contains a list of saved usernames (one per line)
 *
 * Add these prototypes to minesweeper.h:
 *   int save_user_game(const char *username, Board *b, Player *p, long session_elapsed);
 *   int load_user_game(const char *username, Board *b, Player *p, long *out_session_elapsed);
 *   int add_user_to_index(const char *username);
 *   int list_saved_users(void);
 */

#include "minesweeper.h"
#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(path) _mkdir(path)
#else
  #include <sys/stat.h>
  #define MKDIR(path) mkdir(path, 0755)
#endif

/* Constants */
#define SAVES_DIR "saves"
#define INDEX_FILE "saves/index.txt"
#define PATH_BUF 512
#define NAME_BUF 64

/* sanitize_username
 * - copies username into out buffer replacing invalid chars with '_'
 * - allows only A-Z a-z 0-9 '_' '-'
 * - returns 0 on success, -1 on failure
 */
static int sanitize_username(const char *in, char *out, size_t out_sz) 
{
    if (!in || !out || out_sz == 0) 
        return -1;
    size_t i = 0, k = 0;
    while (in[i] != '\0' && k + 1 < out_sz) 
    {
        unsigned char c = (unsigned char)in[i++];
        if (isalnum(c) || c == '_' || c == '-') 
            out[k++] = c;
        else 
            out[k++] = '_';
    }
    out[k] = '\0';
    return 0;
}

/* ensure_saves_dir
 * - creates the "saves" directory if it does not exist
 * - ignores errors (simple approach)
 */
static void ensure_saves_dir() 
{
    /* try to create the directory; if it exists, MKDIR typically fails but that's fine */
    MKDIR(SAVES_DIR);
}

/* build_user_path
 * - fills out buffer with "saves/<user>.txt"
 */
static void build_user_path(const char *user, char *buf, size_t buf_sz) 
{
    snprintf(buf, buf_sz, "%s/%s.txt", SAVES_DIR, user);
}

/* add_user_to_index
 * - ensures username is listed in saves/index.txt (append if missing)
 * - returns 0 on success, -1 on failure
 *
 * This keeps a small registry of users so you can list saved users.
 */
int add_user_to_index(const char *username_raw) 
{
    char user[NAME_BUF];
    if (sanitize_username(username_raw, user, sizeof(user)) != 0) 
        return -1;
    ensure_saves_dir();

    /* check if already present */
    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp) 
    {
        char line[NAME_BUF];
        while (fgets(line, sizeof(line), fp)) 
        {
            /* strip newline */
            size_t L = strlen(line);
            if (L && (line[L-1] == '\n' || line[L-1] == '\r')) 
                line[--L] = '\0';
            if (strcmp(line, user) == 0)  /* already listed */
            {
                fclose(fp); 
                return 0; 
            } 
        }
        fclose(fp);
    }

    /* append username */
    fp = fopen(INDEX_FILE, "a");
    if (!fp) 
        return -1;
    fprintf(fp, "%s\n", user);
    fclose(fp);
    return 0;
}

/* list_saved_users
 * - prints saved usernames (reads saves/index.txt)
 * - returns number of users printed
 */
int list_saved_users(void) 
{
    FILE *fp = fopen(INDEX_FILE, "r");
    if (!fp) 
    { /* no index file => no saves yet */ 
        printf("No saved users found.\n");
        return 0;
    }
    char line[NAME_BUF];
    int i = 0;
    while (fgets(line, sizeof(line), fp)) 
    {
        /* strip newline(s) */
        size_t L = strlen(line);
        while (L && (line[L-1] == '\n' || line[L-1] == '\r')) 
        { 
            line[--L] = '\0'; 
        }
        if (L == 0) 
            continue;
        ++i;
        printf("%d) %s\n", i, line);
    }
    fclose(fp);
    return i;
}


/* save_user_game
 * - writes the given Board and Player into "saves/<user>.txt"
 * - writes to a tmp file first and then rename() to be safer
 * - session_elapsed: number of seconds played in current session (optional)
 * - returns 0 on success, -1 on failure
 */
int save_user_game(const char *username_raw, Board *b, Player *p) 
{
    if (!username_raw || !b || !p) 
        return -1;
    
    char user[NAME_BUF];
    if (sanitize_username(username_raw, user, sizeof(user)) != 0) 
        return -1;

    ensure_saves_dir();

    char final_path[PATH_BUF];
    char tmp_path[PATH_BUF];
    build_user_path(user, final_path, sizeof(final_path));
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", final_path);

    FILE *fp = fopen(tmp_path, "w");
    if (!fp) 
        return -1;

    /* simple header */
    fprintf(fp, "%d %d %d\n", b->rows, b->cols, b->totalMines);
    // fprintf(fp, "cells:\n");
    for (int i = 0; i < b->rows; ++i) 
    {
        for (int j = 0; j < b->cols; ++j) 
        {
            Cell *c = &b->grid[i][j];
            /* same order as your current save: isMine isRevealed adjacentMines isFlagged */
            fprintf(fp, "%d %d %d %d ", c->isMine, c->isRevealed, c->adjacentMines, c->isFlagged);
        }
        fprintf(fp, "\n");
    }
    /* player line: name gamesPlayed gamesWon totalPlaySeconds */
    fprintf(fp, "player: %s\n", p->name);

    fflush(fp);
    fclose(fp);

    /* atomic rename (replace final if exists) */
    if (rename(tmp_path, final_path) != 0) 
    {
        remove(tmp_path);
        return -1;
    }

    /* ensure username is in index so list_saved_users can show it */
    add_user_to_index(user);
    return 0;
}

/* load_user_game
 * - reads "saves/<user>.txt" and fills Board and Player
 * - out_session_elapsed can be NULL if you don't need it
 * - returns 0 on success, -1 on failure
 */

int load_user_game(const char *username_raw, Board *b, Player *p) 
{
    if (!username_raw || !b || !p) 
        return -1;

    char user[NAME_BUF];
    if (sanitize_username(username_raw, user, sizeof(user)) != 0) 
        return -1;

    char path[PATH_BUF];
    build_user_path(user, path, sizeof(path));

    
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -1;
    

    int rows = 0, cols = 0, mines = 0;

    // Read header: rows cols mines
    if (fscanf(fp, "%d %d %d", &rows, &cols, &mines) != 3) 
    {
        fclose(fp);
        return -1;
    }

    // Basic sanity check (optional but good)
    if (rows <= 0 || cols <= 0 || rows > ROW_MAX || cols > COL_MAX) 
    {
        fclose(fp);
        return -1;
    }

    b->rows = rows;
    b->cols  = cols;
    b->totalMines = mines;

    // Read all cells: 4 ints per cell, row-major
    for (int i = 0; i < b->rows; ++i) 
    {
        for (int j = 0; j < b->cols; ++j) 
        {
            int isMine, isRevealed, adjacent, isFlagged;
            if (fscanf(fp, "%d %d %d %d", &isMine, &isRevealed, &adjacent, &isFlagged) != 4) 
            {
                fclose(fp);
                return -1;
            }
            b->grid[i][j].isMine         = isMine;
            b->grid[i][j].isRevealed     = isRevealed;
            b->grid[i][j].adjacentMines  = adjacent;
            b->grid[i][j].isFlagged      = isFlagged;
        }
    }

    // Read player line, e.g. "player: Abhi"
    char line[1024];

    //  Consume leftover end-of-line after the last numbers
    if (!fgets(line, sizeof(line), fp)) 
    {
        fclose(fp);
        return -1;
    }

    if (!fgets(line, sizeof(line), fp)) 
    {
        fclose(fp);
        return -1;
    }

    char pname[NAME_BUF];
    if (sscanf(line, "player: %63s", pname) != 1) 
    {
        fclose(fp);
        return -1;
    }
    strncpy(p->name, pname, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
    
    fclose(fp);
    return 0;
}
int clear_user_game(const char *username_raw, Board *b, Player *p){
    if (!username_raw || !b || !p) 
        return -1;

    char user[NAME_BUF];
    if (sanitize_username(username_raw, user, sizeof(user)) != 0) 
        return -1;

    char path[PATH_BUF];
    build_user_path(user, path, sizeof(path));

    
    FILE *fp = fopen(path, "w");
    if (!fp)
        return -1;
    return 0;
}