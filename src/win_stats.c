#include <curses.h>
#include "tui.h"
#include "win.h"
#include "win_data.h"

static WINDOW *win;

static void
stats_recreate(int h, int w, int y, int x)
{
	if (win)
		delwin(win);
	win = newwin(h, w, y, x);
	wstat_h = h;
	wstat_w = w;

	/* color windows */
	if (win_flags & F_HAS_COLORS) {
		/* win_stat */
		wbkgd(win, COLOR_PAIR(1));
	} else {
		int y, x;

		/* win_stat */
		/* fake wbkgd by filling spaces in reverse video */
		wattr_on(win, A_REVERSE, NULL);
		for (y = 0; y < wstat_h; y++) {
			wmove(win, y, 0);
			for (x = 0; x < wstat_w; x++)
				waddstr(win, " ");
		}
	}
}

static void
stats_draw(void)
{
	mvwprintw(win, 0, 1,
	          "%d PVs, "
	          "active_win = %d, "
	          "win_flags = 0x%04x",
	          npvs, active_win, win_flags);
}

static void
stats_refresh(void)
{
	wrefresh(win);
}

static void
stats_release(void)
{
	delwin(win);
}

static struct win stats_win_data = {
	.recreate = stats_recreate,
	.draw     = stats_draw,
	._refresh = stats_refresh,
	.release  = stats_release
};

REGISTER_WINDOW(WIN_STAT, &stats_win_data);
