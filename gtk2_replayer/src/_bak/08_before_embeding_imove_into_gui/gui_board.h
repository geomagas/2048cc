#ifndef GUI_BOARD_H
#define GUI_BOARD_H

#include <gtk/gtk.h>

typedef struct _GuiBoard GuiBoard;

#ifndef GUI_BOARD_C

extern GuiBoard      *make_gui_board( void );
extern GuiBoard      *gui_board_free( GuiBoard *gb );

extern gboolean      gui_board_init_from_builder(
                           GuiBoard   *gb,
                           GtkBuilder *builder,
                           void       *gui
                           );
extern gboolean      gui_board_resize(
                           GuiBoard *gb,
                           long int imove,
                           void     *gui
                           );

#endif

#endif
