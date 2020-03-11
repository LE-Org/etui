#include <curses.h>
#include "win.h"
#include "win_data.h"

static WINDOW *win;

static void
main_recreate(int h, int w, int y, int x)
{
	if (win)
		delwin(win);
	win = newwin(h, w, y, x);
}

static int
get_tab_number(void)
{
	int i;
	if (!(win_flags & F_TAB_MASK))
		return -1;

	for (i = 1; i <= NTABS; i++)
		if (win_flags & F_TAB(i))
			return i;

	return -2;
}

static void
set_borders(void)
{
	if (windows[WIN_MAIN]->selected) {
		box(win, 0, 0);
		mvwprintw(win, 0, 2, "[%d]", get_tab_number());
	} else {
		wborder(win,' ',' ',' ',' ',' ',' ',' ',' ');
	}
}

static void
main_draw(void)
{
	if (wc.toprow == -1)
		return;

	werase(win);

	switch (win_flags & F_TAB_MASK) {
	case F_TAB(1):
		mvwaddstr(win, 1, 1, wc.sel_pv);
		mvwprintw(win, 2, 1, "%d.", wc.sel_pv_i);
		mvwprintw(win, 3, 1, "VAL = %s", gpvs[wc.sel_pv_i]->value);
		break;
	case F_TAB(2):
		mvwprintw(win, 1, 1, "TAB2");
		break;
	case F_TAB(3):
		mvwprintw(win, 1, 1, "TAB3");
		break;
	case F_TAB(4):
		mvwprintw(win, 1, 1, "TAB4");
		break;
	case F_TAB(5):
		mvwprintw(win, 1, 1, "TAB5");
		break;
	case F_TAB(6):
		mvwprintw(win, 1, 1, "TAB6");
		break;
	}
	set_borders();
}

static void
main_refresh(void)
{
	wrefresh(win);
}

static void
main_release(void)
{
	delwin(win);
	win = NULL;
}

static void
main_handle_key(int c)
{
	if (!windows[WIN_MAIN]->selected)
		return;

	switch (c) {
	/* tabs */
	case '1': case '2': case '3': case '4': case '5': case '6':
		win_flags &= ~F_TAB_MASK;
		win_flags |= F_TAB(c-'0');
		break;
	}
}

static struct win main_win_data = {
	.recreate   = main_recreate,
	.draw       = main_draw,
	._refresh   = main_refresh,
	.release    = main_release,
	.handle_key = main_handle_key
};

REGISTER_WINDOW(WIN_MAIN, &main_win_data);
