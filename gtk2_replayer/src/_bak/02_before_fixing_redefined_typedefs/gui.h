#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "gui_locale.h"
//#include "gui_menus.h"

#define DIR_GUI            "gui/"    /* gui resources */
#define FNAME_APPICON      DIR_GUI"gtk2_viewer.png"
#define FNAME_APPLOGO      FNAME_APPICON
#define FNAME_GLADE        DIR_GUI"gtk2_viewer.glade"

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

#define TXTF_STATUSBAR_ONROLL     _(" Spin: %03d | Frame: %03d | Delay: %06ld")
#define TXTF_STATUSBAR_RESULT     _(" Result: %d")


//#ifndef GUI_MENUS_C
typedef struct _GuiMenus GuiMenus;
//#endif

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
	GuiMenus      *menus;
//	GuiSettings   settings;
	GuiStatusbar  statusbar;
//	Core          *linkToCoreData;
}Gui;

#ifndef GUI_C
extern Gui      *new_gui( int *argc, char ***argv/*, const Core  *core*/ );
extern Gui      *gui_free( Gui *gui );
extern gboolean gui_start( Gui *gui );
extern gboolean gui_reload_from_glade_file( Gui *gui, const gchar *fnameGlade );

extern void     on_destroy_appWindow( GtkWidget *appWindow, Gui *gui );
#endif

#endif
