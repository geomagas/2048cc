#ifndef GUI_DIALOGS_H
#define GUI_DIALOGS_H

#include <gtk/gtk.h>

typedef struct _GuiDialogs GuiDialogs;

#ifndef GUI_DIALOGS_C
extern GuiDialogs  *make_gui_dialogs( void );
extern GuiDialogs  *gui_dialogs_free( GuiDialogs *dialogs );
extern gboolean    gui_dialogs_init_from_builder(
                         GuiDialogs  *dialogs,
                         GtkBuilder  *builder,
                         void        *gui
                         );

extern GtkWidget   *gui_dialogs_get_about_root( GuiDialogs *dialogs );

extern GtkWidget   *gui_dialogs_get_jumpto_root( GuiDialogs *dialogs );
extern GtkWidget   *gui_dialogs_get_jumpto_lblRange( GuiDialogs *dialogs );
extern GtkWidget   *gui_dialogs_get_jumpto_lblCurrent( GuiDialogs *dialogs );
extern GtkWidget   *gui_dialogs_get_jumpto_te( GuiDialogs *dialogs );
extern gboolean    gui_dialogs_do_jumpto(
                         GuiDialogs *dialogs,
                         void *gui
                         );
#endif

#endif
