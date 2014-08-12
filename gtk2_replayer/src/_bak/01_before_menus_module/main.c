/* -------------------------------
 * Preprocessor
 * -------------------------------
 */

#include <glib/gi18n.h>

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>

#include "misc.h"
#include "gui_locale.h"


/* we need this for opening http:// and mailto: links when OS is Windows */
#ifdef G_OS_WIN32
#include <windows.h>
#endif

/* some handy constants */

#define MAXSIZ_FNAME              (255+1)

#define DIR_GUI                   "gui/"    /* gui resources */
#define FNAME_APPICON             DIR_GUI"gtk2_viewer.png"
#define FNAME_APPLOGO             FNAME_APPICON
#define FNAME_GLADE               DIR_GUI"gtk2_viewer.glade"

/* GUI strings that are dynamically added/updated,
 * regardless their initial values specified in
 * the Glade file: FNAME_GLADE.
 * Only those enclosed in _() are translatable.
 */
#define TXT_APP_NAME              _("2048cc GTK2 Viewer")
#define TXT_APP_VERSION           "0.3a3"
#define TXT_APP_DESCRIPTION       _("Replay viewer for the 2048cc game")
#define TXT_APP_COPYRIGHT         "copyright (c) 2014 migf1"
#define TXT_APP_LICENSE           _("Free for all")
#define TXT_APP_WEBSITE_LABEL     "x-karagiannis.gr/prg/"
#define TXT_APP_WEBSITE           "http://x-karagiannis.gr/prg/"
#define TXT_APP_AUTHOR            "migf1 <mig_f1@hotmail.com>"
#define TXT_APP_ARTIST            "Napoleon\nhttp://opengameart.org/content/dice-1/"

#define TXT_BUTTON_ROLL2D         _("_Roll")
#define TXT_BUTTON_ROLLING        _("...rolling...")
#define TXTF_STATUSBAR_ONROLL     _(" Spin: %03d | Frame: %03d | Delay: %06ld")
#define TXTF_STATUSBAR_RESULT     _(" Result: %d")

/* -------------------------------
 * Custom DataTypes
 * -------------------------------
 */

/* a bit more convenient GUI abstraction of the menus */
typedef struct GuiMenus
{
	struct {
		GtkWidget  *title;
		GtkWidget  *miQuit;
	} file;

	struct {
		GtkWidget  *title;
		GtkWidget  *menu;
		GtkWidget  *miEnglish;
		GtkWidget  *miGreek;
		GtkWidget  *miEnvironment;
	} lang;

	struct {
		GtkWidget  *title;
		GtkWidget  *mitemSpeed;
		struct {
			GtkWidget *submenu;
			GtkWidget *miRecorded;
			GtkWidget *miSlower;
			GtkWidget *miFaster;
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

	struct {
		GtkWidget  *title;
		GtkWidget  *miAbout;
	} help;

}GuiMenus;

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
typedef struct Gui {
	gboolean      quitOnDestroyAppWindow;
	GuiLocale     *locale;
	GtkWidget     *appWindow;
	GtkWidget     *dlgAbout;
	GuiMenus      menu;
//	GuiSettings   settings;
	GuiStatusbar  statusbar;
//	Core          *linkToCoreData;
}Gui;


/* -------------------------------
 * Global Variables
 * -------------------------------
 */

gboolean global_debugOn;	/* too lazy to wrap it */

/* -------------------------------
 * Function Prototypes (only the bare minimum needed)
 * -------------------------------
 */

static gboolean  gui_reload_gtkGladeFile( Gui *gui, const gchar *fnameGlade );

/* -------------------------------
 * Function Definitions
 * -------------------------------
 */

/* ---------------------------------------------------
 * Disable visually the specified menu-item and
 * enable all the others, in the Language menu.
 * ---------------------------------------------------
 */
static gboolean gui_disable_langmenu_item_as_radio(
	Gui        *gui,
	GtkWidget  *menuItem
	)
{
	/* temp ptrs for better clarity & for saving us some typing later on */
	GtkWidget *en      = NULL;
	GtkWidget *el      = NULL;
	GtkWidget *envLang = NULL;

	/* sanity checks */
	if ( !gui || !menuItem ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui or menuItem)") );
		return FALSE;
	}
	if ( !gui->menu.lang.miEnglish ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->menu.lang.miEnglish)")
			);
		return FALSE;
	}
	if ( !gui->menu.lang.miGreek ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->menu.lang.miGreek)")
			);
		return FALSE;
	}
	if ( !gui->menu.lang.miEnvironment ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->menu.lang.miEnvironment)")
			);
		return FALSE;
	}

	/* just for brevity & better clarity */
	en      = gui->menu.lang.miEnglish;
	el      = gui->menu.lang.miGreek;
	envLang = gui->menu.lang.miEnvironment;

	/* first enable all langmenu items */
	gtk_widget_set_sensitive(en, TRUE);
	gtk_widget_set_sensitive(el, TRUE);
	gtk_widget_set_sensitive(envLang, TRUE);

	/* then disable only the requested item */
	if ( menuItem == en ) {
		gtk_widget_set_sensitive(en, FALSE);
	}
	else if ( menuItem == el ) {
		gtk_widget_set_sensitive(el, FALSE);
	}
	else if ( menuItem == envLang ) {
		gtk_widget_set_sensitive(envLang, FALSE);
	}

	return TRUE;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "delete-event"
 * signal, for the main window of the application.
 *
 * When returing FALSE on a "delete-event" signal, GTK+
 * converts it to a "destroy" signal and re-emits it.
 * Returning TRUE means we don't want the window to be
 * destroyed, which may be useful if for example we want
 * to ask for confirmation before destroying the window.
 *
 * Here we just return FALSE, thus telling GTK+ to
 * immediately emit a "destroy" signal.
 * ---------------------------------------------------
 */
