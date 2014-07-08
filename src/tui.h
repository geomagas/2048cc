/****************************************************************
 * This file is part of the "2048 Console Clone" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a
 * Date:         July 7, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: my.h, board.h, gs.h, mvhist.h
 * --------------------------------------------------------------
 *
 * The public interface for the text-user-interface of the game.
 * It is meant to be used by main.c, hiding the details of the
 * internal implementation and the dependencies.
 ****************************************************************
 */

#ifndef TUI_H
#define TUI_H

#include "my.h"
#include "board.h"
#include "gs.h"
#include "mvhist.h"

typedef struct Tui Tui;

/* input keys to be handled */
enum {
	TUI_KEYMASK_RESET     = MY_KEYMASK_RESET,
	TUI_KEYMASK_ARROW     = MY_KEYMASK_ARROW,
	TUI_KEY_NUL           = MY_KEY_NUL,
	TUI_KEY_UP            = MY_KEY_UP,
	TUI_KEY_DOWN          = MY_KEY_DOWN,
	TUI_KEY_LEFT          = MY_KEY_LEFT,
	TUI_KEY_RIGHT         = MY_KEY_RIGHT,
	TUI_KEY_ESCAPE        = MY_KEY_ESCAPE,
	TUI_KEY_BOARD_4       = '4',
	TUI_KEY_BOARD_5       = '5',
	TUI_KEY_BOARD_6       = '6',
	TUI_KEY_BOARD_8       = '8',
	TUI_KEY_SKIN          = 'S',
	TUI_KEY_UNDO          = 'U',
	TUI_KEY_REDO          = 'E',
	TUI_KEY_REPLAY        = 'P',
	TUI_KEY_REPLAY_BEG    = MY_KEY_HOME,
	TUI_KEY_REPLAY_END    = MY_KEY_END,
	TUI_KEY_REPLAY_PLAY   = 'P',
	TUI_KEY_REPLAY_SAVE   = 'S',
	TUI_KEY_REPLAY_LOAD   = 'L',
	TUI_KEY_REPLAY_BACK   = 'B',
	TUI_KEY_HINT          = 'H',
	TUI_KEY_RESET         = 'R',
	TUI_KEY_QUIT          = 'Q'
};

#ifndef TUI_C

extern Tui  *new_tui( GameState *state, MovesHistory *mvhist );
extern Tui  *tui_release( Tui *tui );
extern int  tui_update_board_reference( Tui *tui, Board *board );
extern int  tui_update_mvhist_reference( Tui *tui, MovesHistory *mvhist );

extern int  tui_cls( const Tui *tui );

extern void tui_draw_titlebar( const Tui *tui );
extern void tui_draw_scoresbar( const Tui *tui );
extern void tui_draw_help( const Tui *tui, int isenabledcommands );

extern void tui_clear_infobar( const Tui *tui );
extern void tui_draw_infobar_boardinfo( const Tui *tui );
extern void tui_draw_infobar_winmsg( const Tui *tui );

extern int  tui_draw_iobar_prompt_undo( const Tui *tui );
extern int  tui_draw_iobar_prompt_newgame( const Tui *tui );
extern int  tui_draw_iobar_prompt_watchreplay( const Tui *tui );
extern int  tui_draw_iobar_prompt_notyet( const Tui *tui );
extern int  tui_draw_iobar_prompt_replaycommand(
                    const Tui    *tui,
                    unsigned int *keymask
                    );
extern int  tui_draw_iobar_prompt_savereplay( const Tui *tui );
extern int  tui_draw_iobar_prompt_loadreplay( const Tui *tui );
extern int  tui_draw_iobar_prompt_loadreplay_nofile( const Tui *tui );

extern void tui_draw_iobar2_replaynavigation( const Tui *tui );
extern void tui_draw_iobar2_savereplayname(
                    const Tui *tui,
                    const char *fname
                    );
extern void tui_draw_iobar2_movescounter( const Tui *tui );
extern void tui_draw_iobar2_mainmenu( const Tui *tui );
extern void tui_draw_iobar_movescounter( const Tui *tui );
extern void tui_draw_iobar_autoreplayinfo( const Tui *tui );

extern void tui_prompt_replay_fname_to_load( const Tui *tui, char *fname );

extern int  tui_draw_board( const Tui *tui );
extern void tui_redraw( const Tui *tui, int isenabledcommands );

extern int  tui_cycle_skin( Tui *tui );

/* tui_sys_ functions do NOT require an allocated Tui object */
extern int  tui_sys_cls( void );
extern int  tui_sys_cursor_off( void );
extern int  tui_sys_cursor_on( void );
extern void tui_sys_press_a_key( void );
extern int  tui_sys_getkey( unsigned int *outKeyMask );
extern void tui_sys_beep( size_t ntimes );
extern int  tui_sys_sleep( unsigned long int msecs );

#endif

#endif
