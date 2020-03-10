#ifndef WIN_REGISTRAR_H
#define WIN_REGISTRAR_H
#include "win.h"
#define MAX_N_WINDOWS 16

extern int n_windows;
extern struct win *windows[MAX_N_WINDOWS];

void register_window(int win_id, struct win *win);

#define REGISTER_WINDOW(win_id, win) \
	static void \
	_register_window() \
	{ \
		register_window(win_id, win); \
		n_windows++; \
	} \
	void \
	__attribute__((__section__(".init_array"))) (*_regwin_##win_id)() = _register_window

#endif
