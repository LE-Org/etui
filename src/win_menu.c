#include <curses.h>
#include "tui.h"
#include "win.h"
#include "win_data.h"
#include "win_menu.h"

MENU *menu;
static ITEM *mitems[MAX_N_ENTRIES + 1];
static int selected = 0;
static WINDOW *win;

static void
recreate_items_from_pvs(void)
{
	/* state of items is altered when used, that's why this */
	/* needs to be called everytime we want to update the menu */
	int i;
	for (i=0; i < npvs; i++) {
		if (mitems[i])
			free_item(mitems[i]);
		mitems[i] = new_item(gpvs[i]->name, "");
	}
	mitems[i] = NULL;
}

static void
recreate_menu(void)
{
	if (menu) {
		unpost_menu(menu);
		free_menu(menu);
	}
	recreate_items_from_pvs();
	menu = new_menu((ITEM **) mitems);
	set_menu_win(menu, win);
	/* TODO: fix this memory leak */
	set_menu_sub(menu, derwin(win, menu_h, menu_w, 1, 1));
	set_menu_format(menu, menu_h, 1);
	set_menu_mark(menu, "-");
	post_menu(menu);
}

static void
menu_set_borders()
{
	if (selected)
		box(win, 0, 0);
	else
		wborder(win,' ',' ',' ',' ',' ',' ',' ',' ');
}

static void
menu_recreate(int h, int w, int y, int x)
{
	if (win) {
		unpost_menu(menu);
		free_menu(menu);
		menu = NULL;
		delwin(win);
	}
	menu_h = h - 2;
	menu_w = w - 2;
	win = newwin(h, w, y, x);
	recreate_menu();
}

static void
menu_draw(void)
{
	menu_set_borders();
}

static void
menu_refresh(void)
{
	wrefresh(win);
}

static void
menu_release()
{
	int i;
	/* clean up */
	for (i = 0; i < npvs; ++i)
		free_item(mitems[i]);

	unpost_menu(menu);
	free_menu(menu);
	delwin(win);
	win = NULL;
}

static void
menu_select(int status)
{
	selected = status;
}

static void
menu_data_changed(void)
{
	recreate_menu();
}

static void
menu_handle_key(int c)
{
	switch (c) {
	/* menu movement */
	case KEY_DOWN : case 'j': menu_driver(menu, REQ_DOWN_ITEM);  break;
	case KEY_UP   : case 'k': menu_driver(menu, REQ_UP_ITEM);    break;
	case KEY_NPAGE:           menu_driver(menu, REQ_SCR_DPAGE);  break;
	case KEY_PPAGE:           menu_driver(menu, REQ_SCR_UPAGE);  break;
	case 'g'      :           menu_driver(menu, REQ_FIRST_ITEM); break;
	case 'G'      :           menu_driver(menu, REQ_LAST_ITEM);  break;

	/* search in menu */
	case 'n':                 menu_driver(menu, REQ_NEXT_MATCH); break;
	case 'p':                 menu_driver(menu, REQ_PREV_MATCH); break;
	}
}

static struct win menu_win_data = {
	.recreate        = menu_recreate,
	.draw            = menu_draw,
	._refresh        = menu_refresh,
	.release         = menu_release,
	.select          = menu_select,
	.on_data_changed = menu_data_changed,
	.handle_key      = menu_handle_key
};

REGISTER_WINDOW(WIN_MENU, &menu_win_data);
