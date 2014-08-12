/* -------------------------------
 * Preprocessor
 * -------------------------------
 */

//#include <glib/gi18n.h>

#include <stdlib.h>

#include "misc.h"
#include "gamedata.h"
#include "gui.h"

#define MAXSIZ_FNAME              (255+1)

/* -------------------------------
 * Global Variables
 * -------------------------------
 */

gboolean global_debugOn;	/* too lazy to wrap it */

/* -------------------------------
 * Function Definitions
 * -------------------------------
 */

/* ---------------------------------------------------
 * Application entry point.
 * ---------------------------------------------------
 */
int main( int argc, char **argv )
{
	Gamedata *gamedata; /* core-data of the program */
	Gui  *gui = NULL;   /* my (barely) abstracted GUI */

	global_debugOn = TRUE;

	/* gamedata to be passed to gui */
	gamedata = new_gamedata();
	if ( NULL == gamedata ) {
		DBG_STDERR_MSG( "*** (fatal error) gamedata not inited!" );
		goto cleanup_and_exit_failure;
	}

	/* spawn the GTK+2 gui */
	gui = new_gui( &argc, &argv, gamedata );
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "(fatal error) gui not inited!" );
		goto cleanup_and_exit_failure;
	}

	/* print debugging info about the locale */
	dbg_gui_locale_print_inherited(
		gui_get_locale( gui ),
		"Caller Locale Environment was:"
		);
	dbg_gui_locale_print_current(
		gui_get_locale( gui ),
		"GUI Locale Environment set to:"
		);

	gui_start( gui );

	/* cleanup & exit */
	gui_free( gui );
	gamedata_free( gamedata );
	exit( EXIT_SUCCESS );

cleanup_and_exit_failure:

	/* cleanup & exit due to fatal error */
	gui_free( gui );
	gamedata_free( gamedata );
	exit( EXIT_FAILURE );
}
