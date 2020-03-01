menu: menu.c
	gcc -Wall -Wextra -pedantic -g menu.c -lmenu -lncurses -o menu

clean:
	rm -f menu
