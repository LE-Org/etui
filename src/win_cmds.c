#include <string.h>
#include <regex.h>

#include <curses.h>

#include "win.h"
#include "win_common.h"
#include "win_data.h"

static char *cmd = wc.cmd; /* wc.cmd alias */
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
	char *sol; /* start of line, adjust for wcmds_w cmd overflow */

	/* if strlen is less or equal to what fits, don't shift */
	sol = cmd + strlen(cmd) - (wcmds_w - 2);
	if (sol < cmd)
		sol = cmd;

	wmove(win, 0, 0);
	wclrtoeol(win);
	if (win_flags & F_WCMDS_CMDS)
		waddch(win, ':');
	else if (win_flags & F_WCMDS_SRCH)
		waddch(win, '/');
	waddnstr(win, sol, wcmds_w - 2);
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
	if (!status)
		wclear(win);
	visible = status;
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
		if (win_flags & F_WCMDS_CMDS)
			code = process_cmd();

		if (code) {
			return;
		}
	}

	if ((win_flags & F_KEY_ESC) || c == '\n') { /* cancel/finish */
		cmd[i=0] = '\0';
		win_flags &= ~TUI_WCMDS_MASK;
		windows_visible(WIN_CMDS, 0);
		windows_select(WIN_MENU);
	} else {
		if (c == KEY_BACKSPACE) {
			if (--i < 0)
				i = 0;
		} else if (i < MAX_CMD-1) {
			cmd[i++] = c;
		}
		cmd[i] = '\0';
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
