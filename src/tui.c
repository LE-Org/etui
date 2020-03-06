#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <menu.h>

#include "tui.h"

#define WSTAT_H 1
#define WCMDS_H 1
#define WMENU_FRAC (2)
#define WFLDS_FRAC (1)
#define WMAIN_FRAC (3)
#define TOTAL_FRAC (WMENU_FRAC + WFLDS_FRAC + WMAIN_FRAC)

#define NC_TIMEOUT 50

#define MAX_N_ENTRIES 1500

#define TUI_HAS_COLORS 0x0001
#define TUI_TAB(n)    (0x0100 << ((n)-1)) /* one based */

#define NTABS          6
#define TUI_TAB_MASK   0x3f00

static int wcmds_search(int);
static void recreate_menu(void);
static void recreate_items_from_pvs(void);
static void set_borders(void);
static int get_tab_number(void);
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
static void refresh_windows(void);
int process_tui_events(void);

static int npvs = 0;
static struct graphical_pv *gpvs[MAX_N_ENTRIES + 1];

#define N_WINDOWS (5)
enum {
	WIN_MENU=0,
	WIN_FLDS,
	WIN_MAIN,
	WIN_STAT,
	WIN_CMDS
} active_win;

static WINDOW *win[N_WINDOWS];
static MENU *menu;
static ITEM *mitems[MAX_N_ENTRIES + 1];
static unsigned tui_flags = 0 | TUI_TAB(1);

// windows dimensions
int wmenu_h, wmenu_w, wflds_h, wflds_w;
int wmain_h, wmain_w;
int menu_h, menu_w;
int wstat_w, wcmd_w;

static int
wcmds_search(int c)
{
	if (c == '\n') {
		/* hide search field */
		wmove(win[WIN_CMDS], 0, 0);
		wclrtoeol(win[WIN_CMDS]);

		return 1;
	}

	if (c == KEY_BACKSPACE)
		menu_driver(menu, REQ_BACK_PATTERN);
	else
		menu_driver(menu, c); /* TODO: check c */

	/* update search field, but leave '/' character alone */
	wmove(win[WIN_CMDS], 0, 1);
	wclrtoeol(win[WIN_CMDS]);
	mvwaddstr(win[WIN_CMDS], 0, 1, menu_pattern(menu));

	return 0;
}

static void
recreate_windows()
{
	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);
	wmenu_h = maxy - WSTAT_H - WCMDS_H;
	wflds_h = wmain_h = wmenu_h;
	wmenu_w = maxx * WMENU_FRAC / TOTAL_FRAC;
	wflds_w = maxx * WFLDS_FRAC / TOTAL_FRAC;
	wmain_w = maxx - wmenu_w - wflds_w;
	wstat_w = wcmd_w = maxx;
	if (win[WIN_MENU]) {
		unpost_menu(menu);
		free_menu(menu);
		menu = NULL;
		delwin(win[WIN_MAIN]);
		delwin(win[WIN_FLDS]);
		delwin(win[WIN_MAIN]);
		delwin(win[WIN_STAT]);
		delwin(win[WIN_CMDS]);
	}
	/* define window dimensions */
	win[WIN_MENU] = newwin(wmenu_h, wmenu_w, 0, 0);
	win[WIN_FLDS] = newwin(wflds_h, wflds_w, 0, wmenu_w);
	win[WIN_MAIN] = newwin(wmain_h, wmain_w, 0, wmenu_w + wflds_w);
	win[WIN_STAT] = newwin(WSTAT_H, wstat_w, wmenu_h, 0);
	win[WIN_CMDS] = newwin(WCMDS_H, wcmd_w, wmenu_h + WSTAT_H, 0);

	/* color windows */
	if (tui_flags & TUI_HAS_COLORS) {
		/* win_stat */
		wbkgd(win[WIN_STAT], COLOR_PAIR(1));
	} else {
		int y, x;

		/* win_stat */
		/* fake wbkgd by filling spaces in reverse video */
		wattr_on(win[WIN_STAT], A_REVERSE, NULL);
		for (y = 0; y < WSTAT_H; y++) {
			wmove(win[WIN_STAT], y, 0);
			for (x = 0; x < wstat_w; x++)
				waddstr(win[WIN_STAT], " ");
		}
	}
}

