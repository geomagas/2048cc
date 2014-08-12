#ifndef GUI_MENUS_H
#define GUI_MENUS_H

#include <gtk/gtk.h>

#ifndef GUI_MENUS_C
typedef struct _GuiMenus GuiMenus;

extern GuiMenus *new_gui_menus( void );
extern GuiMenus *gui_menus_free( GuiMenus *gm );

extern gboolean  gui_menus_init(
			GuiMenus    *menus,
			GtkBuilder  *builder,
			void         *gui
			);
/*
extern void on_activate_miQuit( GtkWidget *mi, Gui *gui );
extern void on_activate_miEnglish( GtkWidget *mi, Gui *gui );
extern void on_activate_miGreek( GtkWidget *mi, Gui *gui );
extern void on_activate_miEnvironment( GtkWidget *mi, Gui *gui );
extern void on_activate_miAbout( GtkWidget *mi, Gui *gui );
*/

#endif

#endif
