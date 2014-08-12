#define GUI_C

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gui_locale.h"
#include "gui_menus.h"
#include "gui.h"
#include "misc.h"
#include "text.h"

/* we need this for opening http:// and mailto: links when OS is Windows */
#ifdef G_OS_WIN32
#include <windows.h>
#endif

/* a bit more convenient GUI abstraction of the status-bar */
typedef struct GuiStatusbar {
	GtkWidget  *widget;
	guint      contextId;
	guint      currMessageId;
}GuiStatusbar;

/* a bit more convenient abstraction of the application's GUI
 *	consists of the smaller structs defined above, along
 *	with a pointer that links the GUI with the core-data.
 */
struct _Gui {
	gboolean      quitOnDestroyAppWindow;
	GuiLocale     *locale;
	GtkWidget     *appWindow;
	GuiMenus      *menus;
//	GuiSettings   settings;
	struct {
		GtkWidget *ebContainer; /* event-box parent of table-widget */
		GtkWidget *table;       /* child of ebContainer */
	} board;

	struct {
		GtkWidget *ebContainer; /* event-box parent of toolbar-widget */
		GtkWidget *tb;          /* (toolbar) child of ebContainer */
	} playback;

	GtkWidget     *dlgAbout;
	GuiStatusbar  statusbar;
//	Core          *linkToCoreData;
};


/* ---------------------------------------------------
 * Callback function connected to the GTK+ "delete-event"
 * signal, for the main window of the application.
 *
 * When returning FALSE on a "delete-event" signal, GTK+
 * converts it to a "destroy" signal and re-emits it.
 * Returning TRUE means we don't want the window to be
 * destroyed, which may be useful if for example we want
 * to ask for confirmation before destroying the window.
 *
 * Here we just return FALSE, thus telling GTK+ to
 * immediately emit a "destroy" signal.
 * ---------------------------------------------------
 */
static gboolean _on_delete_event_appWindow(
	GtkWidget  *appWindow,
	GdkEvent   *event,
	Gui        *gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( NULL == appWindow || NULL == event || NULL == gui ) {
		return FALSE;
	}

	return FALSE;
}

/* ---------------------------------------------------
 * Callback function connected to the "destroy" GTK+ signal,
 * for the main window of the application.
 *
 * The "destroy" signal is emitted by a GTK+ window when
 * the user closes the window, usually by clicking on its
 * close icon (on Windows that's the 'x' icon, located at
 * the top right corner of the window).
 *
 * Normally we just want to terminate the GTK+ main-loop
 * so the flow continues to our cleanup code (if any),
 * in the main() function and then terminate the program.
 *
 * However, in this program a signal for destroying the
 * main application window does not necessarily mean that
 * the user has requested program termination.
 *
 * It could be that the signal has been "artificially"
 * produced by the program itself, when the user requested
 * a dynamic change of the GUI language, right after
 * changing the value of the environment-variable LANG.
 *
 * In those cases, the program schedules the destruction
 * of all its top-level windows (thus its main window too)
 * inside the function: gui_reload_gtkGladeFile(), by
 * calling gtk_widget_destroy() on all of them.
 *
 * Once this is done, the GUI resources are re-loaded from
 * the Glade file, which means that they get re-initialized
 * according to the new value of LANG, due to GNU-gettext.
 *
 * So obviously, a "destroy" signal sent by the main window
 * of the application does not always dictate program termination.
 *
 * To distinguish whether a "destroy" signal should terminate
 * the main loop of GTK+ or not, I use the boolean variable:
 * gui->quitOnDestroyAppWindow. If it is TRUE gtk_main_quit()
 * is called, otherwise the GTK+ main-loop keeps going on.
 *
 * See also:
 *      gui_reload_gtkGladeFile()
 *	on_activate_miEnglish()
 *	on_activate_miGreek()
 *	on_activate_miEnvironment()
 * ---------------------------------------------------
 */
