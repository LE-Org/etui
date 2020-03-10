#ifndef WIN_DATA_H
#define WIN_DATA_H

#define MAX_N_ENTRIES      1500
#define MAX_GPV_NAME_SIZE  41
#define MAX_GPV_VALUE_SIZE 41

#define NTABS          6
#define TUI_WCMDS_MASK 0x0030
#define KEY_ESC        27
#define F_TAB_MASK     0x3f00
#define F_HAS_COLORS   0x0001
#define F_KEY_ALT      0x0002 /* Alt, getch() returns 27 then other key */
#define F_KEY_ESC      0x0004 /* Esc, getch() returns 27 then ERR */
#define F_WCMDS_SRCH   0x0010 /* win_cmds is in search mode */
#define F_WCMDS_CMDS   0x0020 /* win_cmds is in command mode */
#define F_TAB(n)      (0x0100 << ((n)-1)) /* one based */

extern int want_quit;
extern unsigned win_flags;

enum win_func {
	WIN_MENU=0,
	WIN_FLDS,
	WIN_MAIN,
	WIN_CMDS,
	WIN_STAT
};

extern enum win_func active_win;

struct graphical_pv {
	char name[MAX_GPV_NAME_SIZE];
	char value[MAX_GPV_VALUE_SIZE];
};

extern int npvs;

extern struct graphical_pv *gpvs[MAX_N_ENTRIES + 1];

void release_data(void);

#endif
