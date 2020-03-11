#include <stdlib.h>
#include "win_data.h"

/* windows dimensions */
int wmenu_h, wmenu_w;
int wflds_h, wflds_w;
int wmain_h, wmain_w;
int wstat_h, wstat_w;
int wcmds_h, wcmds_w;
int menu_h, menu_w;

struct wc wc;
int want_quit = 0;
unsigned win_flags = 0 | F_TAB(1);
int npvs = 0;
struct graphical_pv *gpvs[MAX_N_ENTRIES + 1];

void
release_data(void)
{
	int i;
	for (i=0; i < npvs; i++)
		free(gpvs[i]);
}

