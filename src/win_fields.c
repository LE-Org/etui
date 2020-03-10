#include <curses.h>
#include <menu.h>
#include "win.h"
#include "win_data.h"
#include "win_menu.h"

static WINDOW *win;

static void
fields_recreate_window(int h, int w, int y, int x)
{
	if (win)
		delwin(win);
	win = newwin(h, w, y, x);
}

static void
fields_draw(void)
{
	int i;

	if (top_row(menu) == -1)
		return;

	for (i = 0; i < menu_h && (i+top_row(menu)) < npvs; ++i)
		mvwprintw(win, i+1, 1, "%s", gpvs[i+top_row(menu)]->value);
}

static void
fields_refresh(void)
{
	wrefresh(win);
}

static void
fields_release(void)
{
	delwin(win);
	win = NULL;
}

static struct win fields_win_data = {
	.recreate_window = fields_recreate_window,
	._refresh        = fields_refresh,
	.draw            = fields_draw,
	.release         = fields_release
};

REGISTER_WINDOW(WIN_FLDS, &fields_win_data);
