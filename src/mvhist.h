/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 11, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: gs.h
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#ifndef MVHIST_H
#define MVHIST_H

#include "gs.h"

typedef struct _MovesHistory MovesHistory;

#ifndef MVHIST_C
extern MovesHistory  *new_mvhist( void );
extern MovesHistory  *mvhist_free( MovesHistory *mvhist );

extern int mvhist_reset( MovesHistory *mvhist );

extern int mvhist_get_didundo( const MovesHistory *mvhist );
extern int mvhist_set_didundo( MovesHistory *mvhist, int didundo );

/* undo stack */

extern int              mvhist_isempty_undo_stack( const MovesHistory *mvhist );
extern int              mvhist_push_undo_stack(
                                MovesHistory    *mvhist,
                                const GameState *state
                                );
extern long int         mvhist_peek_undo_stack_count(
                                const MovesHistory *mvhist
                                );
extern const GameState  *mvhist_peek_undo_stack_state(
                                const MovesHistory *mvhist
                                );
extern int              mvhist_pop_undo_stack( MovesHistory *mvhist );

/* redo stack */

extern int              mvhist_free_redo_stack( MovesHistory *mvhist );
extern int              mvhist_isempty_redo_stack( const MovesHistory *mvhist );
extern int              mvhist_push_redo_stack(
                                MovesHistory    *mvhist,
                                const GameState *state
                                );
extern long int         mvhist_peek_redo_stack_count(
                                const MovesHistory *mvhist
                                );
extern const GameState  *mvhist_peek_redo_stack_state(
                                const MovesHistory *mvhist
                                );
extern int              mvhist_pop_redo_stack( MovesHistory *mvhist );

/* replay */

extern GSNode           *mvhist_new_replay_stack(
                               MovesHistory *mvhist,
                               unsigned int delay
                               );
extern int              mvhist_free_replay_stack( MovesHistory *mvhist );

extern long int         mvhist_peek_replay_stack_count(
                               const MovesHistory *mvhist
                               );
extern const GameState  *mvhist_peek_replay_stack_state(
                               const MovesHistory *mvhist
                               );
extern int               mvhist_pop_replay_stack( MovesHistory *mvhist );

extern const GSNode     *mvhist_iter_top_replay_stack(
                               MovesHistory *mvhist
                               );
extern const GSNode *mvhist_iter_bottom_replay_stack(
                               MovesHistory *mvhist
                               );
extern const GSNode     *mvhist_iter_down_replay_stack(
                               MovesHistory *mvhist,
                               const GSNode *it
                               );
extern const GSNode     *mvhist_iter_up_replay_stack(
                               MovesHistory *mvhist,
                               const GSNode *it
                               );

extern unsigned long int mvhist_get_replay_delay(
                               const MovesHistory *mvhist
                               );
extern long int          mvhist_get_replay_nmoves(
                               const MovesHistory *mvhist
                               );
extern long int          mvhist_get_replay_itcount(
                               const MovesHistory *mvhist
                               );

extern char              *mvhist_to_text( const MovesHistory *mvhist );
extern int               mvhist_save_to_file(
                               const MovesHistory *mvhist,
                               const char         *fname
                               );
extern MovesHistory      *new_mvhist_from_file( const char *fname );

#endif

#endif
