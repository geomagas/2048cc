#ifndef GUI_MENUS_H
#define GUI_MENUS_H

#include <gtk/gtk.h>

typedef struct _GuiMenus GuiMenus;

#ifndef GUI_MENUS_C
extern GuiMenus *new_gui_menus( void );
extern GuiMenus *gui_menus_free( GuiMenus *gm );

extern gboolean  gui_menus_init(
			GuiMenus    *menus,
			GtkBuilder  *builder,
			void         *gui
			);
#endif

#endif
