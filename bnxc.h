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

/* data structures for database here. */

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

/* FOUND IN utils.c */
	char *strip_nl();
	int stringcmp(const void *a, const void *b);

/* FOUND IN db.c */
	unsigned int num_artists;  /*count of how many artists are in the database */
	int read_db(char *filename);  /*read in the database from a file */
	void dump_db();  /* free up the database */

	list lookup(char *s);  /* look up key (string) in hashtable */

	artist add_artist(char *artist_name);  /* add artist to db */
	album add_album(artist art, char *album_name);  /* add album to artist in db */
	void add_song(album alb, char *trackname, char *path);  /* add track to album in db */

	char** get_artist_list();  /* get a list (array of strings) of all the artists) */
	char** get_album_list(artist art);  /* get a list (array of strings) of all the albums of an artist */
	char** get_track_list(album alb);  /* get a list (array of strings) of all the tracks in an album */

/* FOUND IN frontend.c */
#ifdef FRONTEND_NCURSES
	void ninterface();  /* menu driver */
#endif

/* FOUND IN now_playing.c */
#ifdef NOW_PLAYING_SCREEN
	void now_playing();
#endif

/* FOUND IN player.c */
#ifdef XMMS2
	xmmsc_connection_t *connection;
	xmmsc_connection_t *connection_async;

	xmmsc_result_t *result;
	xmmsv_t *value;
	const char *errbuf;

	char *collection_path;

	void player_add_track(track song);
	void player_add_album(album alb);
	void player_add_artist(artist art);

	void player_clear_playlist();
	void player_play();
	void player_pause();
	void player_toggle();
	void player_stop();
#endif
