menu: menu.c
	gcc -g menu.c -lmenu -lncurses -o menu

clean:
	rm -f menu