static gboolean on_delete_event_appWindow(
	GtkWidget  *appWindow,
	GdkEvent   *event,
	Gui        *gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( !appWindow || !event || !gui ) {
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
static void on_destroy_appWindow(
	GtkWidget  *appWindow,
	Gui        *gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !appWindow ) {
		gtk_main_quit();
	}

	if ( gui && gui->quitOnDestroyAppWindow ) {
		gtk_main_quit();
	}
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
static void on_activate_miQuit(
	GtkWidget  *menuItem,
	Gui        *gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}

	gui->quitOnDestroyAppWindow = TRUE;
	on_destroy_appWindow(gui->appWindow, gui);

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
 *	see also: gui_reload_gtkGladeFile().
 * ---------------------------------------------------
 */
static void on_activate_miEnglish( GtkWidget *menuItem, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->appWindow)")
			);
		return;
	}

	/* set English locale environment, reload GUI resources, update menu-entries */
	gui_locale_apply_en( gui->locale );
	gui_reload_gtkGladeFile( gui, FNAME_GLADE );
	gui_disable_langmenu_item_as_radio( gui, gui->menu.lang.miEnglish );
	gtk_widget_show_all( gui->appWindow );

	dbg_gui_locale_print_current(
		gui->locale,
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
 *	see also: gui_reload_gtkGladeFile().
 * ---------------------------------------------------
 */
static void on_activate_miGreek(
	GtkWidget  *menuItem,
	Gui        *gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->appWindow)")
			);
		return;
	}


	/* set Greek locale environment, reload GUI resources, update menu-entries */
	gui_locale_apply_el( gui->locale );
	gui_reload_gtkGladeFile(gui, FNAME_GLADE);
	gui_disable_langmenu_item_as_radio( gui, gui->menu.lang.miGreek );
	gtk_widget_show_all( gui->appWindow );

	dbg_gui_locale_print_current(
		gui->locale,
		"Locale Environment changed to Greek:"
		);

	return;
}

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "activate"
 * signal, for the menu-item "Language->use Environment".
 *
 * The menu-item is visually disabled, LANG is set to
 * the value saved in gui->envLang (when the program
 * started), and the whole GUI is reloaded in order
 * to reflect the change of the language.
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
 *	see also: gui_reload_gtkGladeFile().
 * ---------------------------------------------------
 */
