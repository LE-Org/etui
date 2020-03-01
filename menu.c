#include <stdio.h>  // getline
#include <stdlib.h> // malloc, realloc
#include <string.h> // strncpy

#include <curses.h>
#include <menu.h>

#define FIRST_ACTIVE_WIN WMENU

int npvs = 0;
char **pvs = NULL;
int **vals = NULL;

enum e_wins { WMENU, WMAIN, WSTAT } active_win;

int process_input_file(FILE *file)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *buf;
    int i;

    while ((nread = getline(&line, &len, file)) != -1) {
        for (i = 0; i < nread; ++i)
            if (line[i] == '\n' || line[i] == ' ')
                break;

        buf = malloc(i+1);
        strncpy(buf, line, i);
        buf[i] = '\0';

        pvs = realloc(pvs, (npvs + 1) * sizeof(char *));
        vals = realloc(vals, (npvs + 1) * sizeof(int *));

        pvs[npvs] = buf;
        vals[npvs] = malloc(sizeof(int));

        if (buf == NULL || pvs == NULL || vals == NULL || vals[npvs] == NULL)
            goto alloc_err;

        ++npvs;
    }
    free(line);

    return 0;
alloc_err:
    return 1;
}

void release()
{
    int i;
    for (i = 0; i < npvs; ++i) {
        free(pvs[i]);
        free(vals[i]);
    }
    free(pvs);
    free(vals);
}

void clear_border(WINDOW *w)
{
    wborder(w, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
}

void wmenu_search(WINDOW *win_menu, MENU *menu)
{
    int c;
    while ((c = getch()) != '\n') {
        mvwaddch(win_menu, 1, 1, '/');
        if (c == KEY_BACKSPACE) {
            menu_driver(menu, REQ_BACK_PATTERN);
        } else {
            menu_driver(menu, c); /* TODO: check c */
        }

        /* update search field, but leave '/' character alone */
        wmove(win_menu, 1, 2);
        wclrtoeol(win_menu);
        mvwaddstr(win_menu, 1, 2, menu_pattern(menu));

        (active_win == WMENU) ? box(win_menu, 0, 0) : clear_border(win_menu);
        wrefresh(win_menu);
    }

    /* hide search field */
    wmove(win_menu, 1, 1);
    wclrtoeol(win_menu);

    (active_win == WMENU) ? box(win_menu, 0, 0) : clear_border(win_menu);
    wrefresh(win_menu);
}


int main(int argc, char *argv[])
{
    FILE *file;
    WINDOW *win_menu, *win_main, *win_stat;
    int c;
    MENU *menu;
    ITEM **mitems;
    int i;

    if (argc != 2 || (file = fopen(argv[1], "r")) == NULL) {
        return -1;
    }
    if (process_input_file(file)) {
        fclose(file);
        fprintf(stderr, "Can't process file");
        return -2;
    }
    fclose(file);

    /* init ncurses */
    initscr();
    timeout(10);
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    /* define window dimensions */
    win_menu = newwin(LINES-3, 40, 0, 0);
    win_main = newwin(LINES-3, COLS-40, 0, 40);
    win_stat = newwin(3, COLS, LINES-3, 0);

    /* create items */
    mitems = (ITEM **)calloc(npvs + 1, sizeof(ITEM *));
    for (i = 0; i < npvs; ++i)
        mitems[i] = new_item(pvs[i], "");
    mitems[npvs] = (ITEM *)NULL;

    /* create menu */
    menu = new_menu((ITEM **)mitems);
    set_menu_win(menu, win_menu);
    set_menu_sub(menu, derwin(win_menu,10,20,5,5));
    set_menu_mark(menu, "-");
    post_menu(menu);

    active_win = FIRST_ACTIVE_WIN;

    while ((c = getch()) != 'q') {
        if (active_win == WMENU) {
            switch (c) {
            /* menu movement */
            case KEY_DOWN: case 'j':
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP: case 'k':
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case 'g':
                menu_driver(menu, REQ_FIRST_ITEM);
                break;
            case 'G':
                menu_driver(menu, REQ_LAST_ITEM);
                break;

            /* search in menu */
            case 'n':
                menu_driver(menu, REQ_NEXT_MATCH);
                break;
            case 'p':
                menu_driver(menu, REQ_PREV_MATCH);
                break;
            case '/': /* append to pattern match buffer */
                wmenu_search(win_menu, menu);
                break;
            }
        }

        switch (c) {
        /* select active win */
        case '\t':
            active_win = (active_win == WMENU) ? WMAIN : WMENU;
            break;
        }

        /* menu window */
        (active_win == WMENU) ? box(win_menu, 0, 0) : clear_border(win_menu);
        wrefresh(win_menu);

        /* main window */
        wclear(win_main);
        mvwaddstr(win_main, 1, 1, item_name(current_item(menu)));
        mvwprintw(win_main, 2, 1, "%d.", item_index(current_item(menu)));
        mvwprintw(win_main, 3, 1, "VAL = %d", vals[item_index(current_item(menu))]);
        (active_win == WMAIN) ? box(win_main, 0, 0) : clear_border(win_main);
        wrefresh(win_main);

        /* status window */
        mvwprintw(win_stat,1,2,"Press %c to Exit", 'q');
        box(win_stat, 0, 0);
        wrefresh(win_stat);
    }

    /* clean up */
    for (i = 0; i < npvs; ++i)
        free_item(mitems[i]);
    free(mitems);

    unpost_menu(menu);
    free_menu(menu);

    endwin();
    release();
}
