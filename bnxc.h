/* bnxc.h
 *
 * main header file for bnxc.h
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#include "config.h"

#ifdef FRONTEND_NCURSES
	#include <ncurses.h>
#endif
#ifdef XMMS2
	#include <xmmsclient/xmmsclient.h>
#endif

#define LINEMAX 1024
#define STRN_SIZE 256

typedef struct songlist {
	char name[STRN_SIZE];
	char path[LINEMAX];
	struct songlist *next;
} *track;

typedef struct st_album {
	char name[STRN_SIZE];
	struct songlist *songs;
	int num_songs;
} *album;

typedef struct st_artist {
	char name[STRN_SIZE];
	struct nlist *albums;
	int num_albums;
} *artist;

typedef struct nlist {
	struct nlist *next;
	void *contents;
	char name[STRN_SIZE];
} *list;

#ifdef FRONTEND_NCURSES
typedef struct menu_parameters_str {
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
#endif

/* FOUND IN utils.c */
char *strip_nl();
int stringcmp(const void *a, const void *b);
void destroy_menu_params(menu_parameters params);

/* FOUND IN db.c */
unsigned int num_artists;
int read_db(char *filename);
void dump_db();

list lookup(char *s);

artist add_artist(char *artist_name);
album add_album(artist art, char *album_name);
void add_song(album alb, char *trackname, char *path);

char** get_artist_list();
char** get_album_list(artist art);
char** get_track_list(album alb);


#ifdef FRONTEND_NCURSES
	void ninterface();
	int nmenu(menu_parameters params);
#endif
#ifndef FRONTEND_NCURSES

#endif

#ifdef XMMS2
	xmmsc_connection_t *connection;
	xmmsc_result_t *result;
	xmmsv_t *value;
	const char *errbuf;

	char *collection_path;

	void player_add_track(char *artist, char *album, char *song);
	void player_add_album(artist art, album alb);
	void player_add_artist(artist art);

	void player_clear_playlist();
	void player_play();
	void player_pause();
	void player_stop();
#endif
