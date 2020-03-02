#ifndef CAHELPER_H
#define CAHELPER_H

#include <cadef.h>

#define CA_TIMEOUT (1.0)
#define CA_EVENT_MASK (DBE_VALUE | DBE_ALARM)
#define MAX_PV_NAME_SIZE (40)
#define CA_PRIORITY  (0)

struct sub_info;
typedef void sub_callback(struct sub_info *sub);

struct sub_info {
	char name[MAX_PV_NAME_SIZE];
	chid chid;
	int eca_err;
	long dbf_type;
	long dbr_type;
	evid event_id;
	char initialized;
	unsigned long count;
	void *value;
	sub_callback *callback;
	void *usr;
};

int
start_ca();

void
stop_ca();

void
init_sub_info(struct sub_info *sub, const char *name, sub_callback func,
    void *usr);

int
subscribe_pv(struct sub_info *sub);

int
unsubscribe_pv(struct sub_info *sub);

#endif
