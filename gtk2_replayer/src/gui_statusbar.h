/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * --------------------------------------------------------------
 *
 * The public interface of the GuiStatusbar "class".
 ****************************************************************
 */

#ifndef GUI_STATUSBAR_H
#define GUI_STATUSBAR_H

#include <gtk/gtk.h>    /* GtkBuilder */

/* Forward declaration of the GuiStatusbar "class" as an opaque data-type. */
typedef struct _GuiStatusbar GuiStatusbar;

#ifndef GUI_STATUSBAR_C
extern GuiStatusbar *make_gui_statusbar( void );              /* constructor */
extern GuiStatusbar *gui_statusbar_free( GuiStatusbar *gsb ); /* destructor */
extern gboolean     gui_statusbar_init_from_builder(          /* initializer */
			GuiStatusbar *gsb,
			GtkBuilder   *builder,
			const void   *gui
			);

/* getters */
//extern GtkWidget    *gui_statusbar_get_widget( GuiStatusbar *gsb );
//extern guint        gui_statusbar_get_contextId( GuiStatusbar *gsb );

/* utilities */
extern gboolean     gui_statusbar_refresh(
			GuiStatusbar *gsb,
			const void   *gui
			);
#endif

#endif