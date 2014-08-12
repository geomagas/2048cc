#define GUI_C

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gui_locale.h"
#include "gui_statusbar.h"
#include "gui_board.h"
#include "gui_menus.h"
#include "gui.h"

#include "misc.h"
#include "text.h"
#include "gamedata.h"

/* we need this for opening http:// and mailto: links when OS is Windows */
#ifdef G_OS_WIN32
#include <windows.h>
#endif

/* a bit more convenient abstraction of the application's GUI
 *	consists of the smaller structs defined above, along
 *	with a pointer that links the GUI with the core-data.
 */
struct _Gui
{
	gint wmax, hmax;  /* width & height of user's full-screen */

	gboolean  quitOnDestroyAppWindow;
	GuiLocale *locale;
	GtkWidget *appWindow;
	GuiMenus  *menus;
	GuiBoard  *board;


//	GuiSettings   settings;

	/* score & best-score*/
	struct {
		GtkWidget *ebScoreContainer;  /* event-box parent */
		GtkWidget *score;             /* label child */
		GtkWidget *ebBScoreContainer; /* event-box parent */
		GtkWidget *bscore;            /* label child */
	} scores;

	/* playback toolbar */
	struct {
		GtkWidget *ebContainer; /* event-box parent of toolbar-widget */
		GtkWidget *tb;          /* (toolbar) child of ebContainer */
		GtkWidget *toolPlay;
		GtkWidget *toolPause;
		GtkWidget *toolStop;
		GtkWidget *toolFirst;
		GtkWidget *toolPrev;
		GtkWidget *toolNext;
		GtkWidget *toolLast;
		GtkWidget *toolSlower;
		GtkWidget *toolResetspeed;
		GtkWidget *toolFaster;
	} playback;

	GtkWidget     *imgPrev, *imgNext;

	GtkWidget     *dlgAbout;
	GuiStatusbar  *statusbar;

	Gamedata      *gamedata;
	gboolean      hasloadedfile;
	long int      imove;            /* currently displayed move */
};


GuiMenus  *gui_get_menus( const Gui *gui );

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
 * inside the function: gui_reload_from_glade_file(), by
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

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "clicked"
 * signal, for the "Reset speed" tool in the playback
 * toolbar.
 *
 * NOTE: When the tool is clicked, we send an "activate"
 *       signal to the menu-item "Play->Speed->Reset",
 *       which corresponds to this tool.
 * ---------------------------------------------------
 */
