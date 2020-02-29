#include <stdlib.h>

#include <curses.h>
#include <menu.h>

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

char *pvs[] = {
    "EX:AAA",
    "EX:BBB",
    "EX:CCC",
    "EX:DDD",
};

char *desc[] = {
    "aaa",
    "bbb",
    "ccc",
    "ddd",
};

int main()
{
    ITEM **my_items;
    int c;
    MENU *my_menu;
    int n_choices, i;

    initscr();
    timeout(10);
    noecho();
    keypad(stdscr, TRUE);

    n_choices = NELEMS(pvs);
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (i = 0; i < n_choices; ++i)
        my_items[i] = new_item(pvs[i], desc[i]);
    my_items[n_choices] = (ITEM *)NULL;

    my_menu = new_menu((ITEM **)my_items);
    mvprintw(LINES - 2, 0, "Press q to Exit");
    post_menu(my_menu);
    refresh();

    while ((c = getch()) != 'q') {
        switch(c) {
        case KEY_DOWN: case 'j':
            menu_driver(my_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP: case 'k':
            menu_driver(my_menu, REQ_UP_ITEM);
            break;
        case 'g':
            menu_driver(my_menu, REQ_FIRST_ITEM);
            break;
        case 'G':
            menu_driver(my_menu, REQ_LAST_ITEM);
            break;

        /* search in menu */
        case 'n':
            menu_driver(my_menu, REQ_NEXT_MATCH);
            break;
        case 'p':
            menu_driver(my_menu, REQ_PREV_MATCH);
            break;
        case '/': /* append to pattern match buffer */
            mvaddch(LINES-3, 0, '/');
            while ((c = getch()) != '\n') {
                if (c == KEY_BACKSPACE) {
                    menu_driver(my_menu, REQ_BACK_PATTERN);
                    continue;
                }
                menu_driver(my_menu, c); /* TODO: check c */

                move(LINES-3, 1);
                clrtoeol();
                mvaddstr(LINES-3, 1, menu_pattern(my_menu));
            }

            move(LINES-3, 0);
            clrtoeol();
            refresh();
            break;
        }

        move(2, 40);
        clrtoeol();
        mvaddstr(2, 40, item_name(current_item(my_menu)));
    }

    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
    free(my_items);

    unpost_menu(my_menu);
    free_menu(my_menu);

    endwin();
}
