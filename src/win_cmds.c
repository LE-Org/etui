#include <string.h>
#include <curses.h>
#include <menu.h>
#include "win.h"
#include "win_cmds.h"
#include "win_data.h"
#include "win_menu.h"
#include "win_stats.h"

static char cmd[MAX_CMD];
static char visible = 0;
static WINDOW *win;

static void
cmds_recreate(int h, int w, int y, int x)
{
	if (win)
		delwin(win);
	win = newwin(h, w, y, x);
}

static void
cmds_draw(void)
{
}

static void
cmds_refresh(void)
{
	wrefresh(win);
}

static void
cmds_release(void)
{
	delwin(win);
	win = NULL;
}

static void
cmds_visible(int status)
{
	if (status) {
		if (win_flags & F_WCMDS_CMDS) {
			cmd[0] = '\0';
			mvwaddch(win, 0, 0, ':');
		} else if (win_flags & F_WCMDS_SRCH) {
			cmd[0] = '\0';
			mvwaddch(win, 0, 0, '/');
		}
	} else {
		wclear(win);
	}
	visible = status;
}

static int
wcmds_search(int c)
{
	if (c == KEY_BACKSPACE)
		menu_driver(menu, REQ_BACK_PATTERN);
	else
		menu_driver(menu, c); /* TODO: check c */

	/* update search field, but leave '/' character alone */
	wmove(win, 0, 1);
	wclrtoeol(win);
	mvwaddstr(win, 0, 1, menu_pattern(menu));

	return 0;
}

static void
wcmds_commands(void)
{
	char *sol; /* start of line, adjust for wstat_w cmd overflow */

	/* if strlen is less or equal to what fits, don't shift */
	sol = cmd + strlen(cmd) - (wstat_w - 2);
	if (sol < cmd)
		sol = cmd;

	wmove(win, 0, 1);
	wclrtoeol(win);
	mvwaddnstr(win, 0, 1, sol, wstat_w - 2);
}

static int
process_cmd(void)
{
	char *cmd_;

	cmd_ = strtok(cmd, " \t");

	if (!cmd_)
		return 0;

	if (!strcmp(cmd_, "quit") || !strcmp(cmd_, "q")) {
		want_quit = 1;
		return 1;
	}

	/* only commands with no trash at the end are valid */
	if (strtok(NULL, " \t") == NULL)
		return 0;

	return 0;
}

static void
cmds_handle_key(int c)
{
	static int i = 0;
	int code = 0;

	if (!visible)
		return;

	if (c == '\n') { /* confirm */
		switch (win_flags & TUI_WCMDS_MASK) {
		case F_WCMDS_CMDS: code = process_cmd(); break;
		case F_WCMDS_SRCH: break; /* search as-you-type */
		}
		if (code) {
			return;
		}
	}

	if ((win_flags & F_KEY_ESC) || c == '\n') { /* cancel/finish */
		cmd[i=0] = '\0';
		win_flags &= ~TUI_WCMDS_MASK;
		windows_visible(WIN_CMDS, 0);
		active_win = WIN_MENU;
	} else {
		if (c == KEY_BACKSPACE) {
			if (--i < 0)
				i = 0;
		} else if (i < MAX_CMD-1) {
			cmd[i++] = c;
		}
		cmd[i] = '\0';

		switch (win_flags & TUI_WCMDS_MASK) {
		case F_WCMDS_CMDS: wcmds_commands(); break;
		case F_WCMDS_SRCH: wcmds_search(c); break;
		}
	}
}

static struct win cmds_win_data = {
	.recreate   = cmds_recreate,
	.draw       = cmds_draw,
	._refresh   = cmds_refresh,
	.release    = cmds_release,
	.visible    = cmds_visible,
	.handle_key = cmds_handle_key
};

REGISTER_WINDOW(WIN_CMDS, &cmds_win_data);
