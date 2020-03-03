#ifndef TUI_H
#define TUI_H

#define MAX_GPV_NAME_SIZE 41
#define MAX_GPV_VALUE_SIZE 41

struct graphical_pv {
  char name[MAX_GPV_NAME_SIZE];
  char value[MAX_GPV_VALUE_SIZE];
};

int
start_tui();

int
stop_tui();

void
process_tui_events();

int
create_tui_entry(const char *name);

int
update_tui_entry(int entry_id, const char *value);

#endif
