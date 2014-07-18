/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 18, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: con_color.h
 * --------------------------------------------------------------
 *
 * The public interface of the TuiSkin "class".
 *
 * The "class" realizes colorized skins for the primitive text
 * user interface used in the game. It is meant to be used by tui.c
 ****************************************************************
 */

#ifndef TUI_SKIN_H
#define TUI_SKIN_H

#include "con_color.h"

/* The "class" is forward-declared as an opaque data-type */
typedef struct _tuiskin TuiSkin;

#ifndef TUI_SKIN_C
extern TuiSkin   *new_tui_skin( void );
extern TuiSkin   *tui_skin_release( TuiSkin *skin );

extern int       tui_skin_cycle( TuiSkin *skin );

/* left half of the screen */
extern const ConColors *tui_skin_get_colors_screen( const TuiSkin *skin );

extern const ConColors *tui_skin_get_colors_titlebar( const TuiSkin *skin );

extern const ConColors *tui_skin_get_colors_tile0( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile2( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile4( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile8( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile16( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile32( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile64( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile128( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile256( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile512( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile1024( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tile2048( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_tilebig( const TuiSkin *skin );

extern const ConColors *tui_skin_get_colors_scoresbar( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_infobar( const TuiSkin *skin );

/* right half of the screen */

extern const ConColors *tui_skin_get_colors_help_box( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_help_header( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_help_footer( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_help_commands(const TuiSkin *skin);
extern const ConColors *tui_skin_get_colors_help_commands_disabled(
                               const TuiSkin *skin
                               );

extern const ConColors *tui_skin_get_colors_iobar2( const TuiSkin *skin );
extern const ConColors *tui_skin_get_colors_iobar( const TuiSkin *skin );

#endif

#endif