static void on_activate_miEnvironment(
	GtkWidget *menuItem,
	Gui       *gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid GUI element (gui->appWindow)") );
		return;
	}

	gui_locale_apply_osenv( gui->locale );
	gui_reload_gtkGladeFile(gui, FNAME_GLADE);

	gui_disable_langmenu_item_as_radio( gui, gui->menu.lang.miEnvironment );
	gtk_widget_show_all( gui->appWindow );

	dbg_gui_locale_print_current(
		gui->locale,
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
 * on_activate_link_dlgAbout() to the "activate-link" signals
 * emitted by the dialog.
 *
 * The callback function is used only when Win32 is the compilation
 * target, regulated by a relative pre-processor directive.
 *
 * For more info, please see the functions:
 *	gui_init_dlgAbout()
 *	on_activate_link_dlgAbout()
 * ---------------------------------------------------
 */
static void on_activate_miAbout(
	GtkWidget  *menuItem,
	Gui        *gui
	)
{
	/* avoid compiler warning for unused parameter */
	if ( !menuItem ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return;
	}
	if ( !gui->appWindow || !gui->dlgAbout ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			_("Invalid GUI element (gui->appWindow or gui->dlgAbout)")
			);
		return;
	}

	gtk_widget_set_sensitive( gui->appWindow, FALSE );

	gtk_dialog_run( GTK_DIALOG( gui->dlgAbout ) );
	gtk_widget_hide( GTK_WIDGET(gui->dlgAbout) );

	gtk_widget_set_sensitive( gui->appWindow, TRUE );

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
static gboolean on_activate_link_dlgAbout(
	GtkWidget  *dlgAbout,
	gchar      *uri,
	Gui        *gui
	)
{
	/* avoid compiler warnings for unused parameters */
	if ( !dlgAbout || !uri ) {
		return FALSE;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !gui->dlgAbout ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Found invalid GUI elements") );
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
static gboolean gui_init_appWindow(
	Gui         *gui,
	GtkBuilder  *builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	if ( !gui->appWindow ) {
		gui->appWindow
		= GTK_WIDGET(gtk_builder_get_object(builder, "windMain") );
	}

	gtk_window_set_icon_from_file(
		GTK_WINDOW(gui->appWindow),
		FNAME_APPICON,
		NULL
		);
	gtk_window_set_position( GTK_WINDOW(gui->appWindow), GTK_WIN_POS_CENTER );
	gtk_window_set_title( GTK_WINDOW(gui->appWindow), TXT_APP_NAME );

	g_signal_connect(
		G_OBJECT(gui->appWindow),
		"delete-event",
		G_CALLBACK(on_delete_event_appWindow),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->appWindow),
		"destroy",
		G_CALLBACK(on_destroy_appWindow),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->appWindow),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->appWindow
		);

	return TRUE;
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
static gboolean gui_init_menus(
	Gui         *gui,
	GtkBuilder  *builder
	)
{
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG(
			gui->appWindow, _("Invalid pointer arg (builder)")
			);
		return FALSE;
	}

	/*
	 * the File menu
	 */

	/* menu-title: File */
	gui->menu.file.title =
		GTK_WIDGET( gtk_builder_get_object(builder, "mtitleFile") );
	g_signal_connect(
		G_OBJECT(gui->menu.file.title),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.file.title
		);

	/* menu-item: File -> Quit */
	gui->menu.file.miQuit =
		GTK_WIDGET( gtk_builder_get_object(builder, "mitemFileQuit") );
	g_signal_connect(
		G_OBJECT(gui->menu.file.miQuit),
		"activate",
		G_CALLBACK(on_activate_miQuit),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.file.miQuit),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.file.miQuit
		);

	/*
	 * the Language menu
	 */

	/* menu-title: Language */
	gui->menu.lang.title =
		GTK_WIDGET( gtk_builder_get_object(builder, "mtitleLang") );
	g_signal_connect(
		G_OBJECT(gui->menu.lang.title),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.lang.title
		);

	/* menu: Lang */
	gui->menu.lang.menu =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuLang") );
	g_signal_connect(
		G_OBJECT(gui->menu.lang.menu),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.lang.menu
		);

	/* menu-item: Language -> English */
	gui->menu.lang.miEnglish =
		GTK_WIDGET( gtk_builder_get_object(builder, "mitemLangEnglish") );
	g_signal_connect(
		G_OBJECT(gui->menu.lang.miEnglish),
		"activate",
		G_CALLBACK(on_activate_miEnglish),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.lang.miEnglish),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.lang.miEnglish
		);

	/* menu-item: Language -> Greek */
	gui->menu.lang.miGreek =
		GTK_WIDGET( gtk_builder_get_object(builder, "mitemLangGreek") );
	g_signal_connect(
		G_OBJECT(gui->menu.lang.miGreek),
		"activate",
		G_CALLBACK(on_activate_miGreek),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.lang.miGreek),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.lang.miGreek
		);

	/* menu-item: Language -> use Environment */
	gui->menu.lang.miEnvironment
	= GTK_WIDGET( gtk_builder_get_object(builder, "mitemLangEnvironment") );

	g_signal_connect(
		G_OBJECT(gui->menu.lang.miEnvironment),
		"activate",
		G_CALLBACK(on_activate_miEnvironment),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menu.lang.miEnvironment),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.lang.miEnvironment
		);
	gtk_widget_set_sensitive( gui->menu.lang.miEnvironment, FALSE );

	/*
	 * the Help menu
	 */

	/* menu-item: Help -> About */
	gui->menu.help.miAbout =
		GTK_WIDGET( gtk_builder_get_object(builder, "mitemHelpAbout") );
	g_signal_connect(
		G_OBJECT(gui->menu.help.miAbout),
		"activate",
		G_CALLBACK(on_activate_miAbout),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->menu.help.miAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menu.help.miAbout
		);


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
static gboolean gui_init_statusBar(
	Gui         *gui,
	GtkBuilder  *builder
	)
{
	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	gui->statusbar.widget =
		GTK_WIDGET( gtk_builder_get_object(builder, "statusbar") );

	g_signal_connect(
		G_OBJECT(gui->statusbar.widget),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->statusbar.widget
		);

	gui->statusbar.contextId =
		gtk_statusbar_get_context_id(
			GTK_STATUSBAR(gui->statusbar.widget),
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
static gboolean gui_init_dlgAbout(
	Gui         *gui,
	GtkBuilder  *builder
	)
{
	GdkPixbuf   *logo      = NULL;
	GError      *error     = NULL;
	const gchar *authors[] = {TXT_APP_AUTHOR, NULL};
	const gchar *artists[] = {TXT_APP_ARTIST, NULL};

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (builder)") );
		return FALSE;
	}

	/* load the about-dialog into gui, from the GTK+2 builder */
	gui->dlgAbout = GTK_WIDGET( gtk_builder_get_object(builder, "dlgAbout") );
	g_signal_connect(
		G_OBJECT(gui->dlgAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->dlgAbout
		);

	/* set the about-dialog opacity */
	gtk_window_set_opacity( GTK_WINDOW(gui->dlgAbout), 0.90 );

	/* set the about-dialog logo */
	logo = gdk_pixbuf_new_from_file( FNAME_APPLOGO, &error );
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(gui->dlgAbout), logo );

	/* set the remaining prperties */
	gtk_about_dialog_set_program_name(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_NAME)
		);

	gtk_about_dialog_set_version(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_VERSION)
		);

	gtk_about_dialog_set_comments(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		TXT_APP_DESCRIPTION
		);

	gtk_about_dialog_set_copyright(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_COPYRIGHT)
		);

	gtk_about_dialog_set_wrap_license( GTK_ABOUT_DIALOG(gui->dlgAbout), TRUE);
	gtk_about_dialog_set_license(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		TXT_APP_LICENSE
		);

	gtk_about_dialog_set_authors(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		authors
		);

	gtk_about_dialog_set_documenters(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		authors
		);

	gtk_about_dialog_set_artists(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		artists
		);

	gtk_about_dialog_set_website_label(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
		gettext(TXT_APP_WEBSITE_LABEL)
		);
	gtk_about_dialog_set_website(
		GTK_ABOUT_DIALOG(gui->dlgAbout),
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
		G_CALLBACK(on_activate_link_dlgAbout),
		gui
		);
