#ifndef TUI_H
#define TUI_H

enum cmd_code { C_NONE = 0, C_QUIT = 1 };
int start_tui(void);
int stop_tui(void);
int process_tui_events(void);
int create_tui_entry(const char *name);
int update_tui_entry(int entry_id, const char *value);

#endif
