#ifndef TUI_H
#define TUI_H

extern int wmenu_h, wmenu_w;
extern int wflds_h, wflds_w;
extern int wmain_h, wmain_w;
extern int wstat_h, wstat_w;
extern int wcmd_h, wcmd_w;
extern int menu_h, menu_w;

enum cmd_code { C_NONE = 0, C_QUIT = 1 };
int start_tui(void);
int stop_tui(void);
int process_tui_events(void);
int create_tui_entry(const char *name);
int update_tui_entry(int entry_id, const char *value);

#endif
