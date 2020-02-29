menu: menu.c
	gcc -Wall -Wextra -g menu.c -lmenu -lncurses -o menu

clean:
	rm -f menu
