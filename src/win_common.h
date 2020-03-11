#define MAX_CMD 100
#define MAX_SRCH 100
struct wc {
	int  win_flags;
	char srch[MAX_SRCH];
	char cmd[MAX_CMD];
	const char *sel_pv;
	int  sel_pv_i;
	int  toprow;
};

extern struct wc wc;