void on_destroy_appWindow( GtkWidget *appWindow, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == appWindow ) {
		gtk_main_quit();
	}

	if ( gui && gui->quitOnDestroyAppWindow ) {
		gtk_main_quit();
	}
}

#ifdef G_OS_WIN32
/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate-link"
 * signal, for the dialog About.
 *
 * When run on Win32, GTK+ (Gio) cannot open links,
 * thus we overwrite the default behaviour by using
 * our own Win32 specific callback function.
 * ---------------------------------------------------
 */
static gboolean _on_activate_link_dlgAbout(
	GtkWidget  *dlgAbout,
	gchar      *uri,
	Gui        *gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( NULL == dlgAbout || NULL == uri ) {
		return FALSE;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return FALSE;
	}
	if ( NULL == gui->dlgAbout ) {
		DBG_GUI_ERRMSG( gui->appWindow, "Found invalid GUI elements" );
		return FALSE;
	}

	/* Win32 API call */
	ShellExecute( NULL, "open", uri, NULL, NULL, SW_SHOWNORMAL );

	dbg_print_info( "Signal \"activate-link\" sent for uri: %s\n", uri );


	/* Returning TRUE means we handled the event, so the signal
	* emission should be stopped (don't call any further
	* callbacks that may be connected). Return FALSE
	* to continue invoking callbacks.
	*/
	return TRUE;
}
#endif	/* #ifdef G_OS_WIN32 */

