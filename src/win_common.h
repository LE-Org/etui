#define MAX_BUF 100
struct wc {
	int  win_flags;
	char srch[MAX_BUF];
	char cmd[MAX_BUF];
	const char *sel_pv;
	int  sel_pv_i;
	int  toprow;
};

extern struct wc wc;

