#include "win.h"


void
windows_draw(void)
{
	int i;
	for (i=0; i < n_windows; i++) {
		if (windows[i] && windows[i]->draw)
			windows[i]->draw();
	}
}

void
windows_refresh(void)
{
	int i;
	for (i=0; i < n_windows; i++) {
		if (windows[i] && windows[i]->_refresh)
			windows[i]->_refresh();
	}
}

void
windows_release(void)
{
	int i;
	for (i=0; i < n_windows; i++) {
		if (windows[i] && windows[i]->release)
			windows[i]->release();
	}
}

void
windows_select(int win_id)
{
	int i;
	for (i=0; i < n_windows; i++)
		windows[i]->selected = 0;
	windows[win_id]->selected = 1;
}

void
windows_visible(int win_id, int status)
{
	if (windows[win_id]->visible)
		windows[win_id]->visible(status);
}

void
windows_on_data_changed(void)
{
	int i;
	for (i=0; i < n_windows; i++) {
		if (windows[i] && windows[i]->on_data_changed)
			windows[i]->on_data_changed();
	}
}

void
windows_handle_key(int c)
{
	int i;
	for (i=0; i < n_windows; i++) {
		if (windows[i] && windows[i]->handle_key &&
		    windows[i]->selected)
			windows[i]->handle_key(c);
	}
}
