/* now_playing.c
 *
 * contains all the code needed for the "now playing" screen
 */

#include "bnxc.h"

#ifdef NOW_PLAYING_SCREEN

#include <pthread.h>
#include <xmmsclient/xmmsclient-glib.h>
#include <glib.h>
#include <wchar.h>

static GMainLoop *ml;

static char *statusformat="${artist} - ${title}";

static int has_songname = 0;
static int fetching_songname = 0;
static int current_id = 0;
static int last_dur = 0;
static int curr_dur = 0;
static char songname[256];
static int curr_status = 0;

static const char *status_messages[] = {
	"Stopped",
	"Playing",
	"Paused"
};

wchar_t ramp[] = L" ▏▎▍▌▋▊▉█";

static void update_display();

static void *ml_thread();
static int handle_current_id(xmmsv_t *val, void *userdata);
static int handle_playtime(xmmsv_t *val, void *userdata);
static int handle_mediainfo_update(xmmsv_t *val, void *userdata);
static int handle_status_change(xmmsv_t *val, void *userdata);
static int do_mediainfo(xmmsv_t *val, void *userdata);

static int val_has_key(xmmsv_t *val, const char *key)
{
	return xmmsv_dict_entry_get_type(val, key) != XMMSV_TYPE_NONE;
}

void now_playing()
{
	pthread_t ml_thd;
	int ch;

	erase();

	/* START THE ASYNC CONNECTION */
	connection_async = xmmsc_init("bnxc_async");

	if(!connection_async) {
		fprintf(stderr, "terrible terrible things just happened\n");
		exit(EXIT_FAILURE);
	}

	if(!xmmsc_connect(connection_async, getenv("XMMS_PATH"))) {
		fprintf(stderr, "Connection to xmms2d failed: %s\n", xmmsc_get_last_error(connection_async));
		exit(EXIT_FAILURE);
	}
	/* DONE STARTING THE ASYNC CONNECTION */


	/* SET UP THE MAINLOOP */
	ml = g_main_loop_new(NULL, FALSE);


	XMMS_CALLBACK_SET(connection_async, xmmsc_broadcast_playback_current_id,
				handle_current_id, connection_async);
	XMMS_CALLBACK_SET(connection_async, xmmsc_playback_current_id,
				handle_current_id, connection_async);

	XMMS_CALLBACK_SET(connection_async, xmmsc_signal_playback_playtime,
				handle_playtime, NULL);

	XMMS_CALLBACK_SET(connection_async, xmmsc_broadcast_medialib_entry_changed,
				handle_mediainfo_update, connection_async);

	XMMS_CALLBACK_SET(connection_async, xmmsc_playback_status,
				handle_status_change, NULL);
	XMMS_CALLBACK_SET(connection_async, xmmsc_broadcast_playback_status,
				handle_status_change, NULL);


	xmmsc_mainloop_gmain_init(connection_async);
	/* DONE SETTING UP THE MAINLOOP */

	/* START THE GMAINLOOP THREAD */
	pthread_create(&ml_thd, NULL, ml_thread, NULL);


	while((ch = getch())) {
		switch(ch) {
			case 'h':
			case KEY_LEFT:
			case QUIT_BACK:
				pthread_cancel(ml_thd);
				pthread_join(ml_thd, NULL);
				return;
				break;
			case TOGGLE_PLAY_PAUSE:
				player_toggle();
				break;
			case NEXT:
				player_next();
				break;
			case PREVIOUS:
				player_previous();
				break;
		}
	}


	/* DESTROY THE ASYNC CONNECTION */
	xmmsc_unref(connection_async);
}

static void *ml_thread()
{
	/* START THE MAINLOOP */
	g_main_loop_run(ml);

	return NULL;
}

static int handle_playtime(xmmsv_t *val, void *userdata)
{
	int dur;

	if(xmmsv_get_error(val, &errbuf)) {
		fprintf(stderr, "%s", errbuf);
	}

	if(!xmmsv_get_int(val, &dur)) {
		printf("broken resultset");
		fflush(stdout);
	}

	if(((dur / 100) % 60) != ((last_dur / 100) % 60)) {
		last_dur = dur;
		if(!fetching_songname)
			update_display();
	}

	return TRUE;
}

static int handle_status_change(xmmsv_t *val, void *userdata)
{
	int new_status;

	if(xmmsv_get_error(val, &errbuf)) {
		fprintf(stderr, "%s", errbuf);
	}

	if(!xmmsv_get_int(val, &new_status)) {
		fprintf(stderr, "broken resultset");
	}

	curr_status = new_status;

	update_display();

	return TRUE;
}