#endif

	/* ensure that gui->dlgAbout will be set to NULL after it is destroyed */
	g_signal_connect(
		G_OBJECT(gui->dlgAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),	/* provided by GTK+ */
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
static gboolean gui_load_gtkGladeFile(
	Gui          *gui,
	const gchar  *fnameGlade
	)
{
	GtkBuilder *builder = NULL;
	GError     *error   = NULL;

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, _("Invalid pointer arg (gui)") );
		return FALSE;
	}
	if ( !fnameGlade ) {
		DBG_GUI_ERRMSG( gui->appWindow, _("Invalid pointer arg (fnameGlade)") );
		return FALSE;
	}

	/* create a GTK+2 builder with all the widgets defined in the glade-file */
	builder = gtk_builder_new();
	if ( !gtk_builder_add_from_file( builder, fnameGlade, &error ) )
	{
		DBG_GUI_ERRMSG( gui->appWindow, error->message );
		g_warning( "%s: (fatal error) %s", error->message, __func__ );
		g_error_free( error );
		return FALSE;
	}

	/* init GUI using only the needed elements from the GTK+2 builder */

	gui_init_appWindow( gui, builder );
	gui_init_dlgAbout( gui, builder );
	gui_init_menus( gui, builder );
	gui_init_statusBar( gui, builder );

	/* release the GTK+2 builder */
	g_object_unref( G_OBJECT(builder) );

	return TRUE;
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
static gboolean gui_reload_gtkGladeFile(
	Gui         *gui,
	const gchar *fnameGlade
	)
{
	/* sanity check */
	if ( !gui ) {
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
	return gui_load_gtkGladeFile(gui, fnameGlade);
}

/* ---------------------------------------------------
 * Cleanup GUI related stuff.
 * Normally, this function is called before program termination.
 * ---------------------------------------------------
 */
static void gui_cleanup( Gui *gui )
{
	/* sanity check */
	if ( !gui ) {
		return;
	}

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
}

/* ---------------------------------------------------
 * Initialize the Gui abstraction using GTK+2.
 *
 * This function initializes our GUI abstraction (gui)
 * as a GTK+ gui & links it with the program's core-data.
 *
 * NOTE:
 * For details about the fields: gui->envLang
 *	and gui->quitOnDestroyAppWindow, consult
 *	the comments of the following functions:
 *		on_destroy_appWindow()
 *		gui_reload_gtkGladeFile()
 *		on_activate_miEnglish()
 *		on_activate_miGreek()
 *		on_activate_miEnvironment()
 *		on_activate_miQuit()
 * ---------------------------------------------------
 */
static gboolean gui_init(
	Gui         *gui,           /* our (barely) GUI abstraction */
//	const Core  *core,          /* for linking gui to app's core-data */
	int         *argc,          /* needed by GTK+ during initialization */
	char        ***argv,        /* needed by GTK+ during initialization */
	const gchar *fnameGladeFile /* glade-file defining our GTK+ resources */
	)
{
	/* sanity checks */
	if ( !gui || /*!core ||*/ !argc || !argv || !fnameGladeFile ) {
		return FALSE;
	}

	/* initialize GTK+ */
	if ( !gtk_init_check( argc, argv ) ) {
		DBG_STDERR_MSG( "(fatal error) gtk+ not inited!" );
		return FALSE;
	}

	/* ensure gui abstraction starts fresh */
	memset( gui, 0, sizeof(Gui) );

	/* initiate gui's locale environment */
	gui->locale = new_gui_locale();
	if ( NULL == gui->locale ) {
		DBG_STDERR_MSG( "(fatal error) gui->locale not inited!" );
		return FALSE;
	}

	/* initially, destruction of main window results in program-termination */
	gui->quitOnDestroyAppWindow = TRUE;

	/* link gui with the core-data of the program */
//	gui->linkToCoreData = (Core *) core;

	/* load GUI resources from Glade file */
	if ( !gui_load_gtkGladeFile(gui, fnameGladeFile) ) {
		DBG_STDERR_MSG( "(fatal error) gui resources not loaded!" );
		gui->locale = gui_locale_free( gui->locale );
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 * Application entry point.
 * ---------------------------------------------------
 */
int main( int argc, char **argv )
{
//	Core core;      /* core-data of the program */
	Gui  gui;       /* my (barely) abstracted GUI */

	global_debugOn = TRUE;

	/* init core-data */
//	if ( !core_init(&core) ) {
//		DBG_STDERR_MSG( "(fatal error) core data not inited!" );
//		goto cleanup_and_exit_failure;
//	}

	/* init the gui as GTK+2 */
	if ( !gui_init(&gui, /*&core,*/ &argc, &argv, FNAME_GLADE) ) {
		DBG_STDERR_MSG( "(fatal error) gui not inited!" );
		goto cleanup_and_exit_failure;
	}

	/* print debugging info about the locale */
	dbg_gui_locale_print_inherited(
		gui.locale,
		"Caller Locale Environment was:"
		);
	dbg_gui_locale_print_current(
		gui.locale,
		"GUI Locale Environment set to:"
		);

	/* display the main-window of the application, and all its children */
	if ( gui.appWindow ) {
		gtk_widget_show_all( gui.appWindow );
	}

	/* start the GTK+ event loop */
	gtk_main();

	/* cleanup & exit successfully */
	gui_cleanup( &gui );
//	core_cleanup( &core );
	exit( EXIT_SUCCESS );

cleanup_and_exit_failure:

	/* cleanup & exit due to fatal error */
	gui_cleanup( &gui );
//	core_cleanup( &core );
	exit( EXIT_FAILURE );
}
