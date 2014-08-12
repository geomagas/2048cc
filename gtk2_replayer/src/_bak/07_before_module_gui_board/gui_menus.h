#ifndef GUI_MENUS_H
#define GUI_MENUS_H

#include <gtk/gtk.h>

typedef struct _GuiMenus GuiMenus;

#ifndef GUI_MENUS_C
extern GuiMenus *make_gui_menus( void );
extern GuiMenus *gui_menus_free( GuiMenus *gm );

extern gboolean  gui_menus_init(
			GuiMenus    *menus,
			GtkBuilder  *builder,
			void        *gui
			);
extern GtkWidget *gui_menus_get_mirPlaybackSpeedReset( GuiMenus *gm );
extern GtkWidget *gui_menus_get_mirPlaybackSpeedSlower( GuiMenus *gm );
extern GtkWidget *gui_menus_get_mirPlaybackSpeedFaster( GuiMenus *gm );

extern void on_activate_miPlaybackPlay( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackPause( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackStop( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackFirst( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackPrevious( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackNext( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackLast( GtkWidget *mi, void *gui );
extern void on_activate_miPlaybackJumpto( GtkWidget *mi, void *gui );
#endif

#endif
