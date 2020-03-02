tui: tui.c
	gcc -Wall -Wextra -pedantic -g tui.c -lmenu -lncurses -o tui

cahelper: cahelper.c
	gcc -Wall -Wextra -pedantic -g -L${EPICS_BASE}/lib/linux-x86_64 -lca \
	    -I${EPICS_BASE}/include/os/Linux -I${EPICS_BASE}/include \
	    -I${EPICS_BASE}/include/compiler/gcc -o $@ $<

clean:
	rm -f tui cahelper

