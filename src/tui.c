#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <menu.h>

#include "tui.h"

#define WMENU_H (LINES - WSTAT_H - WCMDS_H)
#define WMENU_W 40
#define WFLDS_H WMENU_H
#define WFLDS_W 20
#define WMAIN_H WMENU_H
#define WMAIN_W (COLS - WMENU_W - WFLDS_W)
#define WSTAT_H 1
#define WSTAT_W COLS
#define WCMDS_H 1
#define WCMDS_W COLS

#define MENU_H (WMENU_H - 2)
#define MENU_W (WMENU_W - 2)

#define NC_TIMEOUT 50

#define MAX_N_ENTRIES 1500

#define TUI_HAS_COLORS 0x0001

static int wcmds_search(int);
static void recreate_menu(void);
static void recreate_items_from_pvs(void);
static void border_if_active(WINDOW *);
static void release(void);
int start_tui(void);
int stop_tui(void);
int create_tui_entry(const char *);
int update_tui_entry(int, const char *);
static void draw_win_menu(void);
static void draw_win_flds(void);
static void draw_win_main(void);
static void draw_win_stat(void);
static void draw_win_cmds(void);
static void draw_windows(void);
int process_tui_events(void);

static int npvs = 0;
static struct graphical_pv *gpvs[MAX_N_ENTRIES + 1];
static WINDOW *win_menu, *win_flds, *win_main, *win_stat, *win_cmds;
static MENU *menu;
static ITEM *mitems[MAX_N_ENTRIES + 1];
static WINDOW *active_win;
static unsigned tui_flags = 0;

static int
wcmds_search(int c)
{
	if (c == '\n') {
		/* hide search field */
		wmove(win_cmds, 0, 0);
		wclrtoeol(win_cmds);

		return 1;
	}

	if (c == KEY_BACKSPACE)
		menu_driver(menu, REQ_BACK_PATTERN);
	else
		menu_driver(menu, c); /* TODO: check c */

	/* update search field, but leave '/' character alone */
	wmove(win_cmds, 0, 1);
	wclrtoeol(win_cmds);
	mvwaddstr(win_cmds, 0, 1, menu_pattern(menu));

	return 0;
}

static void
recreate_menu(void)
{
	if (menu) {
		unpost_menu(menu);
		free_menu(menu);
	}
	recreate_items_from_pvs();
	menu = new_menu((ITEM **)mitems);
	set_menu_win(menu, win_menu);
	set_menu_sub(menu, derwin(win_menu,MENU_H,MENU_W,1,1));
	set_menu_format(menu, MENU_H, 1);
	set_menu_mark(menu, "-");
	post_menu(menu);
}

static void
refresh_menu_items(void)
{
	unpost_menu(menu);
	recreate_items_from_pvs();
	set_menu_items(menu, mitems);
	post_menu(menu);
}

static void
recreate_items_from_pvs(void)
{
	/* state of items is altered when used, that's why this */
	/* needs to be called everytime we want to update the menu */
	int i;
	for (i=0; i < npvs; i++) {
		if (mitems[i]) {
			free_item(mitems[i]);
		}
		mitems[i] = new_item(gpvs[i]->name, "");
	}
	mitems[i] = NULL;
}


static void
border_if_active(WINDOW *w)
{
	if (w == active_win)
		box(w, 0, 0);
	else
		wborder(w,' ',' ',' ',' ',' ',' ',' ',' ');
}

static void
release(void)
{
    int i;
    for (i=0; i < npvs; i++)
	free(gpvs[i]);
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
		tui_flags |= TUI_HAS_COLORS;
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
	}

	/* define window dimensions */
	win_menu = newwin(WMENU_H, WMENU_W, 0, 0);
	win_flds = newwin(WFLDS_H, WFLDS_H, 0, WMENU_W);
	win_main = newwin(WMAIN_H, WMAIN_W, 0, WMENU_W + WFLDS_W);
	win_stat = newwin(WSTAT_H, WSTAT_W, WMENU_H, 0);
	win_cmds = newwin(WCMDS_H, WCMDS_W, WMENU_H + WSTAT_H, 0);

	recreate_menu();

	/* color windows */
	if (tui_flags & TUI_HAS_COLORS) {
		/* win_stat */
		wbkgd(win_stat, COLOR_PAIR(1));
	} else {
		int y, x;

		/* win_stat */
		/* fake wbkgd by filling spaces in reverse video */
		wattr_on(win_stat, A_REVERSE, NULL);
		for (y = 0; y < WSTAT_H; y++) {
			wmove(win_stat, y, 0);
			for (x = 0; x < WSTAT_W; x++)
				waddstr(win_stat, " ");
		}
	}
	active_win = win_menu;

	return 0;
}

