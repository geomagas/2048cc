/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: gtk.h
 * --------------------------------------------------------------
 * 
 * For generic guidelines regarding the sources of this project
 * (including the coding-style) please visit the following link:
 * https://github.com/geomagas/2048cc/blob/dev_gtk2_replayer/gtk2_replayer/src/BROWSING.md
 *
 * --------------------------------------------------------------
 * The public interface of the GuiDialogs "class".
 ****************************************************************
 */

#ifndef GUI_DIALOGS_H
#define GUI_DIALOGS_H

#include <gtk/gtk.h>    /* GtkBuilder */

/* Forward declaration of the GuiDialogs "class" as an opaque data-type. */
typedef struct _GuiDialogs GuiDialogs;

#ifndef GUI_DIALOGS_C
extern GuiDialogs  *make_gui_dialogs( void );                /* constructor */
extern GuiDialogs  *gui_dialogs_free( GuiDialogs *dialogs ); /* destructor */
extern gboolean    gui_dialogs_init_from_builder(            /* initializer */
                         GuiDialogs  *dialogs,
                         GtkBuilder  *builder,
                         void        *gui
                         );
/* getters */
extern GtkWidget   *gui_dialogs_get_about_root( GuiDialogs *dialogs );

extern GtkWidget   *gui_dialogs_get_jumpto_root( GuiDialogs *dialogs );
extern GtkWidget   *gui_dialogs_get_jumpto_lblRange( GuiDialogs *dialogs );
extern GtkWidget   *gui_dialogs_get_jumpto_lblCurrent( GuiDialogs *dialogs );
extern GtkWidget   *gui_dialogs_get_jumpto_te( GuiDialogs *dialogs );

/* other */
extern gboolean    gui_dialogs_do_jumpto(
                         GuiDialogs *dialogs,
                         void *gui
                         );
#endif

#endif
