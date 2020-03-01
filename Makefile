tui: tui.c
	gcc -Wall -Wextra -pedantic -g tui.c -lmenu -lncurses -o tui

clean:
	rm -f tui
