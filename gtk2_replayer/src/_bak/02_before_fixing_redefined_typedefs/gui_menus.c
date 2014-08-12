#define GUI_MENUS_C

/* Prefix naming conventions:
 * mbar: menu-bar
 * mt  : menu-title
 * mn  : menu
 * mi  : menu-item
 * mir : menu-item-radio
 */
#include <gtk/gtk.h>
#include <stdlib.h>

#include "misc.h"
#include "gui_menus.h"
#include "gui.h"

/* a bit more convenient GUI abstraction of the menus */
typedef struct _GuiMenus GuiMenus;
struct _GuiMenus
{
	struct {
		GtkWidget  *mt;
		GtkWidget  *miQuit;
	} file;

	struct {
		GtkWidget  *mt;
		GtkWidget  *mn;
		GtkWidget  *miEnglish;
		GtkWidget  *miGreek;
		GtkWidget  *miEnvironment;
	} lang;

	struct {
		GtkWidget  *mt;
		GtkWidget  *miSpeed;
		struct {
			GtkWidget *submenu;
			GtkWidget *mirRecorded;
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

	struct {
		GtkWidget  *mt;
		GtkWidget  *miAbout;
	} help;

};

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
GuiMenus *new_gui_menus( void )
{
	GuiMenus *gm = calloc( 1, sizeof(*gm) );
	if ( NULL == gm ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}

	return gm;
}

/* ---------------------------------------------------
 * Disable visually the specified menu-item and
 * enable all the others, in the Language menu.
 * ---------------------------------------------------
 */
static gboolean _disable_lang_mi_as_radio(
	Gui       *gui,
	GtkWidget *mi
	)
{
	/* temp ptrs for better clarity & for saving us some typing later on */
	GtkWidget *enmi = NULL;
	GtkWidget *elmi = NULL;
	GtkWidget *osmi = NULL;

	if ( !gui->menus->lang.miEnglish ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"Invalid GUI element (menus->lang.miEnglish)"
			);
		return FALSE;
	}
	if ( !gui->menus->lang.miGreek ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"Invalid GUI element (menus->lang.miGreek)"
			);
		return FALSE;
	}
	if ( !gui->menus->lang.miEnvironment ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"Invalid GUI element (menus->lang.miEnvironment)"
			);
		return FALSE;
	}

	/* just for brevity & better clarity */
	enmi = gui->menus->lang.miEnglish;
	elmi = gui->menus->lang.miGreek;
	osmi = gui->menus->lang.miEnvironment;

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
void on_activate_miQuit( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !mi ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}

	gui->quitOnDestroyAppWindow = TRUE;
	on_destroy_appWindow( gui->appWindow, gui );

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
void on_activate_miEnglish( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !mi ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"Invalid GUI element (gui->appWindow)"
			);
		return;
	}

	/* set English locale environment, reload GUI resources, update menu-entries */
	gui_locale_apply_en( gui->locale );
	gui_reload_from_glade_file( gui, FNAME_GLADE );
	_disable_lang_mi_as_radio( gui, gui->menus->lang.miEnglish );
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
 *	see also: gui_reload_from_glade_file().
 * ---------------------------------------------------
 */
