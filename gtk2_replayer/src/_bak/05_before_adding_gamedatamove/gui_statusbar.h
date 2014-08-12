#ifndef GUI_STATUSBAR_H
#define GUI_STATUSBAR_H

#include <gtk/gtk.h>

typedef struct _GuiStatusbar GuiStatusbar;

#ifndef GUI_STATUSBAR_C
extern GuiStatusbar *new_gui_statusbar( void );
extern GuiStatusbar *gui_statusbar_free( GuiStatusbar *gsb );

extern gboolean     gui_statusbar_init_from_builder(
			GuiStatusbar *gsb,
			GtkBuilder   *builder,
			const void   *gui
			);
extern GtkWidget    *gui_statusbar_get_widget( GuiStatusbar *gsb );
extern guint        gui_statusbar_get_contextId( GuiStatusbar *gsb );

extern gboolean     gui_statusbar_refresh_txtout(
			GuiStatusbar *gsb,
			const void   *gui
			);
#endif

#endif