/* ---------------------------------------------------
 * Initialize the main window of the application.
 *
 * Copies the main window widget from the specified GTK+ builder
 * to my GUI abstraction: gui->appWindow, it connects callback
 * functions to it and initializes its visual appearance.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_app_window( Gui *gui, GtkBuilder *builder )
{

	if ( NULL == gui->appWindow ) {
		gui->appWindow
		= GTK_WIDGET( gtk_builder_get_object(builder, "windMain") );
	}

	gtk_window_set_icon_from_file(
		GTK_WINDOW( gui->appWindow ),
		FNAME_APPICON,
		NULL
		);
//	gtk_widget_set_size_request(gui->appWindow, 800, 600);
	gtk_widget_set_size_request(gui->appWindow, 640, 768);
	gtk_window_set_position(
		GTK_WINDOW( gui->appWindow ),
		GTK_WIN_POS_CENTER
		);
	gtk_window_set_title( GTK_WINDOW(gui->appWindow), TXT_APP_NAME );
	gtk_window_set_resizable( GTK_WINDOW(gui->appWindow), FALSE );

	g_signal_connect(
		G_OBJECT( gui->appWindow ),
		"delete-event",
		G_CALLBACK( _on_delete_event_appWindow ),
		gui
		);
	g_signal_connect(
		G_OBJECT( gui->appWindow ),
		"destroy",
		G_CALLBACK( on_destroy_appWindow ),
		gui
		);
	g_signal_connect(
		G_OBJECT( gui->appWindow ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gui->appWindow
		);

	return TRUE;
}

/* ---------------------------------------------------
 * Initialize the scores labels.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_scores( Gui *gui, GtkBuilder *builder )
{
	/* just for brevity later on */
	GtkWidget *container = gui->playback.ebContainer;
	GtkWidget *tbar      = gui->playback.tb;

	GdkColor bgcontainer;

	gdk_color_parse( "black", &bgcontainer );

	/* playback-toolbar container (event-box widget) */
	container
	= GTK_WIDGET( gtk_builder_get_object(builder, "ebPlaybackContainer") );

	gtk_widget_modify_bg(
		GTK_WIDGET( container ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);

	/* playback-toolbar */
	tbar = GTK_WIDGET( gtk_builder_get_object(builder, "tbPlayback") );

/*
	gtk_widget_modify_bg(
		GTK_WIDGET( tbar ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);
*/

	return TRUE;
}

 ---------------------------------------------------
 * Initialize the board table.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_board( Gui *gui, GtkBuilder *builder )
{
	const int w=4, h=4;
	int i,j;
	struct {
		GtkWidget *evbox;                  /* container widget */
		GtkWidget *label;                  /* label widget */
		char       lblname[MAXSIZ_DBGMSG]; /* name of label-widget */
		GdkColor   bg;                     /* evbox's bg color */
		GdkColor   fg;                     /* label's fg color */
	} tile;

	GdkColor bgcontainer;

	gdk_color_parse( "#776e65", &tile.fg );
	gdk_color_parse( "#cdc0b4", &tile.bg );
	gdk_color_parse( "#bbada0", &bgcontainer );

	/* board container (event-box widget) */
	gui->board.ebContainer
	= GTK_WIDGET( gtk_builder_get_object(builder, "ebBoardContainer") );

	gtk_widget_modify_bg(
		GTK_WIDGET( gui->board.ebContainer ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);
	g_signal_connect(
		G_OBJECT( gui->board.ebContainer ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gui->board.ebContainer
		);

	/* board table (table-widget) */
	gui->board.table
	= GTK_WIDGET( gtk_builder_get_object(builder, "tableBoard") );
	gtk_table_resize( GTK_TABLE(gui->board.table), w, h );

	g_signal_connect(
		G_OBJECT( gui->board.table ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gui->board.table
		);

	for (i=0; i < w; i++) {
		for (j=0; j < h; j++) {

//			gtk_widget_modify_bg(button, GTK_STATE_NORMAL, &red);
//			gtk_widget_modify_bg(button, GTK_STATE_PRELIGHT, &green);
//			gtk_widget_modify_bg(button, GTK_STATE_ACTIVE, &blue);

			tile.evbox = gtk_event_box_new();
			tile.label = gtk_label_new( "" );

			g_snprintf(
				tile.lblname,
				MAXSIZ_DBGMSG,
				"lb%d%d",
				i,j
				);
			gtk_widget_set_name( tile.label, tile.lblname );

			gtk_label_set_markup(
				GTK_LABEL( tile.label ),
				"<span font=\"18\" font_weight=\"bold\">0</span>"
				);
			gtk_container_add(
				GTK_CONTAINER( tile.evbox ),
				tile.label
				);

			gtk_table_attach_defaults( 
				GTK_TABLE( gui->board.table ),
				tile.evbox,
				j,    /* left_attach column */
				j+1,  /* right_attach column */
				i,    /* top_attach row */
				i+1   /* bottom_attach row */
				);
			gtk_widget_modify_bg(
				GTK_WIDGET( tile.evbox ),
				GTK_STATE_NORMAL,
				&tile.bg
				);
			gtk_widget_modify_fg(
				GTK_WIDGET( tile.label ),
				GTK_STATE_NORMAL,
				&tile.fg
				);
		}
	}

	return TRUE;
}

/* ---------------------------------------------------
 * Initialize the playback toolbar.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_tbar_playback( Gui *gui, GtkBuilder *builder )
{
	/* just for brevity later on */
	GtkWidget *container = gui->playback.ebContainer;
	GtkWidget *tbar      = gui->playback.tb;

	GdkColor bgcontainer;

	gdk_color_parse( "black", &bgcontainer );

	/* playback-toolbar container (event-box widget) */
	container
	= GTK_WIDGET( gtk_builder_get_object(builder, "ebPlaybackContainer") );

	gtk_widget_modify_bg(
		GTK_WIDGET( container ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);

	/* playback-toolbar */
	tbar = GTK_WIDGET( gtk_builder_get_object(builder, "tbPlayback") );

/*
	gtk_widget_modify_bg(
		GTK_WIDGET( tbar ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);
*/

	return TRUE;
}

/* ---------------------------------------------------
 * Initialize the status-bar of our main window.
 *
 * Copies the GUI status-bar from the specified GTK+ builder into
 * my GUI status-bar abstraction: gui->statusbar (GuiStatusbar),
 * it connects callback functions to it and initializes its visual
 * appearance.
 *
 * My status-bar abstraction consists of a GTK+ statusbar widget,
 * and a couple of supporting fields for the context-id and the
 * text-message. We get the former from the builder and we
 * initialize & apply the latter.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_statusbar( Gui *gui, GtkBuilder *builder )
{
	gui->statusbar.widget =
		GTK_WIDGET( gtk_builder_get_object(builder, "statusbar") );

	g_signal_connect(
		G_OBJECT( gui->statusbar.widget ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gui->statusbar.widget
		);

	gui->statusbar.contextId =
		gtk_statusbar_get_context_id(
			GTK_STATUSBAR( gui->statusbar.widget ),
			"Info"
			);

	gui->statusbar.currMessageId = 0;

	return TRUE;
}

/* ---------------------------------------------------
 * Initialize the "About" dialog.
 *
 * Copies the GUI dialog "About" from the specified GTK+
 * builder into my GUI abstraction: gui->dlgAbout, it
 * initializes the dialog's contents and it connects
 * callback functions.
 *
 * The "About" dialog is defined of type GtkAboutDialog
 * in the Glade file, which provides a standardized way
 * of presenting inormation. However, I haven't found
 * an easy way to access the members of its action-area,
 * namely the buttons: "Credits", "License" and "Close".
 *
 * Consequently, I cannot mark the labels of those buttons
 * as translatable, for the GNU-gettext library (I could
 * implement it as a normal GtkDialog, but I prefer to
 * learn the GtkAboutDialog internals and use them in
 * a future version).
 *
 * NOTE:
 *	The callback function: on_activate_link_dlgAbout()
 *	is connected to the "activate-link" signal ONLY
 *	when the program is compiled under Win32. GTK+'s
 *	file-engine (Gio) has trouble registering the
 *	"http://" and "mailto:" protocols under Win32,
 *	so I conditionally use the Win32 API if needed.
 *
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the dialog's pointer will be set to
 *	NULL after the widget gets destroyed.
 * ---------------------------------------------------
 */
static inline gboolean _init_dlg_about(
	Gui         *gui,
	GtkBuilder  *builder
	)
{
	GdkPixbuf   *logo      = NULL;
	GError      *error     = NULL;
	const gchar *authors[] = {TXT_APP_AUTHOR, NULL};
	const gchar *artists[] = {TXT_APP_ARTIST, NULL};

	/* load the about-dialog into gui, from the GTK+2 builder */
	gui->dlgAbout = GTK_WIDGET( gtk_builder_get_object(builder, "dlgAbout") );
	g_signal_connect(
		G_OBJECT( gui->dlgAbout ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&gui->dlgAbout
		);

	gtk_window_set_icon_from_file(
		GTK_WINDOW( gui->dlgAbout ),
		FNAME_APPICON,
		NULL
		);

	/* set the about-dialog opacity */
	gtk_window_set_opacity( GTK_WINDOW(gui->dlgAbout), 0.90 );

	/* set the about-dialog logo */
	logo = gdk_pixbuf_new_from_file( FNAME_APPLOGO, &error );
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(gui->dlgAbout), logo );

	/* set the remaining prperties */
	gtk_about_dialog_set_program_name(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		gettext( TXT_APP_NAME )
		);

	gtk_about_dialog_set_version(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		gettext( TXT_APP_VERSION )
		);

	gtk_about_dialog_set_comments(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		TXT_APP_DESCRIPTION
		);

	gtk_about_dialog_set_copyright(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		gettext( TXT_APP_COPYRIGHT )
		);

	gtk_about_dialog_set_wrap_license(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		TRUE
		);
	gtk_about_dialog_set_license(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		TXT_APP_LICENSE
		);

	gtk_about_dialog_set_authors(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		authors
		);

	gtk_about_dialog_set_documenters(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		authors
		);

	gtk_about_dialog_set_artists(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		artists
		);

	gtk_about_dialog_set_website_label(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		gettext( TXT_APP_WEBSITE_LABEL )
		);
	gtk_about_dialog_set_website(
		GTK_ABOUT_DIALOG( gui->dlgAbout ),
		TXT_APP_WEBSITE
		);

	/*
	 * connect callback functions to signals emitted by the dialog
	 */

#ifdef G_OS_WIN32
	/* when run on Win32, GTK+ (Gio) cannot open links,
	 * thus we overwrite the default behaviour by using
	 * our own Win32 specific callback function
	 */
	g_signal_connect(
		G_OBJECT(gui->dlgAbout),
		"activate-link",
		G_CALLBACK( _on_activate_link_dlgAbout ),
		gui
		);
#endif

	/* ensure that gui->dlgAbout will be set to NULL after it is destroyed */
	g_signal_connect(
		G_OBJECT( gui->dlgAbout ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),   /* provided by GTK+ */
		&gui->dlgAbout
		);


	return TRUE;
}

/* ---------------------------------------------------
 * Load GUI resources from the specified Glade file.
 *
 * This function uses a GTK+ builder object to read the
 * GUI resources defined in the specified glade-file,
 * then it copies into my GUI abstraction only those
 * resources that need further manipulation dynamically,
 * and it initializes them.
 * ---------------------------------------------------
 */
static inline gboolean _init_from_glade_file( Gui *gui, const gchar *fnameGlade )
{
	GtkBuilder *builder = NULL;
	GError     *error   = NULL;

	/* create a GTK+2 builder with all the widgets defined in the glade-file */
	builder = gtk_builder_new();
	if ( !gtk_builder_add_from_file( builder, fnameGlade, &error ) )
	{
		DBG_GUI_ERRMSG( gui->appWindow, error->message );
		g_warning( "%s: (fatal error) %s", error->message, __func__ );
		g_error_free( error );
		return FALSE;
	}


	/*
	 * TEMPRARILY REMOVE RIGHT PANE:
	 * delete the "vboxWorkareaRight" container & all its children
	 */
	GtkWidget *temp =
		GTK_WIDGET( gtk_builder_get_object(builder, "vboxWorkareaRight") );
	gtk_widget_destroy( temp );

	/* init GUI using only the needed elements from the GTK+2 builder */

	_init_app_window( gui, builder );
	gui_menus_init( gui->menus, builder, gui );
	_init_board( gui, builder );
	_init_tbar_playback( gui, builder );
	_init_statusbar( gui, builder );
	_init_dlg_about( gui, builder );

	/* release the GTK+2 builder */
	g_object_unref( G_OBJECT(builder) );

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
Gui *gui_free( Gui *gui )
{
	if ( gui ) {
		dbg_gui_locale_print_current(
			gui->locale,
			"___ Local Locale Environment ___"
			);
		dbg_gui_locale_print_inherited(
			gui->locale,
			"___ Will be restored to... ___"
			);

		/* restore os locale environment and free local */
		gui_locale_free( gui->locale );

		/* free menus */
		gui_menus_free( gui->menus );
	}
	return NULL;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
Gui *new_gui(
	int         *argc,          /* needed by GTK+ during initialization */
	char        ***argv        /* needed by GTK+ during initialization */
//	const Core  *core,          /* for linking gui to app's core-data */
	)
{
	Gui *gui = NULL;

	/* sanity checks */
	if ( /*!core ||*/ NULL == argc || NULL == argv ) {
		return NULL;
	}

	/* initialize GTK+ */
	if ( !gtk_init_check( argc, argv ) ) {
		DBG_STDERR_MSG( "(fatal error) gtk+ not inited!" );
		return NULL;
	}

	gui = calloc( 1, sizeof(*gui) );
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "(fatal error) calloc() failed!" );
		return NULL;
	}

	/* initiate gui's locale environment */
	gui->locale = new_gui_locale();
	if ( NULL == gui->locale ) {
		DBG_STDERR_MSG( "(fatal error) new_gui_locale() failed!" );
		goto ret_failure;
	}

	/* instantiate gui's menu abstractions */
	gui->menus = new_gui_menus();
	if ( NULL == gui->menus ) {
		DBG_STDERR_MSG( "(fatal error) new_gui_menus() failed!" );
		goto ret_failure;
	}

	/* initially, destruction of main window results in program-termination */
	gui->quitOnDestroyAppWindow = TRUE;

	/* link gui with the core-data of the program */
//	gui->linkToCoreData = (Core *) core;

	/* load resources from Glade file into gui */
	if ( !_init_from_glade_file(gui, FNAME_GLADE) ) {
		DBG_STDERR_MSG( "(fatal error) gui resources not loaded!" );
		goto ret_failure;
	}

	return gui;

ret_failure:
	return gui_free( gui );
}

/* ---------------------------------------------------
 * Reload GUI resources from the specified Glade file.
 *
 * This function is called when the user requests a dynamic
 * change of the GUI language, via the "Language" menu.
 *
 * It schedules the destruction of all the top-level windows
 * of the program by calling gtk_widget_destroy() on them.
 *
 * Before scheduling the main window of the application,
 * it sets 'gui->quitOnDestroyAppWindow' to FALSE so that
 * the connected callback function: on_destroy_appWindow()
 * will NOT terminate the program after destructing the window
 * (remember that 'gui' is passed as user-data to the callback
 * function).
 *
 * Once scheduling is done, 'gui->quitOnDestroyAppWindow'
 * is reset back to TRUE and the GUI resources are loaded
 * from scratch, from the glade-file into my 'gui' abstraction,
 * by calling: gui_load_gtkGladeFile().
 *
 * NOTE:
 *	Before this function is called, the environment-variable
 *	LANG is explicitly set according to the user-request.
 *	Since the GNU-gettext library is runtime-aware with
 *	a LANG dependency, the reloaded GUI is displayed in
 *	the language requested by the user.
 *
 * See also:
 *	on_activate_miEnglish()
 *	on_activate_miGreek()
 *	and on_activate_miEnvironment()
 * ---------------------------------------------------
 */
gboolean gui_reload_from_glade_file( Gui *gui, const gchar *fnameGlade )
{
	/* sanity check */
	if ( NULL == gui || NULL == fnameGlade ) {
		return FALSE;
	}

	/* schedule the destruction of all top-level windows */
	if ( gui->dlgAbout ) {
		gtk_widget_destroy( gui->dlgAbout );
		gui->dlgAbout = NULL;
	}
	if ( gui->appWindow ) {
		gui->quitOnDestroyAppWindow = FALSE;
		gtk_widget_destroy( gui->appWindow );
	}

	gui->quitOnDestroyAppWindow = TRUE;
	return _init_from_glade_file(gui, fnameGlade);
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_start( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return FALSE;
	}
	if ( NULL == gui->appWindow ) {
		DBG_GUI_ERRMSG(	NULL, "gui->appWindow is NULL!" );
		return FALSE;
	}

	/* display the main-window of the application, and all its children */
	gtk_widget_show_all( gui->appWindow );

	/* start the GTK+ event loop */
	gtk_main();

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GtkWidget *gui_get_appWindow( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return NULL;
	}

	return gui->appWindow;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiLocale *gui_get_locale( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return NULL;
	}

	return gui->locale;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiMenus *gui_get_menus( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return NULL;
	}
	return gui->menus;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GtkWidget *gui_get_dlgAbout( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return NULL;
	}
	return gui->dlgAbout;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_get_quitOnDestroyAppWindow( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return FALSE;
	}
	return gui->quitOnDestroyAppWindow;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_set_quitOnDestroyAppWindow( Gui *gui, gboolean val )
{
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)!" );
		return FALSE;
	}
	gui->quitOnDestroyAppWindow = val;
	return TRUE;
}
