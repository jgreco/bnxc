/* frontend.c all of the code for the frontends
 *
 * messy crap resides here.
 */

#include "bnxc.h"
#include "config.h"


#ifdef FRONTEND_NCURSES

static int nmenu(menu_parameters params);  /* menu display and input system */
static void destroy_menu_params(menu_parameters params);

static typedef struct menu_parameters_str {
	char** list;
	unsigned int list_len;

	char *title;

	char** sections;
	unsigned int num_sections;
	unsigned int curr_section;

	char *commands;
	unsigned int entered;
	unsigned int select;
	unsigned int scroll;
	unsigned int height;
	unsigned int width;
} *menu_parameters;


/* the menu logic for the ncurses interface, much simpler than it looks...
 */
void ninterface()
{
	menu_parameters artists_menu, albums_menu, tracks_menu;

	char *sections[] = {"Artists", "Albums", "Tracks" };  /* Section titles, used in the menu */

	artist art;
	album alb;
	track song;
	list np;

	int art_choice, alb_choice, trak_choice;

	/* ---===[ ARTISTS MENU ]===--- */
	artists_menu = (menu_parameters)malloc(sizeof(struct menu_parameters_str));
	artists_menu->list = get_artist_list();
	artists_menu->list_len = num_artists;
	artists_menu->title = "Artists:";
	artists_menu->sections = sections;
	artists_menu->num_sections = 3;
	artists_menu->curr_section = 1;
	artists_menu->commands = COMMANDS;
	artists_menu->select = 0;
	artists_menu->scroll = 0;
	artists_menu->height = LINES;
	artists_menu->width = COLS;

	while(1) {
		art_choice = nmenu(artists_menu);  /* draw artist menu */
		artists_menu->select = art_choice;

		if(artists_menu->entered == QUIT_BACK)  /* exit artist menu (and function) */
			break;
		else if(artists_menu->entered == APPEND_PLAYLIST || artists_menu->entered == REPLACE_PLAYLIST) {  /* play an artist (XMMS2) */
			if(artists_menu->entered == REPLACE_PLAYLIST) {
				player_stop();
				player_clear_playlist();
			}

			art = ((artist)(lookup(artists_menu->list[art_choice])->contents));

			player_add_artist(art);

			player_play();
		} else if(artists_menu->entered == TOGGLE_PLAY_PAUSE)
			player_toggle();
		else if(artists_menu->entered == NOWPLAYING) {
			now_playing();
		}

		/* ---===[ ALBUMS MENU ]===--- */
		else if(artists_menu->entered == '\n') { /* enter this menu */
			art = ((artist)(lookup(artists_menu->list[art_choice])->contents));

			albums_menu = (menu_parameters)malloc(sizeof(struct menu_parameters_str));
			albums_menu->list = get_album_list(art);
			albums_menu->list_len = art->num_albums;
			albums_menu->title = art->name;
			albums_menu->sections = sections;
			albums_menu->num_sections = 3;
			albums_menu->curr_section = 2;
			albums_menu->commands = COMMANDS;
			albums_menu->select = 0;
			albums_menu->scroll = 0;
			albums_menu->height = LINES;
			albums_menu->width = COLS;

			while(1) {
				alb_choice = nmenu(albums_menu);  /* draw album menu */

				if(albums_menu->entered == KEY_LEFT || albums_menu->entered == QUIT_BACK) /* exit albums menu */
					break;
				else if(albums_menu->entered == APPEND_PLAYLIST || albums_menu->entered == REPLACE_PLAYLIST)  {  /* play an album (XMMS2) */
					if(albums_menu->entered == REPLACE_PLAYLIST) {
						player_stop();
						player_clear_playlist();
					}


					for(np = art->albums; np != NULL; np = np->next)
						if(strcmp(albums_menu->list[alb_choice], np->name) == 0)
							break;

					alb = (album)np->contents;


					player_add_album(alb);
					player_play();
				} else if(albums_menu->entered == TOGGLE_PLAY_PAUSE)
					player_toggle();
				else if(albums_menu->entered == NOWPLAYING) {
					now_playing();
				}

				/* ---===[ TRACKS MENU ]===--- */
				else if(albums_menu->entered == '\n') { /* enter this menu */
					for(np = art->albums; np != NULL; np = np->next)
						if(strcmp(albums_menu->list[alb_choice], np->name) == 0)
							break;

					alb = (album)np->contents;

					tracks_menu = (menu_parameters)malloc(sizeof(struct menu_parameters_str));
					tracks_menu->list = get_track_list(alb);
					tracks_menu->list_len = alb->num_songs;
					tracks_menu->title = alb->name;
					tracks_menu->sections = sections;
					tracks_menu->num_sections = 3;
					tracks_menu->curr_section = 3;
					tracks_menu->commands = COMMANDS;
					tracks_menu->select = 0;
					tracks_menu->scroll = 0;
					tracks_menu->height = LINES;
					tracks_menu->width = COLS;

					while(1) {
						trak_choice = nmenu(tracks_menu);  /* draw track menu */

						/* play a track (XMMS2) */
						if(tracks_menu->entered == APPEND_PLAYLIST || tracks_menu->entered == REPLACE_PLAYLIST) {  /* play a track (XMMS2) */
							if(tracks_menu->entered == REPLACE_PLAYLIST) {
								player_stop();
								player_clear_playlist();
							}

							for(song = alb->songs; song != NULL; song = song->next)
								if(strcmp(tracks_menu->list[trak_choice], song->name) == 0)
									break;

							player_add_track(song);
							player_play();
						} else if(tracks_menu->entered == TOGGLE_PLAY_PAUSE) {
							player_toggle();
						} else if(tracks_menu->entered == NOWPLAYING) {
							now_playing();
						} else if(tracks_menu->entered == KEY_LEFT || tracks_menu->entered == QUIT_BACK) /* exit tracks menu */
							break;
					} /* drop out of tracks here */

					destroy_menu_params(tracks_menu);
				}
			} /* drop out of albums here */

			destroy_menu_params(albums_menu);
		 }
	} /* drop out of artists here */

	destroy_menu_params(artists_menu);

	return;
}


