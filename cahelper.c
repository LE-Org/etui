#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cahelper.h"

static void
event_callback(struct event_handler_args ev_args)
{
	struct sub_info *sub = ev_args.usr;
	sub->dbr_type = ev_args.type;
	sub->value = (void *) ev_args.dbr;
	sub->count = ev_args.count;
	sub->callback(sub);
}

static void
connection_handler(struct connection_handler_args con_args)
{
	struct sub_info *sub = (struct sub_info *) ca_puser(
	    con_args.chid);
	if (con_args.op == CA_OP_CONN_UP && !sub->initialized) {
		sub->initialized = 1;
		sub->dbf_type = ca_field_type(sub->chid);
		sub->dbr_type = dbf_type_to_DBR_GR(sub->dbf_type);
		sub->eca_err = ca_create_subscription(sub->dbr_type, 0,
		    sub->chid, CA_EVENT_MASK, event_callback, sub,
		    &sub->event_id);
		assert(sub->eca_err == ECA_NORMAL);
	} else if (con_args.op == CA_OP_CONN_DOWN) {
		sub->eca_err = ECA_DISCONN;
	}
}

void
init_sub_info(struct sub_info *sub, const char *name, sub_callback func,
    void *usr)
{
	memset(sub, 0, sizeof(*sub));
	strncpy(sub->name, name, MAX_PV_NAME_SIZE);
	sub->eca_err = ECA_DISCONN;
	sub->callback = func;
	sub->usr = usr;
}

int
subscribe_pv(struct sub_info *sub)
{
	int ca_rc;
	ca_rc = ca_create_channel(sub->name, connection_handler, sub,
	    CA_PRIORITY, &sub->chid);
	assert(ca_rc == ECA_NORMAL);
	/* ca_pend_event(CA_TIMEOUT); */
	return 0;
}

int
unsubscribe_pv(struct sub_info *sub)
{
	int ca_rc;
	// clearing channel also reclaims subscriptions
	ca_rc = ca_clear_channel(sub->chid);
	assert(ca_rc == ECA_NORMAL);
	return ca_rc;
}

int
start_ca()
{
	return ca_context_create(ca_disable_preemptive_callback);
}

void
stop_ca()
{
	ca_context_destroy();
}

// Example of using helper functions

void
callback(struct sub_info *sub)
{
	if (sub->dbr_type == DBR_GR_DOUBLE) {
		struct dbr_gr_double *pv = (struct dbr_gr_double *) sub->value;
		printf("Got something type: %ld value: %f\n", sub->dbr_type,
		    pv->value);
	}
}

int
main()
{
	struct sub_info sub;
	start_ca();
	init_sub_info(&sub, "user:ai1", callback, NULL);
	subscribe_pv(&sub);
	ca_pend_event(0);
	unsubscribe_pv(&sub);
	stop_ca();
	return 0;
}