static void _on_clicked_toolResetspeed( GtkWidget *tool, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	(void)tool;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	GtkWidget *mi = gui_menus_get_mirPlaybackSpeedReset(
				gui_get_menus( gui )
				);
//	g_signal_emit_by_name( mireset, "activate" );
	gtk_menu_item_activate( GTK_MENU_ITEM(mi) );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "clicked"
 * signal, for the "Slower" tool in the playback toolbar.
 *
 * NOTE: When the tool is clicked, we send an "activate"
 *       signal to the menu-item "Play->Speed->Slower",
 *       which corresponds to this tool.
 * ---------------------------------------------------
 */
static void _on_clicked_toolSlower( GtkWidget *tool, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	(void)tool;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	GtkWidget *mi = gui_menus_get_mirPlaybackSpeedSlower(
				gui_get_menus( gui )
				);
//	g_signal_emit_by_name( mireset, "activate" );
	gtk_menu_item_activate( GTK_MENU_ITEM(mi) );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "clicked"
 * signal, for the "Faster" tool in the playback toolbar.
 *
 * NOTE: When the tool is clicked, we send an "activate"
 *       signal to the menu-item "Play->Speed->Faster",
 *       which corresponds to this tool.
 * ---------------------------------------------------
 */
static void _on_clicked_toolFaster( GtkWidget *tool, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	(void)tool;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	GtkWidget *mi = gui_menus_get_mirPlaybackSpeedFaster(
				gui_get_menus( gui )
				);
//	g_signal_emit_by_name( mireset, "activate" );
	gtk_menu_item_activate( GTK_MENU_ITEM(mi) );

	return;
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
 * 
 * ---------------------------------------------------
 */
static inline gboolean _is_valid_imove( long int imove, Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	return 
	-1 == imove || ( imove > -1 && imove < gamedata_get_nmoves(gd) );
}

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
static inline gboolean _init_app_window_from_builder(
	Gui        *gui,
	GtkBuilder *builder
	)
{
	if ( NULL == gui->appWindow ) {
		gui->appWindow
		= GTK_WIDGET( gtk_builder_get_object(builder, "windMain") );
	}

	/* ideally we want 800px height, but we need to check user's screen */
//	gtk_widget_set_size_request(gui->appWindow, 800, 600);
	gint h = (gui->hmax < 800) ? gui->hmax : 800;
	gtk_widget_set_size_request( gui->appWindow, 590, h );

	gtk_window_set_icon_from_file(
		GTK_WINDOW( gui->appWindow ),
		FNAME_APPICON,
		NULL
		);
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
static inline gboolean _init_scores_from_builder(
	Gui        *gui,
	GtkBuilder *builder
	)
{
	char markup[SZMAX_DBGMSG] = {'\0'};

	/* just for brevity later on */
	GtkWidget **scoreContainer  = &gui->scores.ebScoreContainer;
	GtkWidget **bscoreContainer = &gui->scores.ebBScoreContainer;
	GtkWidget **score  = &gui->scores.score;
	GtkWidget **bscore = &gui->scores.bscore;

	GdkColor bg;    /* for score & bscore event-box containers */
	GdkColor fg;    /* for score & bscore labels */

	gdk_color_parse( "#bbada0", &bg );
	gdk_color_parse( "#eee4da", &fg );

	/* score & bscore containers (event-box widgets) */
	(*scoreContainer)
	= GTK_WIDGET( gtk_builder_get_object(builder, "ebScoreContainer") );

	(*bscoreContainer)
	= GTK_WIDGET( gtk_builder_get_object(builder, "ebBScoreContainer") );

	gtk_widget_modify_bg(
		GTK_WIDGET( *scoreContainer ),
		GTK_STATE_NORMAL,
		&bg
		);
	gtk_widget_modify_bg(
		GTK_WIDGET( *bscoreContainer ),
		GTK_STATE_NORMAL,
		&bg
		);

	/* score & bscore text (label widgets) */
	(*score) = GTK_WIDGET( gtk_builder_get_object(builder, "lblScore") );
	g_snprintf( markup, SZMAX_DBGMSG, TXTF_MARKUP_SCORE, 0L );
	gtk_label_set_markup( GTK_LABEL(*score), markup );

	(*bscore) = GTK_WIDGET( gtk_builder_get_object(builder, "lblBScore") );
	g_snprintf( markup, SZMAX_DBGMSG, TXTF_MARKUP_BSCORE, 0L );
	gtk_label_set_markup( GTK_LABEL(*bscore), markup );

	gtk_widget_modify_fg(
		GTK_WIDGET( *score ),
		GTK_STATE_NORMAL,
		&fg
		);
	gtk_widget_modify_fg(
		GTK_WIDGET( *bscore ),
		GTK_STATE_NORMAL,
		&fg
		);

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
static inline gboolean _init_tbar_playback_from_builder(
	Gui *gui,
	GtkBuilder *builder
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */
	GtkWidget **container = &gui->playback.ebContainer;
	GtkWidget **tbar      = &gui->playback.tb;

	GdkColor bgcontainer;

	gdk_color_parse( "black", &bgcontainer );

	/* playback-toolbar container (event-box widget) */

	(*container)
	= GTK_WIDGET( gtk_builder_get_object(builder, "ebPlaybackContainer") );

	gtk_widget_modify_bg(
		GTK_WIDGET( *container ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);

	/* playback-toolbar */
	(*tbar) = GTK_WIDGET( gtk_builder_get_object(builder, "tbPlayback") );

/*
	gtk_widget_modify_bg(
		GTK_WIDGET( *tbar ),
		GTK_STATE_NORMAL,
		&bgcontainer
		);
*/

	/* playback-toolbar tools */

	/* tool: Play */
	pw  = &gui->playback.toolPlay;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder,"toolPlaybackPlay") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackPlay ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Pause */
	pw  = &gui->playback.toolPause;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "toolPlaybackPause"));
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackPause ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Stop */
	pw  = &gui->playback.toolStop;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "toolPlaybackStop"));
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackStop ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: First */
	pw  = &gui->playback.toolFirst;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "toolPlaybackFirst"));
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackFirst ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Prev */
	pw  = &gui->playback.toolPrev;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "toolPlaybackPrev"));
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackPrevious ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Next */
	pw  = &gui->playback.toolNext;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "toolPlaybackNext"));
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackNext ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Last */
	pw  = &gui->playback.toolLast;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "toolPlaybackLast"));
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( on_activate_miPlaybackLast ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Reset speed */
	pw  = &gui->playback.toolResetspeed;
	*pw = GTK_WIDGET(
		gtk_builder_get_object( builder, "toolPlaybackResetspeed" )
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( _on_clicked_toolResetspeed ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Slower speed */
	pw  = &gui->playback.toolSlower;
	*pw = GTK_WIDGET(
		gtk_builder_get_object( builder, "toolPlaybackSlower" )
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( _on_clicked_toolSlower ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* tool: Faster speed */
	pw  = &gui->playback.toolFaster;
	*pw = GTK_WIDGET(
		gtk_builder_get_object( builder, "toolPlaybackFaster" )
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"clicked",
		G_CALLBACK( _on_clicked_toolFaster ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* prev & next move indicators */

	gui->imgPrev = GTK_WIDGET( gtk_builder_get_object(builder, "imgPrev") );
	gui->imgNext = GTK_WIDGET( gtk_builder_get_object(builder, "imgNext") );

	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(
		DIR_GUI"imgNomove.png",
		NULL
		);
	gtk_image_set_from_pixbuf( GTK_IMAGE(gui->imgPrev), pixbuf );
	gtk_image_set_from_pixbuf( GTK_IMAGE(gui->imgNext), pixbuf );

	g_object_unref( G_OBJECT(pixbuf) );

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
static inline gboolean _init_dlg_about_from_builder(
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

	/* set the remaining properties */
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
		g_object_unref( G_OBJECT(builder) );
		return FALSE;
	}

	/*
	 * TEMPORARILY REMOVE RIGHT PANE:
	 * delete the "vboxWorkareaRight" container & all its children
	 */
	GtkWidget *temp =
		GTK_WIDGET( gtk_builder_get_object(builder, "vboxWorkareaRight") );
	gtk_widget_destroy( temp );

	/* init GUI using only the needed elements from the GTK+2 builder */

	_init_app_window_from_builder( gui, builder );
	gui_menus_init_from_builder( gui->menus, builder, gui );
	gui_board_init_from_builder( gui->board, builder, gui );
	gui_statusbar_init_from_builder( gui->statusbar, builder, gui );
	_init_scores_from_builder( gui, builder );
	_init_tbar_playback_from_builder( gui, builder );
	_init_dlg_about_from_builder( gui, builder );

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

		/* free remaining resources */
		gui_menus_free( gui->menus );
		gui_board_free( gui->board );
		gui_statusbar_free( gui->statusbar );
	}

	return NULL;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
Gui *new_gui(
	int      *argc,          /* needed by GTK+ during initialization */
	char     ***argv,         /* needed by GTK+ during initialization */
	Gamedata *gamedata       /* for linking gui to app's core game-data */
	)
{
	Gui       *gui = NULL;
	GdkScreen *fs  = NULL;      /* full-screen */

	/* sanity checks */
	if ( NULL == argc || NULL == argv || NULL == gamedata ) {
		DBG_STDERR_MSG( "(fatal error) NULL pointer argument!" );
		return NULL;
	}

	/* initialize GTK+ */
	if ( !gtk_init_check( argc, argv ) ) {
		DBG_STDERR_MSG( "(fatal error) gtk+ not inited!" );
		return NULL;
	}

	/* instantiate a gui object */
	gui = calloc( 1, sizeof(*gui) );
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "(fatal error) calloc() failed!" );
		return NULL;
	}

	/* */
	gui->hasloadedfile = FALSE;
	gui->imove = -1;

	/* link gui with the core game-data of the program */
	gui->gamedata = gamedata;

	/* get & store full-screen dimensions */
	fs = gdk_screen_get_default();
	gui->wmax = gdk_screen_get_width( fs );
	gui->hmax = gdk_screen_get_height( fs );

	/* initiate gui's locale environment */
	gui->locale = new_gui_locale();
	if ( NULL == gui->locale ) {
		DBG_STDERR_MSG( "(fatal error) new_gui_locale() failed!" );
		goto ret_failure;
	}

	/* instantiate gui's menu abstractions */
	gui->menus = make_gui_menus();
	if ( NULL == gui->menus ) {
		DBG_STDERR_MSG( "(fatal error) make_gui_menus() failed!" );
		goto ret_failure;
	}

	/* instantiate gui's board abstraction */
	gui->board = make_gui_board();
	if ( NULL == gui->board ) {
		DBG_STDERR_MSG( "(fatal error) make_gui_board() failed!" );
		goto ret_failure;
	}

	/* instantiate gui's statusbar abstraction */
	gui->statusbar = make_gui_statusbar();
	if ( NULL == gui->statusbar ) {
		DBG_STDERR_MSG( "(fatal error) make_gui_statusbar() failed!" );
		goto ret_failure;
	}

	/* initially, destruction of main window results in program-termination */
	gui->quitOnDestroyAppWindow = TRUE;

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
GtkWidget *gui_get_appWindow( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
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
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
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
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return NULL;
	}
	return gui->menus;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GtkWidget *gui_get_statusbar_widget( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return NULL;
	}
	return gui_statusbar_get_widget( gui->statusbar );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
guint gui_get_statusbar_contextId( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return G_MININT;
	}
	return gui_statusbar_get_contextId( gui->statusbar );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GtkWidget *gui_get_dlgAbout( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return NULL;
	}
	return gui->dlgAbout;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
Gamedata *gui_get_gamedata( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return NULL;
	}
	return gui->gamedata;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
char *gui_get_gamedata_fname( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return NULL;
	}
	return gamedata_get_fname( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
long int gui_get_gamedata_nmoves( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_nmoves( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
int gui_get_gamedata_dim( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_dim( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
long int gui_get_gamedata_sentinel( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_sentinel( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
int gui_get_gamedata_nrandom( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_nrandom( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
unsigned long int gui_get_gamedata_delay( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_delay( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
int gui_get_gamedata_gamewon( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_gamewon( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
long int gui_get_gamedata_tile_of_move(
	const Gui *gui,
	int       itile,
	long int  imove
	)
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gamedata_get_tile_of_move( gui->gamedata, itile, imove );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GtkWidget *gui_get_board_container( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return NULL;
	}

	return gui_board_get_container( gui->board, gui );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
long int gui_get_imove( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return 0;
	}
	return gui->imove;
}


/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
void dbg_gui_print_gamedata_tiles( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return;
	}
	dbg_gamedata_print_tiles( gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_get_quitOnDestroyAppWindow( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
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
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}
	gui->quitOnDestroyAppWindow = val;
	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_set_gamedata_from_fname( Gui *gui, char *fname )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}
	if ( NULL == fname ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"NULL pointer argument (gui)!"
			);
		return FALSE;
	}

	return gamedata_set_from_fname( gui->gamedata, fname );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_is_imove_valid( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	return _is_valid_imove( gui->imove, gui->gamedata );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_has_imove_prev( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	if ( gui->imove < 1
	|| gui->imove > gamedata_get_nmoves(gui->gamedata) - 1
	){
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_has_imove_next( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	if ( gui->imove < 0
	|| gui->imove > gamedata_get_nmoves(gui->gamedata) - 2
	){
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_set_imove( Gui *gui, long int imove )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	if ( ! _is_valid_imove(imove, gui->gamedata) ) {
		DBG_GUI_ERRMSG( gui->appWindow, "Invalid imove!" );
		return FALSE;
	}

	gui->imove = imove;
	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_set_imove_prev( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	if ( !gui_has_imove_prev(gui) ) {
		DBG_GUI_ERRMSG( gui->appWindow, "There's no previous move!" );
		return FALSE;
	}

	gui->imove--;
	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_set_imove_next( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	if ( !gui_has_imove_next(gui) ) {
		DBG_GUI_ERRMSG( gui->appWindow, "There's no next move!" );
		return FALSE;
	}

	gui->imove++;
	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_get_hasloadedfile( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	return gui->hasloadedfile;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_set_hasloadedfile( Gui *gui, gboolean hasloadedfile )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	gui->hasloadedfile = hasloadedfile;
	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
/*
gboolean _refresh_menus( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG(	"Invalid pointer argument (gui)" );
		return FALSE;
	}

	return gui_menus_refresh( gui->menus, gui );
}
*/
/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean _refresh_scores( Gui *gui )
{
	char markup[SZMAX_DBGMSG] = {'\0'};

//	if ( NULL == gui ) {
//		DBG_STDERR_MSG(	"Invalid pointer argument (gui)" );
//		return FALSE;
//	}

	/* score */
	g_snprintf(
		markup,
		SZMAX_DBGMSG,
		TXTF_MARKUP_SCORE,
		gamedata_get_score_of_move( gui->gamedata, gui->imove )
		);
	gtk_label_set_markup( GTK_LABEL(gui->scores.score), markup );

	/* best score */
	g_snprintf(
		markup,
		SZMAX_DBGMSG,
		TXTF_MARKUP_BSCORE,
		gamedata_get_bestscore_of_move( gui->gamedata, gui->imove )
		);
	gtk_label_set_markup( GTK_LABEL(gui->scores.bscore), markup );

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_resize_board( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	return gui_board_resize( gui->board, gui );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
/*
gboolean _refresh_board( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}

	return gui_board_refresh( gui->board, gui );
}
*/
/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean _refresh_tbar_playback( Gui *gui )
{
//	if ( NULL == gui ) {
//		DBG_STDERR_MSG(	"Invalid pointer argument (gui)" );
//		return FALSE;
//	}

	/* when no replay-file is loaded */
	if ( !gui_get_hasloadedfile(gui) ) {
		gtk_widget_set_sensitive( gui->playback.ebContainer, FALSE );
		return TRUE;
	}
	gtk_widget_set_sensitive( gui->playback.ebContainer, TRUE );

	/* toolFirst & toolPrev */
	if ( gui_has_imove_prev(gui) ) {
		gtk_widget_set_sensitive( gui->playback.toolFirst, TRUE );
		gtk_widget_set_sensitive( gui->playback.toolPrev, TRUE );
	}
	else {
		gtk_widget_set_sensitive( gui->playback.toolFirst, FALSE );
		gtk_widget_set_sensitive( gui->playback.toolPrev, FALSE );
	}

	/* toolNext & toolLast */
	if ( gui_has_imove_next(gui) ) {
		gtk_widget_set_sensitive( gui->playback.toolNext, TRUE );
		gtk_widget_set_sensitive( gui->playback.toolLast, TRUE );
	}
	else {
		gtk_widget_set_sensitive( gui->playback.toolNext, FALSE );
		gtk_widget_set_sensitive( gui->playback.toolLast, FALSE );
	}

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean _refresh_prevnext( Gui *gui )
{
	GdkPixbuf *pixbuf = NULL;
	char *fnames[] = {
		DIR_GUI"imgNomove.png",
		DIR_GUI"imgUp.png",
		DIR_GUI"imgDown.png",
		DIR_GUI"imgLeft.png",
		DIR_GUI"imgRight.png"
	};

//	if ( NULL == gui ) {
//		DBG_STDERR_MSG(	"Invalid pointer argument (gui)" );
//		return FALSE;
//	}
	
	/* when no replay-file is loaded */
	if ( !gui_get_hasloadedfile(gui) ) {
		gtk_widget_set_sensitive( gui->imgPrev, FALSE );
		gtk_widget_set_sensitive( gui->imgNext, FALSE );
		return TRUE;
	}
	gtk_widget_set_sensitive( gui->imgPrev, TRUE );
	gtk_widget_set_sensitive( gui->imgNext, TRUE );

	pixbuf = gdk_pixbuf_new_from_file(
//		DIR_GUI"imgNomove.png",
		fnames[gamedata_get_prevmv_of_move(gui->gamedata, gui->imove)],
		NULL
		);
	gtk_image_set_from_pixbuf( GTK_IMAGE(gui->imgPrev), pixbuf );
	g_object_unref( G_OBJECT(pixbuf) );

	pixbuf = gdk_pixbuf_new_from_file(
//		DIR_GUI"imgNomove.png",
		fnames[gamedata_get_nextmv_of_move(gui->gamedata, gui->imove)],
		NULL
		);
	gtk_image_set_from_pixbuf( GTK_IMAGE(gui->imgNext), pixbuf );
	g_object_unref( G_OBJECT(pixbuf) );

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean _refresh_statusbar( const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}
	return gui_statusbar_refresh( gui->statusbar, gui );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_refresh( Gui *gui )
{
//	if ( NULL == gui ) {
//		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
//		return FALSE;
//	}

	gui_menus_refresh( gui->menus, gui );        // _refresh_menus( gui );
	_refresh_scores( gui );
	gui_board_refresh( gui->board, gui);         //_refresh_board( gui );
	_refresh_tbar_playback( gui );
	_refresh_prevnext( gui );
	gui_statusbar_refresh( gui->statusbar, gui); //_refresh_statusbar(gui);

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_start( Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)!" );
		return FALSE;
	}
	if ( NULL == gui->appWindow ) {
		DBG_GUI_ERRMSG(	NULL, "gui->appWindow is NULL!" );
		return FALSE;
	}

	gui_refresh( gui );

	/* display the main-window of the application, and all its children */
	gtk_widget_show_all( gui->appWindow );

	/* start the GTK+ event loop */
	gtk_main();

	return TRUE;
}
