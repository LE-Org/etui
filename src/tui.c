#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tui.h"
#include "win.h"
#include "win_data.h"
#include "win_menu.h"
#include "win_fields.h"
#include "win_main.h"
#include "win_cmds.h"
#include "win_stats.h"

#define WSTAT_H 1
#define WCMDS_H 1
#define WMENU_FRAC (2)
#define WFLDS_FRAC (1)
#define WMAIN_FRAC (3)
#define TOTAL_FRAC (WMENU_FRAC + WFLDS_FRAC + WMAIN_FRAC)

#define NC_TIMEOUT 50

static void
recreate_windows()
{
	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);
	wmenu_h = maxy - WSTAT_H - WCMDS_H;
	wflds_h = wmain_h = wmenu_h;
	wstat_h = WSTAT_H;
	wcmd_h = WCMDS_H;
	wmenu_w = maxx * WMENU_FRAC / TOTAL_FRAC;
	wflds_w = maxx * WFLDS_FRAC / TOTAL_FRAC;
	wmain_w = maxx - wmenu_w - wflds_w;
	wstat_w = wcmd_w = maxx;

	/* define window dimensions */
	windows[WIN_MENU]->recreate(wmenu_h, wmenu_w, 0, 0);
	windows[WIN_FLDS]->recreate(wflds_h, wflds_w, 0, wmenu_w);
	windows[WIN_MAIN]->recreate(wmain_h, wmain_w, 0, wmenu_w + wflds_w);
	windows[WIN_CMDS]->recreate(wcmd_h, wcmd_w, wmenu_h + wstat_h, 0);
	windows[WIN_STAT]->recreate(wstat_h, wstat_w, wmenu_h, 0);
}

int
start_tui(void)
{
	npvs = 0;
	/* init ncurses */
	initscr();
	timeout(NC_TIMEOUT);
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	if (has_colors()) {
		win_flags |= F_HAS_COLORS;
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
	}

	/* create panels */
	recreate_windows();
	windows_select(WIN_MENU, 1);

	return 0;
}

int
stop_tui(void)
{
	windows_release();
	endwin();
	release_data();
	return 0;
}

int
create_tui_entry(const char *name)
{
	int entry_id = npvs;
	npvs++;
	assert(entry_id < MAX_N_ENTRIES);
	
	gpvs[entry_id] = calloc(1, sizeof(struct graphical_pv));
	strncpy(gpvs[entry_id]->name, name, MAX_GPV_NAME_SIZE - 1);
	gpvs[entry_id]->value[0] = '\0';

	/* create items */
	windows_on_data_changed();
	return entry_id;
}

int
update_tui_entry(int entry_id, const char *value)
{
	assert(entry_id < npvs && entry_id >= 0);
	strncpy(gpvs[entry_id]->value, value, MAX_GPV_VALUE_SIZE);
	return 0;
}

int
process_tui_events(void)
{
	int c;
	enum cmd_code code = C_NONE;

	c = getch();
	if (c == ERR)
		goto end_refresh;

	win_flags &= ~(F_KEY_ESC|F_KEY_ALT);
	if (c == KEY_ESC) {
		c = getch();
		if (c == ERR)
			win_flags |= F_KEY_ESC;
		else
			win_flags |= F_KEY_ALT;
	}

	if (c == KEY_RESIZE) {
		recreate_windows();
		return C_NONE;
	}

	windows_handle_key(c);

	if (want_quit)
		return C_QUIT;

	if (active_win != WIN_CMDS) {
		switch (c) {
		/* select active win */
		case '\t':
			windows_select(active_win, 0);
			active_win = (active_win == WIN_MENU) ?
				     WIN_MAIN : WIN_MENU;
			windows_select(active_win, 1);
			break;
		/* search mode */
		case '/':
			win_flags |= F_WCMDS_SRCH;
			windows_visible(WIN_CMDS, 1);
			active_win = WIN_CMDS;
			break;
		/* command mode */
		case ':':
			win_flags |= F_WCMDS_CMDS;
			windows_visible(WIN_CMDS, 1);
			active_win = WIN_CMDS;
			break;
		}
	}

end_refresh:
	windows_draw();
	windows_refresh();

	return code;
}

