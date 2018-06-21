#include <curses.h>
#include <menu.h>
#include <cstdlib>
#include <cstring>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *Tasks[] = {
                        "Task 1", "Task 2", "Task 3", "Task 4", "Task 5",
			"Task 6", "Task 7", "Task 8", "Task 9", "Task 10",
			"Task 11", "Task 12", "Task 13", "Task 14", "Task 15",
			"Task 16", "Task 17", "Task 18", "Task 19", "Task 20",
                        "Exit",
                        (char *)NULL,
                  };//redefine

int main()
{	ITEM **my_items;
	int c;				
	MENU *my_menu;
        WINDOW *my_menu_win;
        int n_Tasks, i;
	
	/* Initialize curses */
	initscr();
	start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);

	/* Create items */
        n_Tasks = ARRAY_SIZE(Tasks);
        my_items = (ITEM **)calloc(n_Tasks, sizeof(ITEM *));
        for(i = 0; i < n_Tasks; ++i)
                my_items[i] = new_item(Tasks[i], Tasks[i]);

	/* Crate menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Set menu option not to show the description */
	menu_opts_off(my_menu, O_SHOWDESC);

	/* Create the window to be associated with the menu */
        my_menu_win = newwin(10, 70, 4, 4);
        keypad(my_menu_win, TRUE);
     
	/* Set main window and sub window */
        set_menu_win(my_menu, my_menu_win);
        set_menu_sub(my_menu, derwin(my_menu_win, 6, 68, 3, 1));
	set_menu_format(my_menu, 5, 3);
	set_menu_mark(my_menu, " * ");

	/* Print a border around the main window and print a title */
        box(my_menu_win, 0, 0);
	
	attron(COLOR_PAIR(2));
	mvprintw(LINES - 3, 0, "Use PageUp and PageDown to scroll");
	mvprintw(LINES - 2, 0, "Use Arrow Keys to navigate (F1 to Exit)");
	attroff(COLOR_PAIR(2));
	refresh();

	/* Post the menu */
	post_menu(my_menu);
	wrefresh(my_menu_win);
	
	while((c = wgetch(my_menu_win)) != KEY_F(1))
	{       switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case KEY_LEFT:
				menu_driver(my_menu, REQ_LEFT_ITEM);
				break;
			case KEY_RIGHT:
				menu_driver(my_menu, REQ_RIGHT_ITEM);
				break;
			case KEY_NPAGE:
				menu_driver(my_menu, REQ_SCR_DPAGE);
				break;
			case KEY_PPAGE:
				menu_driver(my_menu, REQ_SCR_UPAGE);
				break;
		}
                wrefresh(my_menu_win);
	}	

	/* Unpost and free all the memory taken up */
        unpost_menu(my_menu);
        free_menu(my_menu);
        for(i = 0; i < n_Tasks; ++i)
                free_item(my_items[i]);
	endwin();
}
