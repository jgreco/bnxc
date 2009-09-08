/* db.c
 *
 * all database functions go in here.
 */

#include "bnxc.h"

#define HASHSIZE 50

static list hashtable[HASHSIZE];
static unsigned int hash(char *s);


int read_db(char *filename)
{
	char line[LINEMAX];
	char path[LINEMAX];
	FILE *in = fopen(filename, "r");

	if(in == NULL) {
		printf("could not open database.\n");
		return 0;
	}

	num_artists = 0;

	while(fgets(line, LINEMAX, in) != NULL)
	{
		artist curr_artist;
		album curr_album;

		/* consume blank lines */
		while(line[0] == '\n')
			fgets(line, LINEMAX, in);

		curr_artist = add_artist(strip_nl(line));

		fgets(line, LINEMAX, in);
		curr_album = add_album(curr_artist, strip_nl(line));

		while(fgets(line, LINEMAX, in) != NULL) {
			if(line[0] != '\n') {
				fgets(path, LINEMAX, in);
				add_song(curr_album, strip_nl(line), strip_nl(path));
			} else
				break;
		}
	}

	fclose(in);

	return 1;
}

/* destroys the database, freeing the memory.  still misses some right now but good enough for goverment work ;) */
void dump_db()
{
	int i;

	for(i=0; i<HASHSIZE; i++)
		if(hashtable[i] != NULL) {
			list np;

			for(np = hashtable[i]; np != NULL; np = np->next) {
				artist at;
				list n_alb;
				list temp;

				at = (artist)(np->contents);

				for(n_alb = at->albums; n_alb != NULL; n_alb = n_alb->next) {  /* for all the albums */
					album alb;
					track sng;
					track temp;

					alb = (album)(n_alb->contents);

					sng = alb->songs;

					while(sng != NULL) {  /* free all the songs */
						temp = sng;
						sng = sng->next;
						free(temp);
					}
				}

				n_alb = at->albums;

				while(n_alb != NULL) {
					temp = n_alb;
					n_alb = n_alb->next;
					free(temp->contents);
					free(temp);
				}
			}
		}

	for(i=0; i<HASHSIZE; i++) {
		if(hashtable[i] != NULL) {
			list temp;
			list np = hashtable[i];

			while(np != NULL) {
				temp = np;
				np = np->next;
				free(temp->contents);
				free(temp);
			}
		}
	}

	return;
}

static unsigned int hash(char *s)
{
	unsigned int hashval;

	for(hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;

	return hashval % HASHSIZE;
}

list lookup(char *s)
{
	list np;

	for(np = hashtable[hash(s)]; np != NULL; np = np->next)
		if(strcmp(s, np->name) == 0)
			return np; /* found */

	return NULL; /* not found */
}

artist add_artist(char *artist_name)
{
	unsigned int hashval;
	list np = lookup(artist_name);

	if(np == NULL) { /* not found */
		np = (list) malloc(sizeof(*np));
		np->contents = (artist) malloc(sizeof(struct st_artist));

		if(np == NULL || np->contents==NULL) /* create the artist */ {
			fprintf(stderr, "failled to allocate artist\n");
			exit(EXIT_FAILURE); /* return if crap broke */
		}

		strncpy(np->name,artist_name, STRN_SIZE*sizeof(char));

		hashval = hash(artist_name);
		np->next = hashtable[hashval];
		hashtable[hashval] = np;

		strncpy(((artist)(np->contents))->name,artist_name,STRN_SIZE*sizeof(char));
		((artist)(np->contents))->num_albums = 0;
		((artist)(np->contents))->albums = NULL;

		num_artists++;
	}

	return (artist)(np->contents);
}


album add_album(artist art, char *album_name)
{
	list new;
	list np;

	for(np = art->albums; np != NULL; np = np->next)
		if(strcmp(album_name, np->name) == 0)
			return (album) np->contents;

	new = (list) malloc(sizeof(struct nlist));
	new->contents = (album) malloc(sizeof(struct st_album));

	if(new == NULL || new->contents == NULL) {
		fprintf(stderr, "failed to allocate album\n");
		exit(EXIT_FAILURE); /* crap broke. */
	}

	strncpy(new->name, album_name, STRN_SIZE*sizeof(char));

	new->next = art->albums;
	art->albums = new;
	art->num_albums++;

	strncpy(((album)(new->contents))->name, album_name,STRN_SIZE*sizeof(char));
	((album)(new->contents))->songs = NULL;
	((album)(new->contents))->num_songs = 0;

	return (album)(new->contents);
}

void add_song(album alb, char *trackname, char *path)
{
	track np = alb->songs;
	alb->num_songs++;

	if(np == NULL) {
		alb->songs = (track) malloc(sizeof(struct songlist));
		strncpy(alb->songs->name,trackname, STRN_SIZE*sizeof(char));
		strncpy(alb->songs->path,path, STRN_SIZE*sizeof(char));
		alb->songs->next = NULL;
		return;
	} else
		for(np = alb->songs; np->next != NULL; np = np->next);

	np->next = (track) malloc(sizeof(struct songlist));
	strncpy(np->next->name, trackname, STRN_SIZE*sizeof(char));
	strncpy(np->next->path, path, STRN_SIZE*sizeof(char));
	np->next->next = NULL;

	return;
}

char** get_artist_list()
{
	int i;
	int x=0;

	char** str_list = malloc(sizeof(char*)*num_artists);

	for(i=0; i<HASHSIZE; i++)
		if(hashtable[i] != NULL) {
			list np;

			for(np = hashtable[i]; np != NULL; np = np->next) {
				str_list[x++] = np->name;
			}
		}

	qsort(str_list, num_artists, sizeof(char *), stringcmp);

	return str_list;
}

char** get_album_list(artist art)
{
	char** str_list = malloc(sizeof(char*)*art->num_albums);

	list np;

	int i=0;

	for(np = art->albums; np != NULL; np = np->next)
		str_list[i++] = np->name;

	return str_list;
}

char** get_track_list(album alb)
{
	char** str_list = malloc(sizeof(char*)*alb->num_songs);

	track np;

	int i=0;

	for(np = alb->songs; np != NULL; np = np->next)
		str_list[i++] = np->name;

	return str_list;
}
