#define GUI_MENUS_C

/* Prefix naming conventions:
 * mbar: menu-bar
 * mt  : menu-title
 * mn  : menu
 * mi  : menu-item
 * mir : menu-item-radio
 */
#include <gtk/gtk.h>
#include <stdio.h>    /* just for debugging messages in the console */
#include <stdlib.h>

#include "misc.h"
#include "text.h"
#include "gui_menus.h"
#include "gui.h"

#include "gamedata.h"

/* a bit more convenient GUI abstraction of the menus */
struct _GuiMenus
{
	/* File menu */
	struct {
		GtkWidget  *mt;
		GtkWidget  *miOpen;
		GtkWidget  *miQuit;
	} file;

	/* Playback menu */
	struct {
		GtkWidget  *mt;
		GtkWidget  *miSpeed;
		struct {
			GtkWidget *submenu;
			GtkWidget *mirReset;
			GtkWidget *mirSlower;
			GtkWidget *mirFaster;
		} speed;

		GtkWidget  *miPlay;
		GtkWidget  *miPause;
		GtkWidget  *miStop;

		GtkWidget  *miFirst;
		GtkWidget  *miPrevious;
		GtkWidget  *miNext;
		GtkWidget  *miLast;

		GtkWidget  *miJumpto;
	} playback;

	/* Language menu */
	struct {
		GtkWidget  *mt;
		GtkWidget  *mn;
		GtkWidget  *miEnglish;
		GtkWidget  *miGreek;
		GtkWidget  *miEnvironment;
	} lang;

	/* Help menu */
	struct {
		GtkWidget  *mt;
		GtkWidget  *miAbout;
	} help;

};

gboolean gui_menus_refresh( GuiMenus *gm, const Gui *gui );

/* ---------------------------------------------------
 * Disable visually the specified menu-item and
 * enable all the others, in the Language menu.
 * ---------------------------------------------------
 */
