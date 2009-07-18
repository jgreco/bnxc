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

struct songlist {
	char *name;
	struct songlist *next;
};

struct st_album {
	char *name;
	struct songlist *songs;
	int num_songs;
};

struct st_artist {
	char *name;
	struct nlist *albums;
	int num_albums;
};

struct nlist {
	struct nlist *next;
	void *contents;
	char *name;
};


/* FOUND IN utils.c */
char* rm_dash(char *str);
char *strip_nl();
int stringcmp(const void *a, const void *b);

/* FOUND IN db.c */
unsigned int num_artists;
int read_db(char *filename);
void dump_db();
struct nlist *lookup(char *s);
struct st_artist *add_artist(char *artist_name);
struct st_album *add_album(struct st_artist *artist, char *album_name);
void add_song(struct st_album *album, char *trackname);
char** get_artist_list();
char** get_album_list(struct st_artist *artist);
char** get_track_list(struct st_album *album);
#ifndef FRONTEND_NCURSES  /*only needed/wanted for the limited interface */
	void delete_album(struct st_artist *artist, char *album);
#endif



#ifdef FRONTEND_NCURSES
	void ninterface();
	int nmenu(char* list[], unsigned int list_len, char *title, char* sections[], unsigned int num_sections, unsigned int curr_section, char* commands, int* entered, int select);
#endif
#ifndef FRONTEND_NCURSES
	void min_print_usage();
	void min_list_reg();
	void min_list_del();
	void min_add_alb();
	int menu(char* list[], unsigned int list_len, char *title, char* commands, char* entered);
#endif

#ifdef XMMS2
	xmmsc_connection_t *connection;
	xmmsc_result_t *result;
	char *collection_path;

	void player_add_track(char *artist, char *album, char *track);
	void player_add_album(struct st_artist *artist, struct st_album *album);
	void player_add_artist(struct st_artist *artist);

	void player_clear_playlist();
	void player_play();
	void player_pause();
	void player_stop();
#endif
