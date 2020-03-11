#include <curses.h>
#include "win.h"
#include "win_common.h"
#include "win_data.h"

static WINDOW *win;

static void
fields_recreate(int h, int w, int y, int x)
{
	if (win)
		delwin(win);
	win = newwin(h, w, y, x);
}

static void
fields_draw(void)
{
	int i;

	if (wc.toprow == -1)
		return;

	for (i = 0; i < menu_h && (i+wc.toprow) < npvs; ++i)
		mvwprintw(win, i+1, 1, "%s", gpvs[i+wc.toprow]->value);
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
	.recreate = fields_recreate,
	._refresh = fields_refresh,
	.draw     = fields_draw,
	.release  = fields_release
};

REGISTER_WINDOW(WIN_FLDS, &fields_win_data);