static int handle_current_id(xmmsv_t *val, void *userdata)
{
	xmmsc_result_t *res;

	if(xmmsv_get_error(val, &errbuf)) {
		fprintf(stderr, "%s", errbuf);
	}

	if(!xmmsv_get_int(val, &current_id)) {
		fprintf(stderr, "broken resultset");
	}

	if(current_id) {
		fetching_songname = 1;
		res = xmmsc_medialib_get_info(connection_async, current_id);
		xmmsc_result_notifier_set(res, do_mediainfo, NULL);
		xmmsc_result_unref(res);
	}

	return TRUE;
}

static int handle_mediainfo_update(xmmsv_t *val, void *userdata)
{
	int id;
	xmmsc_result_t *res;

	if(xmmsv_get_error(val, &errbuf)) {
		fprintf(stderr, "%s", errbuf);
	}

	if(!xmmsv_get_int(val, &id)) {
		fprintf(stderr, "broken resultset");
	}

	if(id == current_id) {
		res = xmmsc_medialib_get_info(connection_async, current_id);
		xmmsc_result_notifier_set(res, do_mediainfo, NULL);
		xmmsc_result_unref(res);
	}

	return TRUE;
}

static int do_mediainfo(xmmsv_t *propdict, void *userdata)
{
	xmmsv_t *val;

	if(xmmsv_get_error(propdict, &errbuf)) {
		fprintf(stderr, "%s", errbuf);
	}

	val = xmmsv_propdict_to_dict(propdict, NULL);

	if(val_has_key(val, "channel") && val_has_key(val, "title")) {
		xmmsc_entry_format(songname, sizeof(songname), "[stream] ${title}", val);
		has_songname = 1;
	} else if(val_has_key(val, "channel")) {
		xmmsc_entry_format(songname, sizeof(songname), "${channel}", val);
		has_songname = 1;
	} else if(!val_has_key(val, "title")) {
		const char *url;

		if(xmmsv_dict_entry_get_string(val, "url", &url)) {
			char *filename = g_path_get_basename(url);

			if(filename) {
				snprintf(songname, sizeof(songname), "%s", filename);
				g_free(filename);
				has_songname = 1;
			}
		}
	} else {
		xmmsc_entry_format(songname, sizeof(songname), statusformat, val);
		has_songname = 1;
	}

	if(xmmsv_dict_entry_get_int(val, "duration", &curr_dur)) {
		curr_dur += 500;
	} else {
		curr_dur = 0;
	}

	xmmsv_unref(val);

	fetching_songname = FALSE;

	return FALSE;
}

static void update_display()
{
	int fullblocks, partial;
	int x, bar_len, bar_full_len;
	double progress;

	if(!has_songname)
		return;

	erase();

	mvprintw(0,COLS/2-5,"NOW PLAYING");
	mvprintw(2,COLS/2-strlen(songname)/2,"%s",songname);
	mvprintw(3,COLS/2-strlen(status_messages[curr_status])/2,"%s", status_messages[curr_status]);
	mvprintw(4,COLS/2-7,"%02d:%02d of %02d:%02d", last_dur/60000, (last_dur / 1000) % 60, curr_dur / 60000, (curr_dur / 1000) % 60);

	bar_len = ((double)COLS * .9);
	bar_full_len = bar_len*8;
	progress = ((double)last_dur) / ((double)curr_dur);
	if(progress > 1.0)
		progress = 1.0;

	fullblocks = (progress*bar_len);
	partial = (int)(progress*bar_full_len) % 8;

/*mvprintw(8,0,"bar size: %d", bar_len);
mvprintw(9,0,"bar full size: %d", bar_full_len);
mvprintw(10,0,"bar progress: %f", progress);
mvprintw(11,0,"fullblocks: %d", fullblocks);
mvprintw(12,0,"partial block: %d", partial);*/

	for(x=0; x<fullblocks; x++)
		mvprintw(6,COLS/2-bar_len/2 + x, "█");

	mvprintw(6,COLS/2-bar_len/2 + fullblocks, "%lc", ramp[partial]);

	for(x=0; x<bar_len; x++)
		mvchgat(6, COLS/2-bar_len/2 + x, 1, COLOR_PAIR(4), 4, NULL);

	refresh();
}


#endif