void on_activate_miGreek( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !mi ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"Invalid GUI element (gui->appWindow)"
			);
		return;
	}


	/* set Greek locale environment, reload GUI resources, update menu-entries */
	gui_locale_apply_el( gui->locale );
	gui_reload_from_glade_file(gui, FNAME_GLADE);
	_disable_lang_mi_as_radio( gui, gui->menus->lang.miGreek );
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
void on_activate_miEnvironment( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !mi ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( !gui->appWindow ) {
		DBG_GUI_ERRMSG( NULL, "Invalid GUI element (gui->appWindow)" );
		return;
	}

	gui_locale_apply_osenv( gui->locale );
	gui_reload_from_glade_file(gui, FNAME_GLADE);

	_disable_lang_mi_as_radio( gui, gui->menus->lang.miEnvironment );
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
void on_activate_miAbout( GtkWidget *mi, Gui *gui )
{
	/* avoid compiler warning for unused parameter */
	if ( !mi ) {
		return;
	}

	/* sanity checks */
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (gui)" );
		return;
	}
	if ( !gui->appWindow || !gui->dlgAbout ) {
		DBG_GUI_ERRMSG(
			gui->appWindow,
			"Invalid GUI element (gui->appWindow or gui->dlgAbout)"
			);
		return;
	}

	gtk_widget_set_sensitive( gui->appWindow, FALSE );

	gtk_dialog_run( GTK_DIALOG(gui->dlgAbout) );
	gtk_widget_hide( GTK_WIDGET(gui->dlgAbout) );

	gtk_widget_set_sensitive( gui->appWindow, TRUE );

	return;
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
	/* menu-title: File */
	menus->file.mt =
		GTK_WIDGET( gtk_builder_get_object(builder, "mtFile") );
	g_signal_connect(
		G_OBJECT( menus->file.mt ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&menus->file.mt
		);

	/* menu-item: File -> Quit */
	menus->file.miQuit =
		GTK_WIDGET( gtk_builder_get_object(builder, "miFileQuit") );
	g_signal_connect(
		G_OBJECT( menus->file.miQuit ),
		"activate",
		G_CALLBACK( on_activate_miQuit ),
		gui
	);
	g_signal_connect(
		G_OBJECT( menus->file.miQuit ),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&menus->file.miQuit
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
	/* menu-title: Language */
	menus->lang.mt
	= GTK_WIDGET( gtk_builder_get_object(builder, "mtLang") );

	g_signal_connect(
		G_OBJECT( menus->lang.mt ),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&menus->lang.mt
		);

	/* menu: Lang */
	menus->lang.mn
	= GTK_WIDGET( gtk_builder_get_object(builder, "mnLang") );

	g_signal_connect(
		G_OBJECT( menus->lang.mn ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&menus->lang.mn
		);

	/* menu-item: Language -> English */
	menus->lang.miEnglish
	= GTK_WIDGET( gtk_builder_get_object(builder, "miLangEnglish") );

	g_signal_connect(
		G_OBJECT( menus->lang.miEnglish ),
		"activate",
		G_CALLBACK( on_activate_miEnglish ),
		gui
	);
	g_signal_connect(
		G_OBJECT( menus->lang.miEnglish ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&menus->lang.miEnglish
		);

	/* menu-item: Language -> Greek */
	menus->lang.miGreek
	= GTK_WIDGET( gtk_builder_get_object(builder, "miLangGreek") );

	g_signal_connect(
		G_OBJECT( menus->lang.miGreek ),
		"activate",
		G_CALLBACK( on_activate_miGreek ),
		gui
	);
	g_signal_connect(
		G_OBJECT( menus->lang.miGreek ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&menus->lang.miGreek
		);

	/* menu-item: Language -> use Environment */
	menus->lang.miEnvironment
	= GTK_WIDGET( gtk_builder_get_object(builder, "miLangEnvironment") );

	g_signal_connect(
		G_OBJECT( menus->lang.miEnvironment ),
		"activate",
		G_CALLBACK( on_activate_miEnvironment ),
		gui
	);
	g_signal_connect(
		G_OBJECT( menus->lang.miEnvironment ),
		"destroy",
		G_CALLBACK( gtk_widget_destroyed ),
		&menus->lang.miEnvironment
		);
	gtk_widget_set_sensitive( menus->lang.miEnvironment, FALSE );
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
		G_CALLBACK( on_activate_miAbout ),
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
gboolean gui_menus_init(
	GuiMenus    *menus,
	GtkBuilder  *builder,
	Gui         *gui
	)
{
	if ( NULL == menus ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (menus)" );
		return FALSE;
	}
	if ( !builder ) {
		DBG_GUI_ERRMSG(
			gui->appWindow, "NULL pointer argument (builder)"
			);
		return FALSE;
	}
	if ( !gui ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument (gui)" );
		return FALSE;
	}

#if 1
	_init_file_menu( gui->menus, builder, gui );
	_init_language_menu( gui->menus, builder, gui );
	_init_help_menu( gui->menus, builder, gui );
#else
	/*
	 * the File menu
	 */

	/* menu-title: File */
	gui->menus->file.mt =
		GTK_WIDGET( gtk_builder_get_object(builder, "mtFile") );
	g_signal_connect(
		G_OBJECT(gui->menus->file.mt),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->file.mt
		);

	/* menu-item: File -> Quit */
	gui->menus->file.miQuit =
		GTK_WIDGET( gtk_builder_get_object(builder, "miFileQuit") );
	g_signal_connect(
		G_OBJECT(gui->menus->file.miQuit),
		"activate",
		G_CALLBACK(on_activate_miQuit),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menus->file.miQuit),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->file.miQuit
		);

	/*
	 * the Language menu
	 */

	/* menu-title: Language */
	gui->menus->lang.mt =
		GTK_WIDGET( gtk_builder_get_object(builder, "mtLang") );
	g_signal_connect(
		G_OBJECT(gui->menus->lang.mt),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->lang.mt
		);

	/* menu: Lang */
	gui->menus->lang.mn =
		GTK_WIDGET( gtk_builder_get_object(builder, "menuLang") );
	g_signal_connect(
		G_OBJECT(gui->menus->lang.mn),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->lang.mn
		);

	/* menu-item: Language -> English */
	gui->menus->lang.miEnglish =
		GTK_WIDGET( gtk_builder_get_object(builder, "miLangEnglish") );
	g_signal_connect(
		G_OBJECT(gui->menus->lang.miEnglish),
		"activate",
		G_CALLBACK(on_activate_miEnglish),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menus->lang.miEnglish),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->lang.miEnglish
		);

	/* menu-item: Language -> Greek */
	gui->menus->lang.miGreek =
		GTK_WIDGET( gtk_builder_get_object(builder, "miLangGreek") );
	g_signal_connect(
		G_OBJECT(gui->menus->lang.miGreek),
		"activate",
		G_CALLBACK(on_activate_miGreek),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menus->lang.miGreek),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->lang.miGreek
		);

	/* menu-item: Language -> use Environment */
	gui->menus->lang.miEnvironment
	= GTK_WIDGET( gtk_builder_get_object(builder, "miLangEnvironment") );

	g_signal_connect(
		G_OBJECT(gui->menus->lang.miEnvironment),
		"activate",
		G_CALLBACK(on_activate_miEnvironment),
		gui
	);
	g_signal_connect(
		G_OBJECT(gui->menus->lang.miEnvironment),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->lang.miEnvironment
		);
	gtk_widget_set_sensitive( gui->menus->lang.miEnvironment, FALSE );

	/*
	 * the Help menu
	 */

	/* menu-item: Help -> About */
	gui->menus->help.miAbout =
		GTK_WIDGET( gtk_builder_get_object(builder, "miHelpAbout") );
	g_signal_connect(
		G_OBJECT(gui->menus->help.miAbout),
		"activate",
		G_CALLBACK(on_activate_miAbout),
		gui
		);
	g_signal_connect(
		G_OBJECT(gui->menus->help.miAbout),
		"destroy",
		G_CALLBACK(gtk_widget_destroyed),
		&gui->menus->help.miAbout
		);
#endif

	return TRUE;
}