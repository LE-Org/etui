#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tui.h"
#include "cahelper.h"
#include "utils.h"
#define FPS (20)
#define LOOP_PERIOD (1000000000 / FPS)

static int npvs = 0;
static char **pvs = NULL;

struct pv_entry {
	int entry_id;
	struct sub_info sub;
};

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
	int entry_id = create_tui_entry(name);
	pve->entry_id = entry_id;
	init_sub_info(&pve->sub, name, pv_entry_callback, pve);
	subscribe_pv(&pve->sub);
}

static void
finish_pv_entry(struct pv_entry *pve)
{
	unsubscribe_pv(&pve->sub);
}

/* TODO: get this to a separate file */
static int
process_input_file(const char *path)
{
	FILE *file = fopen(path, "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *buf;
	int i;

	if (!file)
		return 1;

	while ((nread = getline(&line, &len, file)) != -1) {
		for (i = 0; i < nread; ++i)
			if (line[i] == '\n' || line[i] == ' ')
				break;

		buf = malloc(i+1);
		strncpy(buf, line, i);
		buf[i] = '\0';

		if (buf == NULL)
			goto alloc_err;

		/* is that optimized in glibc? */
		pvs = realloc(pvs, (npvs + 1) * sizeof(char *));

		if (pvs == NULL)
			goto alloc_err;

		pvs[npvs] = buf;

		++npvs;
	}
	free(line);

	return 0;
alloc_err:
	free(buf);
	return 1;
}


int
main(int argc, char **argv)
{
	int i;
	int c;
	swallow_stderr();
	if (argc != 2 || process_input_file(argv[1])) {
		return -1;
	}
	start_ca();
	start_tui();

	pves = malloc(npvs*sizeof(struct pv_entry));
	for (i=0; i < npvs; i++) {
		init_pv_entry(&pves[i], pvs[i]);
	}

	for(;;) {
		process_ca_events();
		c = process_tui_events();
		/* quit condition */
		if (c == 'q')
			break;
		wait_tick(LOOP_PERIOD);
	}

	for (i=0; i < npvs; i++) {
		finish_pv_entry(&pves[i]);
	}
	free(pves);

	stop_tui();
	stop_ca();

	return 0;
}
