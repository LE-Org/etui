#include <regex.h>

#include <curses.h>
#include <menu.h>

#include "win.h"
#include "win_data.h"
#include "win_common.h"

static MENU *menu;
static ITEM *mitems[MAX_N_ENTRIES + 1];
static WINDOW *win;

static int search(int off, int dir);

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
	if (windows[WIN_MENU]->selected)
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
	for (i = 0; i < npvs; ++i)
		free_item(mitems[i]);

	unpost_menu(menu);
	free_menu(menu);
	delwin(win);
	win = NULL;
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
	/* search next/previous */
	case 'n': search(1,1); break;
	case 'p': search(-1,-1); break;
	}

	/* update common data */
	wc.toprow = top_row(menu);
	wc.sel_pv = item_name(current_item(menu));
	wc.sel_pv_i = item_index(current_item(menu));
}

static int
search(int off, int dir)
{
	int i, n;
	ITEM *item, *match;
	regex_t preg;

	n = item_count(menu);
	off += item_index(current_item(menu));

	regcomp(&preg, wc.srch, REG_NOSUB | REG_EXTENDED);

	match = NULL;
	for (i = 0; i < n; ++i) {
		item = menu->items[(n+dir*i+off)%n];
		if (regexec(&preg, item->name.str,0,0,0) == 0) {
			match = item;
			break;
		}
	}
	regfree(&preg);

	if (match)
		set_current_item(menu, match);

	return 0;
}

static void
menu_handle_passive(void)
{
	if (win_flags & F_WCMDS_SRCH)
		search(0,1); /* search as you type */
}

static struct win menu_win_data = {
	.recreate        = menu_recreate,
	.draw            = menu_draw,
	._refresh        = menu_refresh,
	.release         = menu_release,
	.on_data_changed = menu_data_changed,
	.handle_key      = menu_handle_key,
	.handle_passive  = menu_handle_passive
};

REGISTER_WINDOW(WIN_MENU, &menu_win_data);
