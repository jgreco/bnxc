/* db.c
 *
 * all database functions go in here.
 */

#include "bnxc.h"

#define HASHSIZE 50

static struct nlist *hashtable[HASHSIZE];
static unsigned int hash(char *s);


int read_db(char *filename)
{
	char line[LINEMAX];
	FILE *in = fopen(filename, "r");
	if(in == NULL)
	{
		printf("could not open database.\n");
		return 0;
	}

	num_artists = 0;

	while(fgets(line, LINEMAX, in) != NULL)
	{
		struct st_artist *curr_artist;
		struct st_album *curr_album;

		/* consume blank lines */
		while(line[0] == '\n')
			fgets(line, LINEMAX, in);

		curr_artist = add_artist(strip_nl(line));

		fgets(line, LINEMAX, in);
		curr_album = add_album(curr_artist, strip_nl(line));

		while(fgets(line, LINEMAX, in) != NULL)
		{
			if(line[0] != '\n')
				add_song(curr_album, strip_nl(line));
			else
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
		if(hashtable[i] != NULL)
		{
			struct nlist *np;

			for(np = hashtable[i]; np != NULL; np = np->next)
			{
				struct st_artist *at;
				struct nlist *alb;
				struct nlist *temp;

				at = (struct st_artist *)(np->contents);

				for(alb = at->albums; alb != NULL; alb = alb->next)  /* for all the albums */
				{
					struct st_album *album;
					struct songlist *sng;
					struct songlist *temp;

					album = (struct st_album *)(alb->contents);

					sng = album->songs;
					while(sng != NULL)  /* free all the songs */
					{
						temp = sng;
						sng = sng->next;
						free(temp);
					}
				}

				alb = at->albums;
				while(alb != NULL)
				{
					temp = alb;
					alb = alb->next;
					free(temp->contents);
					free(temp);
				}
			}
		}

	for(i=0; i<HASHSIZE; i++)
	{
		if(hashtable[i] != NULL)
		{
			struct nlist *temp;
			struct nlist *np = hashtable[i];

			while(np != NULL)
			{
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

struct nlist *lookup(char *s)
{
	struct nlist *np;

	for(np = hashtable[hash(s)]; np != NULL; np = np->next)
		if(strcmp(s, np->name) == 0)
			return np; /* found */
	return NULL; /* not found */
}

struct st_artist *add_artist(char *artist_name)
{
	unsigned int hashval;
	struct nlist *np = lookup(artist_name);

	if(np == NULL) { /* not found */
		np = (struct nlist *) malloc(sizeof(*np));
		np->contents = (struct st_artist *) malloc(sizeof(struct st_artist));

		if(np == NULL || np->contents==NULL) /* create the artist */
		{
			fprintf(stderr, "failled to allocate artist\n");
			exit(EXIT_FAILURE); /* return if crap broke */
		}
		strncpy(np->name,artist_name, STRN_SIZE*sizeof(char));

		hashval = hash(artist_name);
		np->next = hashtable[hashval];
		hashtable[hashval] = np;

		strncpy(((struct st_artist *)(np->contents))->name,artist_name,STRN_SIZE*sizeof(char));
		((struct st_artist *)(np->contents))->num_albums = 0;
		((struct st_artist *)(np->contents))->albums = NULL;

		num_artists++;
	}

	return (struct st_artist *)(np->contents);
}


struct st_album *add_album(struct st_artist *artist, char *album_name)
{
	struct nlist *new;
	struct nlist *np;

	for(np = artist->albums; np != NULL; np = np->next)
		if(strcmp(album_name, np->name) == 0)
			return (struct st_album *) np->contents;

	new = (struct nlist *) malloc(sizeof(struct nlist));
	new->contents = (struct st_album *) malloc(sizeof(struct st_album));
	if(new == NULL || new->contents == NULL)
	{
		fprintf(stderr, "failed to allocate album\n");
		exit(EXIT_FAILURE); /* crap broke. */
	}
	strncpy(new->name, album_name, STRN_SIZE*sizeof(char));

	new->next = artist->albums;
	artist->albums = new;
	artist->num_albums++;

	strncpy(((struct st_album *)(new->contents))->name, album_name,STRN_SIZE*sizeof(char));
	((struct st_album *)(new->contents))->songs = NULL;
	((struct st_album *)(new->contents))->num_songs = 0;

	return (struct st_album *)(new->contents);
}

void add_song(struct st_album *album, char *trackname)
{
	struct songlist *np = album->songs;
	album->num_songs++;

	if(np == NULL)
	{
		album->songs = (struct songlist *) malloc(sizeof(struct songlist));
		strncpy(album->songs->name,trackname, STRN_SIZE*sizeof(char));
		album->songs->next = NULL;
		return;
	}
	else
		for(np = album->songs; np->next != NULL; np = np->next);

	np->next = (struct songlist *) malloc(sizeof(struct songlist));
	strncpy(np->next->name, trackname, STRN_SIZE*sizeof(char));
	np->next->next = NULL;

	return;
}

char** get_artist_list()
{
	int i;
	int x=0;

	char** list = malloc(sizeof(char*)*num_artists);

	for(i=0; i<HASHSIZE; i++)
		if(hashtable[i] != NULL)
		{
			struct nlist *np;

			for(np = hashtable[i]; np != NULL; np = np->next)
			{
				list[x++] = np->name;
			}
		}

	qsort(list, num_artists, sizeof(char *), stringcmp);

	return list;
}

char** get_album_list(struct st_artist *artist)
{
	char** list = malloc(sizeof(char*)*artist->num_albums);

	struct nlist *np;

	int i=0;
	for(np = artist->albums; np != NULL; np = np->next)
		list[i++] = np->name;

	return list;
}

char** get_track_list(struct st_album *album)
{
	char** list = malloc(sizeof(char*)*album->num_songs);

	struct songlist *np;

	int i=0;
	for(np = album->songs; np != NULL; np = np->next)
		list[i++] = np->name;

	return list;
}
