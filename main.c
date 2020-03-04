#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tui.h"
#include "cahelper.h"
#include "utils.h"

#define FPS (20)
#define LOOP_PERIOD_NS (1000000000 / FPS)

struct pv_entry {
	int entry_id;
	struct sub_info sub;
};

static int npvs = 0;
static char **pvs = NULL;
static struct pv_entry *pves = NULL;

void
pv_entry_callback(struct sub_info *sub)
{
	struct pv_entry *pve = (struct pv_entry *) sub->usr;
	char tmp[40];
	if (sub->dbr_type == DBR_GR_DOUBLE) {
		struct dbr_gr_double *pv = (struct dbr_gr_double *) sub->value;
		snprintf(tmp, 40, "%lf", pv->value);
		update_tui_entry(pve->entry_id, tmp);
	}
}

static void
init_pv_entry(struct pv_entry *pve, const char *name)
{
	pve->entry_id = create_tui_entry(name);
	init_sub_info(&pve->sub, name, pv_entry_callback, pve);
	subscribe_pv(&pve->sub);
}

static void
finish_pv_entry(struct pv_entry *pve)
{
	unsubscribe_pv(&pve->sub);
}

int
main(int argc, char **argv)
{
	int i;
	swallow_stderr();
	if (argc != 2 || !(pvs = process_input_file(argv[1]))) {
		return -1;
	}
	start_ca();
	start_tui();

	for (npvs = 0; pvs[npvs] != NULL; npvs++)
		;
	pves = malloc(npvs*sizeof(struct pv_entry));
	for (i = 0; i < npvs; i++) {
		init_pv_entry(&pves[i], pvs[i]);
	}

	for(;;) {
		process_ca_events();

		if (process_tui_events() != 0)
			break;

		wait_ns(LOOP_PERIOD_NS);
	}

	for (i=0; i < npvs; i++) {
		finish_pv_entry(&pves[i]);
	}
	free(pves);

	stop_tui();
	stop_ca();

	return 0;
}
