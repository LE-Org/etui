#include <string.h>
#include <regex.h>

#include <curses.h>

#include "win.h"
#include "win_data.h"

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
	char *inp;

	wmove(win, 0, 0);
	wclrtoeol(win);

	/* quit if win not active */
	if (!(win_flags & TUI_WCMDS_MASK))
		return;

	if (win_flags & F_WCMDS_SRCH)
		inp = wc.srch;
	else
		inp = wc.cmd;

	/* if strlen <= win_width, don't shift */
	sol = inp + strlen(inp) - (wcmds_w - 2);
	if (sol < inp)
		sol = inp;

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

	cmd_ = strtok(wc.cmd, " \t");

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

static int
add_key_to_buffer(int c, char *buf, int *pi) {
	if ((win_flags & F_KEY_ESC) || c == '\n') { /* cancel/confirm */
		if (c == '\n')
			return 1; /* confirm (enter) */
		return 2; /* cancel (escape) */
	} else {
		if (c == KEY_BACKSPACE) {
			if (--(*pi) < 0)
				*pi = 0;
		} else if (*pi < MAX_BUF-1) {
			buf[(*pi)++] = c;
		}
		buf[*pi] = '\0';
	}

	return 0;
}

static void
cmds_handle_key(int c)
{
	static int si = 0; /* search index */
	static int ci = 0; /* command index */
	int ret = 0;

	switch (win_flags & TUI_WCMDS_MASK) {
	case F_WCMDS_SRCH: /* search mode */
		ret = add_key_to_buffer(c, wc.srch, &si);
		if (ret == 2)
			wc.srch[si=0] = '\0';
		break;
	case F_WCMDS_CMDS: /* command mode */
		ret = add_key_to_buffer(c, wc.cmd, &ci);
		if (ret == 1)
			process_cmd();
		if (ret == 1 || ret == 2)
			wc.cmd[ci=0] = '\0';
		break;
	}

	if (ret) {
		win_flags &= ~TUI_WCMDS_MASK;
		windows_select(WIN_MENU);
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