/* menu drawing routine for the ncurses version, designed to be relatively re-usable.
 * breif explaination of arguments follows:
*/
static int nmenu(menu_parameters params)
{
	unsigned int list_height;
	unsigned int key=0;

	unsigned int i;
	void *dumb = NULL;  /* needed for mvchgat, (argument reserved for future expansion...) */

	WINDOW *menu_win;

	params->height = LINES;
	params->width = COLS;

	if(params->height-1 <= params->list_len)
		list_height = params->height-1;
	else
		list_height = params->list_len;

	menu_win = newwin(params->height, params->width, 0, 0);  /* create the ncurses window */

	while(1) {
		/* =====DRAW THE MENU===== */
		werase(menu_win);

		mvwprintw(menu_win, 0, params->curr_section, "%s", params->title);
		mvwchgat(menu_win, 0, 0, COLS, COLOR_PAIR(3)|A_BOLD, 3, dumb);
		for(i=0; i < list_height; i++)
			mvwprintw(menu_win, 1+i, params->curr_section+1, "%s", params->list[i+params->scroll]);

		for(i=params->curr_section; i<(unsigned)COLS; i++)
			mvwchgat(menu_win, params->select-params->scroll+1, i, 1, COLOR_PAIR(1)|A_BOLD, 1, dumb);

		for(i=0; i<params->curr_section; i++) {
			unsigned int y = 0;
			int offset = (params->height / 2) - (strlen(params->sections[i]) / 2);


			for(y=0; y<strlen(params->sections[i]); y++)
				mvwprintw(menu_win, y+offset, i, "%c", params->sections[i][y]);
		}

		for(i=0; i<params->curr_section; i++) {
			int y;
			for(y=0; y<LINES; y++) {
				if(i == params->curr_section-1)
					mvwchgat(menu_win, y, i, 1, COLOR_PAIR(3)|A_BOLD, 3, dumb);
				else
					mvwchgat(menu_win, y, i, 1, COLOR_PAIR(2), 2, dumb);
			}
		}

		for(i=params->curr_section; i<params->num_sections; i++) {
			unsigned int y = 0;
			int offset = (params->height / 2) - (strlen(params->sections[i]) / 2);

			for(y=0; y<strlen(params->sections[i]); y++)
				mvwprintw(menu_win, y+offset, COLS-(params->num_sections-i), "%c", params->sections[i][y]);
		}

		for(i=params->curr_section; i<params->num_sections; i++) {
			int y;
			for(y=0; y<LINES; y++)
				mvwchgat(menu_win, y, COLS-(params->num_sections-i), 1, COLOR_PAIR(2), 2, dumb);
		}

		refresh();
		wrefresh(menu_win);
		/* =====DONE DRAWING MENU===== */

		/* =====HANDLE INPUT===== */
		key = getch();
		switch(key) {
			case QUIT_BACK:
				params->entered = QUIT_BACK;  /* q *always* quits, not a custom command */
				return 0;
			case KEY_UP: case 'k':  /* move selection up */
				if(params->select > 0)
					params->select--;
				if(params->select < params->scroll)
					params->scroll--;
				break;
			case KEY_DOWN: case 'j':  /* move selection down */
				if(params->select < params->list_len-1) {
					params->select++;

					if(params->select > params->scroll+params->height-2 && params->scroll+params->height-1 < params->list_len)
						params->scroll++;
				}
				break;
			case '\n': case KEY_RIGHT: case 'l':  /* select/chose something */
				params->entered = '\n';
				return params->select;
				break;
			case KEY_LEFT: case 'h':  /* go back */
				params->entered = KEY_LEFT;
				return params->select;
				break;
		}

		/* see if we hit a custom command */
		if(strchr(params->commands, key) != NULL) {
			params->entered = key;
			return params->select;
		}
	}
}

static void destroy_menu_params(menu_parameters params)
{
	free(params->list);
	free(params);
}
#endif