static gboolean _disable_lang_mi_as_radio(
	GtkWidget *mi,
	Gui       *gui
	)
{
	/* temp ptrs for better clarity & for saving us some typing later on */
	GuiMenus  *guimenus = gui_get_menus( gui );
	GtkWidget *enmi = NULL;
	GtkWidget *elmi = NULL;
	GtkWidget *osmi = NULL;

/*
	if ( NULL == guimenus->lang.miEnglish ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"Invalid GUI element (menus->lang.miEnglish)"
			);
		return FALSE;
	}
	if ( NULL == guimenus->lang.miGreek ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"Invalid GUI element (menus->lang.miGreek)"
			);
		return FALSE;
	}
	if ( NULL == guimenus->lang.miEnvironment ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"Invalid GUI element (menus->lang.miEnvironment)"
			);
		return FALSE;
	}
*/
	/* just for brevity & better clarity */
	enmi = guimenus->lang.miEnglish;
	elmi = guimenus->lang.miGreek;
	osmi = guimenus->lang.miEnvironment;

	/* first enable all langmenu items */
	gtk_widget_set_sensitive( enmi, TRUE );
	gtk_widget_set_sensitive( elmi, TRUE );
	gtk_widget_set_sensitive( osmi, TRUE );

	/* then disable only the requested item */
	if ( mi == enmi ) {
		gtk_widget_set_sensitive( enmi, FALSE );
	}
	else if ( mi == elmi ) {
		gtk_widget_set_sensitive( elmi, FALSE );
	}
	else if ( mi == osmi ) {
		gtk_widget_set_sensitive( osmi, FALSE );
	}

	return TRUE;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "File->Open".
 * ---------------------------------------------------
 */
static void _on_activate_miFileOpen( GtkWidget *mi, Gui *gui )
{
	GtkWidget *appwind = NULL;
	GtkWidget *dialog  = NULL;

	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	appwind  = gui_get_appWindow( gui );
	if ( NULL == appwind ) {
		DBG_GUI_ERRMSG(
			appwind,
			"Invalid GUI element (gui->appWindow or gui->dlgAbout)"
			);
		return;
	}

	gtk_widget_set_sensitive( appwind, FALSE );

	dialog = gtk_file_chooser_dialog_new(
			"Open File",
			GTK_WINDOW( appwind ),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
			);
	if ( GTK_RESPONSE_ACCEPT == gtk_dialog_run( GTK_DIALOG (dialog) ) ) {
		char *fname;

		fname = gtk_file_chooser_get_filename(
				GTK_FILE_CHOOSER( dialog )
				);
		if ( fname ) {
			/* invalid replay-file */
			if ( !gui_set_gamedata_from_fname(gui, fname) ) {
				mygtk_alert_box(
					gui_get_appWindow(gui),
					TXT_ERR_INVALID_REPLAYFILE
					);
			}
			/* valid replay-file */
			else {
				gui_set_imove( gui, 0 );
//				gui_set_imove( gui, gui_get_gamedata_nmoves(gui)-1 );
				gui_set_hasloadedfile( gui, TRUE );
				gui_resize_board( gui );

				gui_refresh( gui );
			}

		}
		g_free( fname );
	}
	gtk_widget_destroy( dialog );

	gtk_widget_set_sensitive( appwind, TRUE );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Menu->Quit".
 *
 * When the user selects this menu-item we want to
 * terminate the program, so we first set to TRUE
 * the boolean variable: gui->quitOnDestroyAppWindow,
 * and then we call the same callback function we use
 * when the "destroy" signal is sent by the main window
 * of our program.
 *
 * For details about gui->quitOnDestroyAppWindow, please
 * read the comments of the function:
 *	on_destroy_appWindow()
 * ---------------------------------------------------
 */
static void _on_activate_miFileQuit( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	gui_set_quitOnDestroyAppWindow( gui, TRUE );
	on_destroy_appWindow( gui_get_appWindow(gui), gui );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Play".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackPlay( GtkWidget *mi, Gui *gui )
{
	unsigned long delay = 0;

	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

//	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Play" );
	delay = gui_get_gamedata_delay( gui );
	while ( gui_has_imove_next(gui) ) {
		gui_set_imove_next( gui );
		gui_refresh( gui );
		mygtk_widget_redraw( gui_get_board_container(gui) );
		g_usleep( (gulong)delay * 1000 );
	}

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for any radio-menu-item (mi) present in
 * the "Playback->Speed" submenu.
 * ---------------------------------------------------
 */
static void _on_activate_menuradioitem_in_PlaybackSpeed(
	GtkWidget *mi,
	Gui *gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	/* ignore radio-menu-item if it is not currently active */
	if ( !gtk_check_menu_item_get_active( GTK_CHECK_MENU_ITEM(mi) ) ) {
		return;
	}

	mygtk_alert_box(
		gui_get_appWindow(gui),
		gtk_menu_item_get_label( GTK_MENU_ITEM(mi) )
		);
	// mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Speed -> Reset" );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Pause".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackPause( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Pause" );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Stop".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackStop( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Stop" );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->First".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackFirst( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

//	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> First" );
	if ( !gui_set_imove(gui, 0) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui_set_imove(0) failed!"
			);
		return;
	}

	gui_refresh( gui );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Previous".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackPrevious( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

//	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Previous" );
	if ( !gui_set_imove_prev(gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui_set_imove_prev() failed!"
			);
		return;
	}

	gui_refresh( gui );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Next".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackNext( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

//	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Next" );
	if ( !gui_set_imove_next(gui) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui_set_imove_next() failed!"
			);
		return;
	}

	gui_refresh( gui );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Last".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackLast( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

//	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Last" );
	if ( !gui_set_imove(gui, gui_get_gamedata_nmoves(gui)-1) ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"gui_set_imove(last) failed!"
			);
		return;
	}

	gui_refresh( gui );


	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Playback->Jump to".
 *
 * NOTE: Public, because also used by the playback-toolbar
 * ---------------------------------------------------
 */
void on_activate_miPlaybackJumpto( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	mygtk_alert_box( gui_get_appWindow(gui), "Playback -> Jump to" );

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->English".
 *
 * The menu-item is visually disabled, LANG is set
 * to "en" and the whole GUI is reloaded in order
 * to reflect the change of the language.
 *
 * NOTE:
 * 	To better understand how this works,
 *	see also: gui_reload_from_glade_file().
 * ---------------------------------------------------
 */
static void _on_activate_miLanguageEnglish( GtkWidget *mi, Gui *gui )
{
	GuiLocale *guilocale = gui_get_locale( gui );
	GuiMenus  *guimenus  = gui_get_menus( gui );

	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( NULL == gui_get_appWindow(gui) ) {
		DBG_GUI_ERRMSG(
			NULL,
			"Invalid GUI element (gui->appWindow)"
			);
		return;
	}

	/* set English locale environment, reload GUI resources, update menu-entries */
	gui_locale_apply_en( guilocale );
	gui_reload_from_glade_file( gui, FNAME_GLADE );
	_disable_lang_mi_as_radio(
		guimenus->lang.miEnglish,
		gui
		);
	gtk_widget_show_all( gui_get_appWindow(gui) );

	dbg_gui_locale_print_current(
		guilocale,
		"Locale Environment changed to English:"
		);

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->Greek".
 *
 * The menu-item is visually disabled, LANG is set
 * to "el" and the whole GUI is reloaded in order
 * to reflect the change of the language.
 *
 * NOTE:
 *	To better understand how this works,
 *	see also: gui_reload_from_glade_file().
 * ---------------------------------------------------
 */
static void _on_activate_miLanguageGreek( GtkWidget *mi, Gui *gui )
{
	GuiLocale *guilocale = gui_get_locale( gui );
	GuiMenus  *guimenus  = gui_get_menus( gui );

	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( NULL == gui_get_appWindow(gui) ) {
		DBG_GUI_ERRMSG(
			NULL,
			"Invalid GUI element (gui->appWindow)"
			);
		return;
	}


	/* set Greek locale environment, reload GUI resources, update menu-entries */
	gui_locale_apply_el( guilocale );
	gui_reload_from_glade_file(gui, FNAME_GLADE);
	_disable_lang_mi_as_radio(
		guimenus->lang.miGreek,
		gui
		);
	gtk_widget_show_all( gui_get_appWindow(gui) );

	dbg_gui_locale_print_current(
		guilocale,
		"Locale Environment changed to Greek:"
		);

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->use Environment".
 *
 * The menu-item is visually disabled, LANG is set to
 * the value saved in gui->locale->os.varLang (when the
 * program started), and the whole GUI is reloaded in
 * order to reflect the change of the language.
 *
 * NOTE:
 *	If the saved value of LANG was NULL then we set
 *	it to "en" and we inform the user.
 *
 *	If the saved value of LANG defines a language
 *	with no translation available then GNU-gettext
 *	ensures that it will fallback to the default
 *	language, which is English.
 *
 *	In that case, the user is not directly informed,
 *	but he gets a visual hint because the menu-item
 *	stays disabled inside the menu.
 *
 *	To better understand how this works,
 *	see also: gui_reload_from_glade_file().
 * ---------------------------------------------------
 */
static void _on_activate_miLanguageEnvironment( GtkWidget *mi, Gui *gui )
{
	GuiLocale *guilocale = gui_get_locale( gui );
	GuiMenus  *guimenus  = gui_get_menus( gui );

	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( NULL == gui_get_appWindow(gui) ) {
		DBG_GUI_ERRMSG( NULL, "Invalid GUI element (gui->appWindow)" );
		return;
	}

	gui_locale_apply_osenv( guilocale );
	gui_reload_from_glade_file( gui, FNAME_GLADE );

	_disable_lang_mi_as_radio(
		guimenus->lang.miEnvironment,
		gui
		);
	gtk_widget_show_all( gui_get_appWindow(gui) );

	dbg_gui_locale_print_current(
		guilocale,
		"Locale Environment changed to System's:"
		);

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Menu->About".
 *
 * It Displays the "About" dialog, which is defined as a
 * GtkAboutDialog window in the Glade file.
 *
 * This makes it easier to present the desired information
 * in a standardized manner, but I haven't found yet an easy
 * way to access the members of the dialog's action-area,
 * that is the buttons: "Credits", "License" and "Close".
 *
 * Perhaps someday I dig deeper into the internals of the
 * GtkAboutDialog type, but right now I see no easy way to
 * mark the labels of those buttons as translatable by GNU-gettext.
 *
 * Furthermore, when running on Win32 OSes, GTK+ has trouble
 * registering properly some common hyperlink-protocols,
 * such as "http://" & "mailto:". The problem is that the
 * About dialog contains quite a few of them.
 *
 * To solve this problem, I overwrite GTK+'s default behaviour
 * for links in this dialog, by connecting the callback function:
 * _on_activate_link_dlgAbout() to the "activate-link" signals
 * emitted by the dialog.
 *
 * The callback function is used only when Win32 is the compilation
 * target, regulated by a relative pre-processor directive.
 *
 * For more info, please see the functions:
 *	gui_init_dlgAbout()
 *	_on_activate_link_dlgAbout()
 * ---------------------------------------------------
 */
static void _on_activate_miAbout( GtkWidget *mi, Gui *gui )
{
	GtkWidget *appwind  = NULL;
	GtkWidget *dlgabout = NULL;

	/* avoid compiler warning for unused parameter */
	if ( NULL == mi ) {
		return;
	}

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	appwind  = gui_get_appWindow( gui );
	dlgabout = gui_get_dlgAbout( gui );
	if ( NULL == appwind || NULL == dlgabout ){
		DBG_GUI_ERRMSG(
			appwind,
			"Invalid GUI element (gui->appWindow or gui->dlgAbout)"
			);
		return;
	}

	gtk_widget_set_sensitive( appwind, FALSE );

	gtk_dialog_run( GTK_DIALOG( dlgabout ) );
	gtk_widget_hide( GTK_WIDGET( dlgabout ) );

	gtk_widget_set_sensitive( appwind, TRUE );

	return;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiMenus *gui_menus_free( GuiMenus *gm )
{
	if ( gm ) {
		free( gm );
	}
	return NULL;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiMenus *make_gui_menus( void )
{
	GuiMenus *gm = calloc( 1, sizeof(*gm) );
	if ( NULL == gm ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}

	return gm;
}

/* ---------------------------------------------------
 * Initialize the File menu
 * ---------------------------------------------------
 */
static inline void _init_file_menu(
	GuiMenus   *menus,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */

	/* menu-title: File */
	pw  = &menus->file.mt;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "mtFile") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: File -> Open */
	pw  = &menus->file.miOpen;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miFileOpen") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_miFileOpen ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: File -> Quit */
	pw  = &menus->file.miQuit;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miFileQuit") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_miFileQuit ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);
}

/* ---------------------------------------------------
 * Initialize the Playback->Speed submenu
 * ---------------------------------------------------
 */
static inline void _init_playback_speed_submenu(
	GuiMenus   *menus,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */

	/* submenu-item-radio: Playback -> Speed -> Reset */
	pw  = &menus->playback.speed.mirReset;
	*pw = GTK_WIDGET(
		gtk_builder_get_object( builder, "mirPlaybackSpeedReset" )
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_menuradioitem_in_PlaybackSpeed ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* submenu-item-radio: Playback -> Speed -> Slower */
	pw  = &menus->playback.speed.mirSlower;
	*pw = GTK_WIDGET(
		gtk_builder_get_object( builder, "mirPlaybackSpeedSlower" )
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_menuradioitem_in_PlaybackSpeed ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* submenu-item-radio: Playback -> Speed -> Faster */
	pw  = &menus->playback.speed.mirFaster;
	*pw = GTK_WIDGET(
		gtk_builder_get_object( builder, "mirPlaybackSpeedFaster" )
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_menuradioitem_in_PlaybackSpeed ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);
}

/* ---------------------------------------------------
 * Initialize the Playback menu
 * ---------------------------------------------------
 */
static inline void _init_playback_menu(
	GuiMenus   *menus,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */

	/* menu-title: Playback */
	pw  = &menus->playback.mt;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "mtPlayback") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Speed */
	pw  = &menus->playback.miSpeed;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackSpeed") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Play */
	pw  = &menus->playback.miPlay;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackPlay") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackPlay ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Pause */
	pw  = &menus->playback.miPause;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackPause") );
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackPause ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Stop */
	pw  = &menus->playback.miStop;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackStop") );
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackStop ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> First */
	pw  = &menus->playback.miFirst;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackFirst") );
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackFirst ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Previous */
	pw  = &menus->playback.miPrevious;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackPrevious") );
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackPrevious ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Next */
	pw  = &menus->playback.miNext;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackNext") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackNext ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Last */
	pw  = &menus->playback.miLast;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackLast") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackLast ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Playback -> Jump to */
	pw  = &menus->playback.miJumpto;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miPlaybackJumpto") );
	gtk_menu_item_set_label(
		GTK_MENU_ITEM( *pw ),
		TXT_MENUITEM_PLAYBACK_JUMPTO
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( on_activate_miPlaybackJumpto ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

}

/* ---------------------------------------------------
 * Initialize the Language menu
 * ---------------------------------------------------
 */
static inline void _init_language_menu(
	GuiMenus   *menus,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	/* just for brevity later on */
	GtkWidget **pw = NULL;        /* pointer to a widget pointer */

	/* menu-title: Language */
	pw  = &menus->lang.mt;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "mtLang") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu: Lang */
	pw  = &menus->lang.mn;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "mnLang") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Language -> English */
	pw  = &menus->lang.miEnglish;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miLangEnglish") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_miLanguageEnglish ),
		gui
	);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Language -> Greek */
	pw  = &menus->lang.miGreek;
	*pw = GTK_WIDGET( gtk_builder_get_object(builder, "miLangGreek") );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_miLanguageGreek ),
		gui
	);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);

	/* menu-item: Language -> use Environment */
	pw  = &menus->lang.miEnvironment;
	*pw = GTK_WIDGET(gtk_builder_get_object(builder, "miLangEnvironment"));
	gtk_widget_set_sensitive( *pw, FALSE );
	g_signal_connect(
		G_OBJECT( *pw ),
		"activate",
		G_CALLBACK( _on_activate_miLanguageEnvironment ),
		gui
		);
	g_signal_connect(
		G_OBJECT( *pw ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		pw
		);
}

/* ---------------------------------------------------
 * Initialize the Help menu
 * ---------------------------------------------------
 */
static inline void _init_help_menu(
	GuiMenus   *menus,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	/* menu-item: Help -> About */
	menus->help.miAbout
		= GTK_WIDGET( gtk_builder_get_object(builder, "miHelpAbout") );
	g_signal_connect(
		G_OBJECT( menus->help.miAbout ),
		"activate",
		G_CALLBACK( _on_activate_miAbout ),
		gui
		);
	g_signal_connect(
		G_OBJECT( menus->help.miAbout ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&menus->help.miAbout
		);
}

/* ---------------------------------------------------
 * Initialize the menus of the program.
 *
 * Copies the menu widgets from the specified GTK+ builder
 * into my GUI abstraction: gui->menu (GuiMenus), it connects
 * callback functions to them and it initializes their visual
 * appearance.
 *
 * NOTE:
 *	The GTK+ callback function: gtk_widget_destroyed()
 *	ensures that the widget pointer will be set to NULL
 *	after the widget gets destroyed.
 * ---------------------------------------------------
 */
gboolean gui_menus_init_from_builder(
	GuiMenus    *menus,
	GtkBuilder  *builder,
	Gui         *gui
	)
{
	if ( NULL == menus ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (menus)" );
		return FALSE;
	}
	if ( NULL == builder ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow(gui),
			"NULL pointer argument (builder)"
			);
		return FALSE;
	}
	if ( NULL == gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return FALSE;
	}

	_init_file_menu( menus, builder, gui );
	_init_playback_menu( menus, builder, gui );
	_init_playback_speed_submenu( menus, builder, gui );
	_init_language_menu( menus, builder, gui );
	_init_help_menu( menus, builder, gui );

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_menus_get_mirPlaybackSpeedReset( GuiMenus *gm )
{
	if ( NULL == gm ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gm)"
			);
		return NULL;
	}

	return gm->playback.speed.mirReset;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_menus_get_mirPlaybackSpeedSlower( GuiMenus *gm )
{
	if ( NULL == gm ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gm)"
			);
		return NULL;
	}

	return gm->playback.speed.mirSlower;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GtkWidget *gui_menus_get_mirPlaybackSpeedFaster( GuiMenus *gm )
{
	if ( NULL == gm ) {
		DBG_GUI_ERRMSG(
			NULL,
			"NULL pointer argument (gm)"
			);
		return NULL;
	}

	return gm->playback.speed.mirFaster;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean gui_menus_refresh( GuiMenus *gm, const Gui *gui )
{
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gm)" );
		return FALSE;
	}
	if ( NULL == gm ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gm)"
		);
	}

	/* Playback menu */

	/* when no replay-file is loaded */
	if ( !gui_get_hasloadedfile(gui) ) {
		gtk_widget_set_sensitive( gm->playback.mt, FALSE );
		return TRUE;
	}
	gtk_widget_set_sensitive( gm->playback.mt, TRUE );

	/* miFirst & miPrevious */
	gtk_widget_set_sensitive( gm->playback.miPrevious, TRUE );
	if ( gui_has_imove_prev(gui) ) {
		gtk_widget_set_sensitive( gm->playback.miFirst, TRUE );
		gtk_widget_set_sensitive( gm->playback.miPrevious, TRUE );
	}
	else {
		gtk_widget_set_sensitive( gm->playback.miFirst, FALSE );
		gtk_widget_set_sensitive( gm->playback.miPrevious, FALSE );
	}

	/* miNext & miLast */
	if ( gui_has_imove_next(gui) ) {
		gtk_widget_set_sensitive( gm->playback.miNext, TRUE );
		gtk_widget_set_sensitive( gm->playback.miLast, TRUE );
	}
	else {
		gtk_widget_set_sensitive( gm->playback.miNext, FALSE );
		gtk_widget_set_sensitive( gm->playback.miLast, FALSE );
	}

	/* miJumpto */
	if ( gui_is_imove_valid(gui)
	&& gui_get_imove(gui) > -1
	&& gui_get_gamedata_nmoves(gui) > 1
	){
		gtk_widget_set_sensitive( gm->playback.miJumpto, TRUE );
	}
	else {
		gtk_widget_set_sensitive( gm->playback.miJumpto, FALSE );
	}

	return TRUE;
}

