default: nccamon

nccamon: utils.c tui.c cahelper.c main.c
	gcc -L${EPICS_BASE}/lib/linux-x86_64 -lca \
	    -I${EPICS_BASE}/include/os/Linux -I${EPICS_BASE}/include \
	    -I${EPICS_BASE}/include/compiler/gcc \
	    -lmenu -lncurses -Wall -Wextra -pedantic -g -o $@ $^

caexample: cahelper.c caexample.c
	gcc -L${EPICS_BASE}/lib/linux-x86_64 -lca \
	    -I${EPICS_BASE}/include/os/Linux -I${EPICS_BASE}/include \
	    -I${EPICS_BASE}/include/compiler/gcc \
	    -Wall -Wextra -pedantic -g -o $@ $^

clean:
	rm -f nccamon

