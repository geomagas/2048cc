/* -------------------------------
 * Preprocessor
 * -------------------------------
 */

//#include <glib/gi18n.h>

#include <stdlib.h>

#include "misc.h"
#include "gui.h"

/* some handy constants */

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
//	Core core;          /* core-data of the program */
	Gui  *gui = NULL;   /* my (barely) abstracted GUI */

	global_debugOn = TRUE;

	/* init core-data */
//	if ( !core_init(&core) ) {
//		DBG_STDERR_MSG( "(fatal error) core data not inited!" );
//		goto cleanup_and_exit_failure;
//	}

	/* spawn the GTK+2 gui */
	gui = new_gui( &argc, &argv/*, Core *core*/ );
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
//	core_cleanup( &core );
	exit( EXIT_SUCCESS );

cleanup_and_exit_failure:

	/* cleanup & exit due to fatal error */
	gui_free( gui );
//	core_cleanup( &core );
	exit( EXIT_FAILURE );
}
