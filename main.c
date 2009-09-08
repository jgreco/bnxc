/* main.c
 *
 * contains the main function for bnxc
 */

#include "bnxc.h"
#include "config.h"

int main(int argc, char* argv[])
{
	if(!read_db(getenv("BNXC")))
	{
		fprintf(stderr, "failed to read the database file: %s\n", getenv("BNXC"));
		return -1;
	}

#ifdef XMMS2
/* START XMMS2 STUFF */
	connection = xmmsc_init("bnxc");
	if(!connection) {
		fprintf(stderr, "terrible terrible things just happened\n");
		exit(EXIT_FAILURE);
	}
	if(!xmmsc_connect(connection, getenv("XMMS_PATH"))) {
		fprintf(stderr, "Connection to xmms2d failed: %s\n", xmmsc_get_last_error(connection));
		exit(EXIT_FAILURE);
	}
	collection_path = getenv("COLLECTION");
#endif
#ifdef FRONTEND_NCURSES
/* START NCURSES STUFF */
	setlocale(LC_CTYPE, "");  /* make sure we display UTF-8 crap properly */

	initscr();  /* start ncurses */
	raw();  /* enable raw keyboard input */
	keypad(stdscr, TRUE);  /* more keyboard input stuffs */
	noecho();  /* don't echo stdin to stdout */
	curs_set(0);  /* invisible cursor (maybe we should just always put the cursor on the player?) */
	start_color();
	use_default_colors();

	/*           forground   ,background */
	init_pair(1, COLOR_YELLOW,COLOR_RED);  /*color pair for the selected line */
	init_pair(2, COLOR_WHITE, COLOR_CYAN);  /*color pair for "inactive sections" */
	init_pair(3, COLOR_WHITE, COLOR_BLUE);  /*color pair for active sections and the title */
	assume_default_colors(COLOR_WHITE, COLOR_BLACK);  /*default to white on black */

	ninterface(); /* start the interactive ncurses interface */

	endwin();  /* cleanup */

	dump_db();

	return 0;
#endif

#ifndef FRONTEND_NCURSES
	printf("NCurses interface was not compiled in.\n");

	return 1;
#endif
}
