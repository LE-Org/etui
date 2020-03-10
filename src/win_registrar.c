#include <stdio.h>
#include <assert.h>

#include "win_registrar.h"

int n_windows = 0;
struct win *windows[MAX_N_WINDOWS];

void
register_window(int win_id, struct win *win)
{
	assert(win_id >= 0 && win_id < MAX_N_WINDOWS);
	windows[win_id] =  win;
	n_windows++;
}
