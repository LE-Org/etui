#ifndef TUI_H
#define TUI_H

#define MAX_GPV_NAME_SIZE 41
#define MAX_GPV_VALUE_SIZE 41

enum cmd_code { C_NONE = 0, C_QUIT = 1 };

struct graphical_pv {
  char name[MAX_GPV_NAME_SIZE];
  char value[MAX_GPV_VALUE_SIZE];
};

int start_tui(void);
int stop_tui(void);
int process_tui_events(void);
int create_tui_entry(const char *name);
int update_tui_entry(int entry_id, const char *value);

#endif