static void
recreate_menu(void)
{
	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);
	menu_h = maxy - WSTAT_H - WCMDS_H - 2;
	menu_w = maxx * WMENU_FRAC / TOTAL_FRAC - 2;
	if (menu) {
		unpost_menu(menu);
		free_menu(menu);
	}
	recreate_items_from_pvs();
	menu = new_menu((ITEM **) mitems);
	set_menu_win(menu, win[WIN_MENU]);
	/* TODO: fix this memory leak */
	set_menu_sub(menu, derwin(win[WIN_MENU], menu_h, menu_w, 1, 1));
	set_menu_format(menu, menu_h, 1);
	set_menu_mark(menu, "-");
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
set_borders()
{
	if (active_win == WIN_MENU) {
		box(win[WIN_MENU], 0, 0);
		wborder(win[WIN_MAIN],' ',' ',' ',' ',' ',' ',' ',' ');
	} else if (active_win == WIN_MAIN) {
		box(win[WIN_MAIN], 0, 0);
		mvwprintw(win[WIN_MAIN], 0, 2, "[%d]", get_tab_number());
		wborder(win[WIN_MENU],' ',' ',' ',' ',' ',' ',' ',' ');
	}
}

static int
get_tab_number(void)
{
	int i;
	if (!(tui_flags & TUI_TAB_MASK))
		return -1;

	for (i = 1; i <= NTABS; i++)
		if (tui_flags & TUI_TAB(i))
			return i;

	return -2;
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

	/* create panels */
	recreate_windows();
	recreate_menu();
	active_win = WIN_MENU;

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
	recreate_menu();
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
}

static void
draw_win_flds(void)
{
	int i;

	if (top_row(menu) == -1)
		return;

	for (i = 0; i < menu_h && (i+top_row(menu)) < npvs; ++i)
		mvwprintw(win[WIN_FLDS],i+1,1,"%s",gpvs[i+top_row(menu)]->value);
}

static void
draw_win_main(void)
{
	if (top_row(menu) == -1)
		return;

	werase(win[WIN_MAIN]);

	switch (tui_flags & TUI_TAB_MASK) {
	case TUI_TAB(1):
		mvwaddstr(win[WIN_MAIN], 1, 1, item_name(current_item(menu)));
		mvwprintw(win[WIN_MAIN], 2, 1, "%d.",
			  item_index(current_item(menu)));
		mvwprintw(win[WIN_MAIN], 3, 1, "VAL = %s",
			  gpvs[item_index(current_item(menu))]->value);
		break;
	case TUI_TAB(2):
		mvwprintw(win[WIN_MAIN], 1, 1, "TAB2");
		break;
	case TUI_TAB(3):
		mvwprintw(win[WIN_MAIN], 1, 1, "TAB3");
		break;
	case TUI_TAB(4):
		mvwprintw(win[WIN_MAIN], 1, 1, "TAB4");
		break;
	case TUI_TAB(5):
		mvwprintw(win[WIN_MAIN], 1, 1, "TAB5");
		break;
	case TUI_TAB(6):
		mvwprintw(win[WIN_MAIN], 1, 1, "TAB6");
		break;
	}
}

static void
draw_win_stat(void)
{
	mvwprintw(win[WIN_STAT],0,1,"%d PVs", npvs);
}

static void
draw_win_cmds(void)
{
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

static void
refresh_windows(void)
{
	wrefresh(win[WIN_MENU]);
	wrefresh(win[WIN_FLDS]);
	wrefresh(win[WIN_MAIN]);
	wrefresh(win[WIN_STAT]);
	wrefresh(win[WIN_CMDS]);
}

int
process_tui_events(void)
{
	int c;

	c = getch();
	if (c == ERR)
		goto refresh;

	if (c == KEY_RESIZE) {
		recreate_windows();
		recreate_menu();
		return 0;
	}
	if (c == 'q') /* quit condition */
		return 1;
	if (active_win == WIN_CMDS) {
		if (wcmds_search(c)) { /* finish */
			wmove(win[WIN_CMDS], 0, 0); wclrtoeol(win[WIN_CMDS]);
			active_win = WIN_MENU;
		}
	}
	if (active_win == WIN_MENU) {
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
	if (active_win == WIN_MAIN) {
		switch (c) {
		/* tabs */
		case '1': case '2': case '3': case '4': case '5': case '6':
			tui_flags &= ~TUI_TAB_MASK;
			tui_flags |= TUI_TAB(c-'0');
			break;
		}
	}

	switch (c) {
	/* select active win */
	case '\t':
		active_win = (active_win == WIN_MENU) ?
			     WIN_MAIN : WIN_MENU;
		break;
	/* search mode */
	case '/':
		mvwaddch(win[WIN_CMDS], 0, 0, '/');
		active_win = WIN_CMDS;
		break;
	/* command mode */
	case ':':
		mvwaddch(win[WIN_CMDS], 0, 0, ':');
		break;
	}

refresh:
	draw_windows();      /* set_borders should be called after, so that we */
	set_borders();       /* are free to use wclear inside of draw_windows  */
	refresh_windows();   /* we can refresh only after both are done */

	return 0;
}
