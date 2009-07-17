/* frontend.c all of the code for the frontends
 *
 * messy crap resides here.
 */

#include "bnxc.h"
#include "config.h"

#ifdef FRONTEND_NCURSES
/* the menu logic for the ncurses interface, much simpler than it looks...
 */
void ninterface()
{
	char** list;

	char *sections[] = {"Artists", "Albums", "Tracks" };  /* Section titles, used in the menu */
	int entered;

	struct st_artist *artist;
	int selected1=0, selected2=0, selected3=0;  /*used to store the last selected item in each menu.
						      improves user experiance significantly */

	int art_choice, alb_choice, trak_choice;

	/* ---===[ ARTISTS MENU ]===--- */
	while(1)
	{
		list = get_artist_list();
		art_choice = nmenu(list, num_artists, "Artists:", sections, 3, 1, "aA", &entered, selected1);  /* draw artist menu */
		selected1 = art_choice;

		if(entered == 'q')/*|| entered == KEY_LEFT)*/ /* exit artist menu (and function) */
			return;

		else if(entered == 'a' || entered == 'A') /* play an artist (XMMS2) */
		{
			if(entered == 'A')
			{
				player_stop();
				player_clear_playlist();
			}

			artist = ((struct st_artist *)(lookup(list[art_choice])->contents));

			player_add_artist(artist);

			player_play();
		}

		/* ---===[ ALBUMS MENU ]===--- */
		if(entered == '\n')  /* enter this menu */
		{
			artist = ((struct st_artist *)(lookup(list[art_choice])->contents));
			while(1)
			{
				list = get_album_list(artist);
				alb_choice = nmenu(list, artist->num_albums, artist->name, sections, 3, 2, "aA", &entered, selected2);  /* draw album menu */
				selected2 = alb_choice;

				if(entered == KEY_LEFT || entered == 'q') /* exit albums menu */
				{
					free(list);
					selected2 = 0;
					break;
				}

				if(entered == 'a' || entered == 'A') /* play an album (XMMS2) */
				{
					struct st_album *album;
					struct nlist *np;

					for(np = artist->albums; np != NULL; np = np->next)
						if(strcmp(list[alb_choice], np->name) == 0)
							break;

					album = (struct st_album *) np->contents;

					if(entered == 'A')
					{
						player_stop();
						player_clear_playlist();
					}

					player_add_album(artist, album);

					player_play();
				}

				/* ---===[ TRACKS MENU ]===--- */
				if(entered == '\n') /* enter this menu */
				{
					struct st_album *album;
					struct nlist *np;

					for(np = artist->albums; np != NULL; np = np->next)
						if(strcmp(list[alb_choice], np->name) == 0)
							break;

					album = (struct st_album *) np->contents;
					while(1)
					{
						list = get_track_list(album);
						trak_choice = nmenu(list, album->num_songs, album->name, sections, 3, 3, "aA", &entered, selected3);  /* draw track menu */
						selected3 = trak_choice;

						/* play a track (XMMS2) */
						if(entered == 'a' ) /*|| entered == '\n')*/
							player_add_track(artist->name, album->name, list[trak_choice]);
						else if(entered == 'A')
						{
							player_stop();
							player_clear_playlist();
							player_add_track(artist->name, album->name, list[trak_choice]);
							player_play();
						}

						if(entered == KEY_LEFT || entered == 'q') /* exit tracks menu */
						{
							free(list);
							selected3 = 0;
							break;
						}
					} /* drop out of tracks here */
				}
			} /* drop out of albums here */
		 }
	}
}

/* menu drawing routine for the ncurses version, designed to be relatively re-usable.
 * breif explaination of arguments follows:
 *
 *        list of items,  number of items,        title,        sections (on sides),  num of sections,            current section displayed,  custom commands,  command entered,  default selection */