int
stop_tui(void)
{
	int i;
	/* clean up */
	for (i = 0; i < npvs; ++i)
		free_item(mitems[i]);

	unpost_menu(menu);
	free_menu(menu);

	endwin();
	release();
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
	refresh_menu_items();
	wrefresh(win_menu);
	return entry_id;
}

int
update_tui_entry(int entry_id, const char *value)
{
	assert(entry_id < npvs && entry_id >= 0);
	strncpy(gpvs[entry_id]->value, value, MAX_GPV_VALUE_SIZE);
	return 0;
}

static void
draw_win_menu(void)
{
	border_if_active(win_menu);
	wrefresh(win_menu);
}

static void
draw_win_flds(void)
{
	int i;

	if (top_row(menu) == -1)
		return;

	for (i = 0; i < MENU_H && (i+top_row(menu)) < npvs; ++i)
		mvwprintw(win_flds,i+1,1,"%s",gpvs[i+top_row(menu)]->value);
	wrefresh(win_flds);
}

static void
draw_win_main(void)
{
	if (top_row(menu) == -1)
		return;

	wmove(win_main, 1, 1); wclrtoeol(win_main);
	mvwaddstr(win_main, 1, 1, item_name(current_item(menu)));
	wmove(win_main, 2, 1); wclrtoeol(win_main);
	mvwprintw(win_main, 2, 1, "%d.",
		  item_index(current_item(menu)));
	wmove(win_main, 3, 1); wclrtoeol(win_main);
	mvwprintw(win_main, 3, 1, "VAL = %s",
		  gpvs[item_index(current_item(menu))]->value);
	border_if_active(win_main);
	wrefresh(win_main);
}

static void
draw_win_stat(void)
{
	mvwprintw(win_stat,0,1,"%d PVs", npvs);
	wrefresh(win_stat);
}

static void
draw_win_cmds(void)
{
	wrefresh(win_cmds);
}

static void
draw_windows(void)
{
	draw_win_menu();
	draw_win_flds();
	draw_win_main();
	draw_win_stat();
	draw_win_cmds();
}

int
process_tui_events(void)
{
	int c;

	c = getch();
	if (c == 'q') /* quit condition */
		return 1;
	if (active_win == win_cmds) {
		if (wcmds_search(c)) { /* finish */
			wmove(win_cmds, 0, 0); wclrtoeol(win_cmds);
			active_win = win_menu;
		}
	}
	if (active_win == win_menu) {
		switch (c) {
		/* menu movement */
		case KEY_DOWN: case 'j':
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP: case 'k':
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case KEY_NPAGE:
			menu_driver(menu, REQ_SCR_DPAGE);
			break;
		case KEY_PPAGE:
			menu_driver(menu, REQ_SCR_UPAGE);
			break;
		case 'g':
			menu_driver(menu, REQ_FIRST_ITEM);
			break;
		case 'G':
			menu_driver(menu, REQ_LAST_ITEM);
			break;

		/* search in menu */
		case 'n':
			menu_driver(menu, REQ_NEXT_MATCH);
			break;
		case 'p':
			menu_driver(menu, REQ_PREV_MATCH);
			break;
		}
	}

	switch (c) {
	/* select active win */
	case '\t':
		active_win = (active_win == win_menu) ?
			     win_main : win_menu;
		break;
	/* search mode */
	case '/':
		mvwaddch(win_cmds, 0, 0, '/');
		active_win = win_cmds;
		break;
	/* command mode */
	case ':':
		mvwaddch(win_cmds, 0, 0, ':');
		break;
	}

	draw_windows();

	return 0;
}

