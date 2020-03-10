#include <stdio.h>

#include "cahelper.h"

/* Example of using CA helper functions */

void
callback(struct sub_info *sub)
{
	if (sub->dbr_type == DBR_GR_DOUBLE) {
		struct dbr_gr_double *pv = (struct dbr_gr_double *) sub->value;
		printf("Got type: %ld value: %f\n", sub->dbr_type, pv->value);
	}
}

int
main()
{
	struct sub_info sub;
	start_ca();
	init_sub_info(&sub, "user:ai1", callback, NULL);
	subscribe_pv(&sub);
	while (1) {
		process_ca_events();
	}
	unsubscribe_pv(&sub);
	stop_ca();
	return 0;
}

