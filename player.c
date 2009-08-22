/* player.c
 *
 * all XMMS2 client code in here.
 */

#include "bnxc.h"
#include "config.h"

#ifdef XMMS2
void player_add_track(char *art, char *alb, char *song)
{
	char filename[LINEMAX];

	snprintf(filename, LINEMAX, "file://%s/%s/%s/%s",collection_path, art, alb, song);

	result = xmmsc_playlist_add_url(connection, NULL, filename);
	xmmsc_result_wait(result);

	return;
}

void player_add_album(artist art, album alb)
{
	int i;
	char **tracks = get_track_list(alb);

	for(i=0; i<alb->num_songs; i++)
		player_add_track(art->name, alb->name, tracks[i]);

	return;
}

void player_add_artist(artist art)
{
	list np;

	for(np = art->albums; np != NULL; np = np->next)
		player_add_album(art, (album)(np->contents));

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
