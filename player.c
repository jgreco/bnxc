/* player.c
 *
 * all XMMS2 client code in here.
 */

#include "bnxc.h"
#include "config.h"

#ifdef XMMS2
void player_add_track(char *artist, char *album, char *track)
{
	char filename[LINEMAX];

	snprintf(filename, LINEMAX, "file://%s/%s/%s/%s",collection_path, artist, album, track);

	result = xmmsc_playlist_add_url(connection, NULL, filename);
	xmmsc_result_wait(result);

	return;
}

void player_add_album(struct st_artist *artist, struct st_album *album)
{
	int i;
	char **tracks = get_track_list(album);

	for(i=0; i<album->num_songs; i++)
		player_add_track(artist->name, album->name, tracks[i]);

	return;
}

void player_add_artist(struct st_artist *artist)
{
	struct nlist *np;

	for(np = artist->albums; np != NULL; np = np->next)
		player_add_album(artist, (struct st_album *)(np->contents));

	return;
}

void player_clear_playlist()
{
	result = xmmsc_playlist_clear(connection, NULL);
	xmmsc_result_wait(result);

	value = xmmsc_result_get_value(result);

	if(xmmsv_get_error(value, &errbuf))
		fprintf(stderr, "%s", errbuf);

	return;
}

void player_play()
{
	result = xmmsc_playback_start(connection);
	xmmsc_result_wait(result);

	value = xmmsc_result_get_value(result);

	if(xmmsv_get_error(value, &errbuf))
		fprintf(stderr, "%s", errbuf);

	return;
}

void player_pause()
{
	result = xmmsc_playback_pause(connection);
	xmmsc_result_wait(result);

	value = xmmsc_result_get_value(result);

	if(xmmsv_get_error(value, &errbuf))
		fprintf(stderr, "%s", errbuf);

	return;
}

void player_stop()
{
	result = xmmsc_playback_stop(connection);
	xmmsc_result_wait(result);

	value = xmmsc_result_get_value(result);

	if(xmmsv_get_error(value, &errbuf))
		fprintf(stderr, "%s", errbuf);

	return;
}

#endif
