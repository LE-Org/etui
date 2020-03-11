#ifndef WIN_H
#define WIN_H
#include "win_registrar.h"

struct win {
	void (*recreate)(int h, int w, int y, int x);
	void (*draw)(void);
	void (*handle_key)(int c);
	void (*handle_passive)(void);
	void (*_refresh)(void);
	void (*release)(void);
	void (*on_data_changed)(void);
	void (*visible)(int state);
	int selected;
};

void windows_draw(void);
void windows_handle_key(int c);
void windows_handle_passive(void);
void windows_refresh(void);
void windows_release(void);
void windows_on_data_changed(void);
void windows_visible(int win_id, int status);
void windows_select(int win_id);

#endif
