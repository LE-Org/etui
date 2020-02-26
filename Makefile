menu: menu.c
	gcc menu.c -lmenu -lncurses -o menu

clean:
	rm -f menu
