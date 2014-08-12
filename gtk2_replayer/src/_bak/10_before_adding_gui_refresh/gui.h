#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "gui_locale.h"
#include "gui_menus.h"
#include "gamedata.h"

#define DIR_GUI            "gui/"    /* gui resources */
#define FNAME_APPICON      DIR_GUI"gtk2_viewer_icon.png"
#define FNAME_APPLOGO      DIR_GUI"gtk2_viewer_logo.png"
#define FNAME_GLADE        DIR_GUI"gtk2_viewer.glade"

typedef struct _Gui Gui;

#ifndef GUI_C
extern Gui          *new_gui( int *argc, char ***argv, Gamedata *gamedata );
extern Gui          *gui_free( Gui *gui );
extern gboolean      gui_start( Gui *gui );

extern gboolean      gui_reload_from_glade_file(
                           Gui         *gui,
                           const gchar *fnameGlade
                           );

//extern gboolean      gui_get_quitOnDestroyAppWindow( const Gui *gui );
extern GtkWidget     *gui_get_appWindow( const Gui *gui );
extern GuiLocale     *gui_get_locale( const Gui *gui );
extern GuiMenus      *gui_get_menus( const Gui *gui );

/* statusbar */
extern GtkWidget     *gui_get_statusbar_widget( const Gui *gui );
extern guint         gui_get_statusbar_contextId( const Gui *gui );

/* gamedata related */
extern void          dbg_gui_print_gamedata_tiles( const Gui *gui );
extern Gamedata      *gui_get_gamedata( Gui *gui );
extern char          *gui_get_gamedata_fname( const Gui *gui );
extern long int      gui_get_gamedata_nmoves( const Gui *gui );
extern int           gui_get_gamedata_dim( const Gui *gui );
extern long int      gui_get_gamedata_sentinel( const Gui *gui );
extern int           gui_get_gamedata_nrandom( const Gui *gui );
extern unsigned long gui_get_gamedata_delay( const Gui *gui );
extern int           gui_get_gamedata_gamewon( const Gui *gui );
extern long int      gui_get_gamedata_tile_of_move(
                           const Gui *gui,
                           int       itile,
                           long int  imove
                           );
extern gboolean      gui_set_gamedata_from_fname( Gui *gui, char *fname );

extern GtkWidget     *gui_get_dlgAbout( const Gui *gui );

extern gboolean      gui_set_quitOnDestroyAppWindow( Gui *gui, gboolean val );

/* imove related */
extern long int      gui_get_imove( const Gui *gui );
extern gboolean      gui_set_imove( Gui *gui, long int imove );
extern gboolean      gui_is_imove_valid( const Gui *gui );
extern gboolean      gui_has_imove_prev( const Gui *gui );
extern gboolean      gui_has_imove_next( const Gui *gui );
extern gboolean      gui_set_imove_prev( Gui *gui );
extern gboolean      gui_set_imove_next( Gui *gui );

extern gboolean      gui_has_loaded_file( const Gui *gui );

extern gboolean      gui_refresh_scores( Gui *gui );
extern gboolean      gui_resize_board( Gui *gui );
extern gboolean      gui_refresh_board( Gui *gui );
extern gboolean      gui_refresh_prevnext( Gui *gui );
extern gboolean      gui_refresh_statusbar( const Gui *gui );

extern void          on_destroy_appWindow( GtkWidget *appWindow, Gui *gui );

#endif

#endif
