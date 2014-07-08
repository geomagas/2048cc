/****************************************************************
 * This file is part of the "2048 Console Clone" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a
 * Date:         July 7, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#ifndef GS_H
#define GS_H

#include "board.h"

typedef struct _GameState GameState;
typedef struct _GSNode    GSNode;

enum {  /* GameState direction of next-move */
	GS_NEXTMOVE_NONE = 0,
	GS_NEXTMOVE_UP,
	GS_NEXTMOVE_DOWN,
	GS_NEXTMOVE_LEFT,
	GS_NEXTMOVE_RIGHT
};

#ifndef GS_C

/* gamestate interface */

extern GameState  *new_gamestate( int dim );
extern GameState  *gamestate_free( GameState *state );
extern int        gamestate_reset( GameState *state );
extern int        gamestate_copy( GameState *dst, const GameState *src );

extern Board      *gamestate_get_board( const GameState *state );
extern long int   gamestate_get_score( const GameState *state );
extern long int   gamestate_get_bestscore( const GameState *state );
extern const char *gamestate_get_prevmove_label( const GameState *state );

extern int        gamestate_set_board_reference( GameState *state, Board *board );
extern int        gamestate_set_score( GameState *state, long int score );
extern int        gamestate_set_bestscore( GameState *state, long int bscore );
extern int        gamestate_set_prevmove( GameState *state, int prevmv );

extern char       *gamestate_to_text( const GameState *state );
extern GameState  *new_gamestate_from_text( char *text );


/* gamestate-stack interface */

extern int             gsstack_push( GSNode **stack, const GameState *state );
extern const GSNode    *gsstack_peek( const GSNode *stack );
extern long int        gsstack_peek_count( const GSNode *stack );
extern const GameState *gsstack_peek_state( const GSNode *stack );
extern int             gsstack_pop( GSNode **stack );
extern GSNode          *gsstack_dup_reversed( const GSNode *stack );
extern GSNode          *gsstack_free( GSNode **stack );

extern const GSNode    *gsstack_iter_top( const GSNode *stack );
extern const GSNode    *gsstack_iter_bottom( const GSNode *stack );
extern const GSNode    *gsstack_iter_down( const GSNode *it );
extern const GSNode    *gsstack_iter_up( const GSNode *it );

extern char            *gsstack_to_text( const GSNode *stack );
extern GSNode          *new_gsnode_from_text( char *text );

extern void            dbg_gsnode_dump( GSNode *node );
extern void            dbg_gsstack_dump( GSNode *stack );

#endif

#endif
