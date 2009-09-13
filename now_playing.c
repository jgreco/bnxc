/* now_playing.c
 *
 * contains all the code needed for the "now playing" screen
 */

//#ifdef NOW_PLAYING

#include <pthread.h>
#include <xmmsclient/xmmsclient-glib.h>
#include <glib.h>

#include "bnxc.h"

static GMainLoop *ml;
static int last_dur;

static void quit(void *data);
static void *handle_input();
static int handle_playtime(xmmsv_t *val, void *userdata);

void now_playing()
{
	pthread_t input_thd;

	erase();

	ml = g_main_loop_new(NULL, FALSE);
	XMMS_CALLBACK_SET(connection_async, xmmsc_signal_playback_playtime,
				handle_playtime, NULL);

	xmmsc_disconnect_callback_set(connection_async, quit, NULL);
	xmmsc_mainloop_gmain_init(connection_async);

	pthread_create(&input_thd, NULL, handle_input, NULL);

	if(xmmsc_io_want_out(connection_async))
		fprintf(stderr, "error: output already requested\n");

	g_main_loop_run(ml);
}

static void quit(void *data)
{
	return;
}

static void *handle_input()
{
	int ch;

	while(ch = getch()) {
		switch(ch) {
			case 'q':
				g_main_loop_quit(ml);
				return;
				break;
		}
	}
}

static int handle_playtime(xmmsv_t *val, void *userdata)
{
	return TRUE;

	int dur;
	if(xmmsv_is_error(val)) {
		printf("%s", xmmsv_get_error_old(val));
		fflush(stdout);
	}

	if(!xmmsv_get_int(val, &dur)) {
		printf("broken resultset");
		fflush(stdout);
	}

	if(((dur / 1000) % 60) != ((last_dur / 1000) % 60)) {
		last_dur = dur;
		mvprintw(0,0,"%d:%d\n", (dur/1000/60), (dur/1000 % 60));
//		printf("%d:%d\n", (dur/1000/60), (dur/1000 % 60));
		refresh();
	}

	return TRUE;
}
//#endif
