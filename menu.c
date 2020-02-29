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
    mvprintw(LINES - 2, 0, "F1 to Exit");
    post_menu(my_menu);
    refresh();

    while ((c = getch()) != KEY_F(1))
    {
        switch(c)
        {
            case KEY_DOWN:
                menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(my_menu, REQ_UP_ITEM);
                break;
        }
        mvaddstr(2, 40, item_name(current_item(my_menu)));
    }

    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
    free(my_items);

    unpost_menu(my_menu);
    free_menu(my_menu);

    endwin();
}