int nmenu(char* list[],   unsigned int list_len,  char *title,  char* sections[],     unsigned int num_sections,  unsigned int curr_section,  char* commands,   int *entered,     int select)
{
/*
	FILE *out = fopen("list_debug","a");
	int a;
	for(a=0; a<list_len; a++)
		fprintf(out, "%s%d\n", list[a], strlen(list[a]));


	for(a=0; a<list_len; a++)
		mvprintw(a,1 , "%s\n", list[a]);
	getch();

	fclose(out);
*/
	unsigned int height = LINES;  /* these are used to allow for future expansion (non-fullscreen menus) */
	unsigned int width  = COLS;
	unsigned int list_height;
	unsigned int begin=0, key=0;  /* FIXME, pointed default to non-zero but begin cannot */
	unsigned int pointed=select;

	unsigned int i;
	void *dumb = NULL;  /* needed for mvchgat, (argument reserved for future expansion...) */

	WINDOW *menu_win;

	if(height-1 <= list_len)
		list_height = height-1;
	else
		list_height = list_len;

	menu_win = newwin(height, width, 0, 0);  /* create the ncurses window */

	while(1)
	{
		/* =====DRAW THE MENU===== */
		werase(menu_win);

		mvwprintw(menu_win, 0, curr_section, "%s", title);
		mvwchgat(menu_win, 0, 0, COLS, COLOR_PAIR(3)|A_BOLD, 3, dumb);
		for(i=0; i < list_height; i++)
			mvwprintw(menu_win, 1+i, curr_section+1, "%s", list[i+begin]);

		for(i=curr_section; i<(unsigned)COLS; i++)
			mvwchgat(menu_win, pointed-begin+1, i, 1, COLOR_PAIR(1)|A_BOLD, 1, dumb);

		for(i=0; i<curr_section; i++)
		{
			unsigned int y = 0;
			int offset = (height / 2) - (strlen(sections[i]) / 2);

			for(y=0; y<strlen(sections[i]); y++)
				mvwprintw(menu_win, y+offset, i, "%c", sections[i][y]);
		}
		for(i=0; i<curr_section; i++)
		{
			int y;
			for(y=0; y<LINES; y++)
			{
				if(i == curr_section-1)
					mvwchgat(menu_win, y, i, 1, COLOR_PAIR(3)|A_BOLD, 3, dumb);
				else
					mvwchgat(menu_win, y, i, 1, COLOR_PAIR(2), 2, dumb);
			}
		}
		for(i=curr_section; i<num_sections; i++)
		{
			unsigned int y = 0;
			int offset = (height / 2) - (strlen(sections[i]) / 2);

			for(y=0; y<strlen(sections[i]); y++)
				mvwprintw(menu_win, y+offset, COLS-(num_sections-i), "%c", sections[i][y]);
		}
		for(i=curr_section; i<num_sections; i++)
		{
			int y;
			for(y=0; y<LINES; y++)
				mvwchgat(menu_win, y, COLS-(num_sections-i), 1, COLOR_PAIR(2), 2, dumb);
		}

		refresh();
		wrefresh(menu_win);
		/* =====DONE DRAWING MENU===== */

		/* =====HANDLE INPUT===== */
		key = getch();
		switch(key)
		{
			case 'q':
				*entered = 'q';  /* q *always* quits, not a custom command */
				return 0;
			case KEY_UP: case 'k':  /* move selection up */
				if(pointed > 0)
					pointed--;
				if(pointed < begin)
					begin--;
				break;
			case KEY_DOWN: case 'j':  /* move selection down */
				if(pointed < list_len-1)
				{
					pointed++;

					if(pointed > begin+height-2 && begin+height-1 < list_len)
						begin++;
				}
				break;
			case '\n': case KEY_RIGHT: case 'l':  /* select/chose something */
				*entered = '\n';
				return pointed;
				break;
			case KEY_LEFT: case 'h':  /* go back */
				*entered = KEY_LEFT;
				return pointed;
				break;
		}

		/* see if we hit a custom command */
		if(strchr(commands, key) != NULL)
		{
			*entered = key;
			return pointed;
		}
	}
}
#endif

