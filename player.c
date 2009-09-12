/* player.c
 *
 * all XMMS2 client code in here.
 */

#include "bnxc.h"
#include "config.h"

#ifdef XMMS2
void player_add_track(track song)
{
	char filename[LINEMAX];

	snprintf(filename, LINEMAX, "file://%s/%s",collection_path, song->path);

	result = xmmsc_playlist_add_url(connection, NULL, filename);
	xmmsc_result_wait(result);

	return;
}

void player_add_album(album alb)
{
	track np;

	for(np = alb->songs; np != NULL; np = np->next)
		player_add_track(np);

	return;
}

void player_add_artist(artist art)
{
	list np;

	for(np = art->albums; np != NULL; np = np->next)
		player_add_album((album)(np->contents));

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

void player_toggle()
{
	int32_t status;

	result = xmmsc_playback_status(connection);
	xmmsc_result_wait(result);

	value = xmmsc_result_get_value(result);

	if(xmmsv_get_error(value, &errbuf))
		fprintf(stderr, "%s", errbuf);

	if(!xmmsv_get_int(value, &status))
		fprintf(stderr, "something broke");

	if(status == XMMS_PLAYBACK_STATUS_PLAY)
		player_pause();
	else
		player_play();

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
