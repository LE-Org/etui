#include <stdlib.h>
#include "win_data.h"

int want_quit = 0;
unsigned win_flags = 0 | F_TAB(1);
enum win_func active_win;
int npvs = 0;
struct graphical_pv *gpvs[MAX_N_ENTRIES + 1];

void
release_data(void)
{
    int i;
    for (i=0; i < npvs; i++)
	free(gpvs[i]);
}

