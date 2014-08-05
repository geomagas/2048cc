/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 21, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: common.h, gs.h, mvhist.h
 * --------------------------------------------------------------
 *
 * Private implementation of the MovesHistory "class". The accompanying
 * header file "mvhist.h" exposes publicly the "class" as an opaque
 * data-type.
 *
 * Functions with a "_" prefix in their names are meant to be private
 * in this source-module. They are usually inlined, without performing
 * any sanity check on their arguments.
 *
 * A MovesHistory object is responsible for recording in several forms
 * all the moves performed by the player, along with related information,
 * during a game.
 *
 * The "class" realizes 3 gsstacks: undo, redo & replay.stack,
 * along with some meta-data:
 * - didundo (has the player un-done at least one of his moves?)
 * - replay.delay (how many milliseconds between moves when
 *   auto-playing a replay?)
 * - replay.nmoves (how many moves have been recorded?)
 * - replay.itcount (what is the count of the current move
 *   when in replay-mode?)
 *
 * The undo & redo gsstacks are expected to be utilized during
 * normal-mode of a game, while the replay.stack is expected to
 * be utilized in replay-mode (see _do_replay() in file: "main.c").
 *
 * The above is conceptually enforced by this "class" by defining
 * the replay field as a separate nested struct, called: replay
 * (see the definition of struct _MovesHistory further below).
 *
 * The replay.stack is actually a reversed duplicate of the undo
 * gsstack. However, the count field of each node is NOT reversed,
 * so extra care should be taken in calculations involving them! 
 ****************************************************************
 */

#define MVHIST_C

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gs.h"
#include "mvhist.h"

/* The definition of the "class"
 * (it is publicly exposed as an opaque data-type).
 */
struct _MovesHistory {
	int    didundo;    /* has the player done at least 1 undo? */
	GSNode *undo;      /* undo stack (stores game-states) */
	GSNode *redo;      /* redo stack (stores game-states) */
	struct {
		unsigned long int delay;/* time delay during autoplay (msecs)*/
		long int nmoves;        /* length of replay-stack */
		long int itcount;       /* count of node under iterator */
		GSNode   *stack;        /* the replay-stack */
	} replay;
};

/* --------------------------------------------------------------
 * (Constructor) MovesHistory *new_mvhist():
 *
 * The moves-history constructor instantiates a new object in memory,
 * initializes it with default values, and returns a pointer to it,
 * or NULL on error.
 * --------------------------------------------------------------
 */
MovesHistory *new_mvhist( void )
{
	MovesHistory *mvhist = calloc(1, sizeof(*mvhist) );
	if ( NULL == mvhist ) {
		DBGF( "%s", "calloc() failed!" );
		return NULL;
	}

	mvhist->replay.delay = 750;
	return mvhist;
}

/* --------------------------------------------------------------
 * (Destructor) MovesHistory *mvhist_free():
 *
 * The moves-history destructor releases all resources occupied
 * by the specified object, and returns NULL (so the caller may
 * assign it back to the object pointer).
 * --------------------------------------------------------------
 */
MovesHistory *mvhist_free( MovesHistory *mvhist )
{
	if ( mvhist ) {
		gsstack_free( &mvhist->undo );
		gsstack_free( &mvhist->redo );
		gsstack_free( &mvhist->replay.stack );
		free( mvhist );
	}

	return NULL;
}

/* --------------------------------------------------------------
 * int mvhist_reset():
 *
 * Reset the specified moves-history object (mvhist) for a new game.
 * Return 0 (false) on error, 1 (true) otherwise.
 *
 * NOTE: Resetting a moves-history object does NOT change its
 *       replay.delay field.
 * --------------------------------------------------------------
 */
int mvhist_reset( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return 0; /* false */
	}
	mvhist->didundo = 0; /* false */
	mvhist->undo = gsstack_free( &mvhist->undo );
	mvhist->redo = gsstack_free( &mvhist->redo );
	mvhist->replay.stack = gsstack_free( &mvhist->replay.stack );
	mvhist->replay.nmoves = 0;
	mvhist->replay.itcount = 0;

	return 1;
}

/* --------------------------------------------------------------
 * int mvhist_isempty_undo_stack():
 *
 * Return 1 (true) if the undo gsstack of the specified moves-history
 * object (mvhist) is empty, or on error. Return 0 (false) otherwise. 
 * --------------------------------------------------------------
 */
int mvhist_isempty_undo_stack( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 1; /* true */
	}

	return NULL == mvhist->undo;
}

/* --------------------------------------------------------------
 * int mvhist_push_undo_stack():
 *
 * Push the specified game-state object (state) onto the undo
 * gsstack of the specified moves-history object (mvhist).
 * Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int mvhist_push_undo_stack( MovesHistory *mvhist, const GameState *state )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;  /* false */
	}

	return gsstack_push( &mvhist->undo, state );
}

/* --------------------------------------------------------------
 * long int mvhist_peek_undo_stack_count():
 *
 * Return the value of the count field, stored at the top node of
 * the undo gsstack of the specified moves-history object (mvhsit),
 * or 0 on error.
 *
 * NOTE: The count of nodes in a gsstack is 1-based.
 * --------------------------------------------------------------
 */
long int mvhist_peek_undo_stack_count( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;
	}

	return gsstack_peek_count( mvhist->undo );
}

/* --------------------------------------------------------------
 * const GameState *mvhist_peek_undo_stack_state():
 *
 * Return a pointer to the game-state object stored at the top
 * node of the undo gsstack of the specified moves-history object
 * (mvhist), or NULL on error.
 * --------------------------------------------------------------
 */
const GameState *mvhist_peek_undo_stack_state( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}
	return gsstack_peek_state( mvhist->undo );
}

/* --------------------------------------------------------------
 * int mvhist_pop_undo_stack():
 *
 * Remove the top node of the undo gsstack of the specified
 * moves-history object (mvhist). Return 0 (false) on error,
 * 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int mvhist_pop_undo_stack( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;  /* false */
	}
	return gsstack_pop( &mvhist->undo );
}

/* --------------------------------------------------------------
 * int mvhist_isempty_redo_stack():
 *
 * Return 1 (true) if the redo gsstack of the specified moves-history
 * object (mvhist) is empty, or on error. Return 0 (false) otherwise. 
 * --------------------------------------------------------------
 */
int mvhist_isempty_redo_stack( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 1; /* true */
	}

	return NULL == mvhist->redo;
}

/* --------------------------------------------------------------
 * int mvhist_free_redo_stack():
 *
 * Destroy all nodes of the redo gsstack of the specified moves-history
 * object (mvhist). Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int mvhist_free_redo_stack( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;  /* false */
	}

	mvhist->redo = gsstack_free( &mvhist->redo );
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int mvhist_push_redo_stack():
 *
 * Push the specified game-state object (state) onto the redo
 * gsstack of the specified moves-history object (mvhist).
 * Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int mvhist_push_redo_stack( MovesHistory *mvhist, const GameState *state )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;  /* false */
	}
	return gsstack_push( &mvhist->redo, state );
}

/* --------------------------------------------------------------
 * long int mvhist_peek_redo_stack_count():
 *
 * Return the value of the count field, stored at the top node of
 * the redo gsstack of the specified moves-history object (mvhsit),
 * or 0 on error.
 *
 * NOTE: The count of nodes in a gsstack is 1-based.
 * --------------------------------------------------------------
 */
long int mvhist_peek_redo_stack_count( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;
	}
	return gsstack_peek_count( mvhist->redo );
}

/* --------------------------------------------------------------
 * const GameState *mvhist_peek_redo_stack_state():
 *
 * Return a pointer to the game-state object stored at the top
 * node of the redo gsstack of the specified moves-history object
 * (mvhist), or NULL on error.
 * --------------------------------------------------------------
 */
const GameState *mvhist_peek_redo_stack_state( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}
	return gsstack_peek_state( mvhist->redo );
}

/* --------------------------------------------------------------
 * int mvhist_pop_redo_stack():
 *
 * Remove the top node of the redo gsstack of the specified
 * moves-history object (mvhist). Return 0 (false) on error,
 * 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int mvhist_pop_redo_stack( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;  /* false */
	}
	return gsstack_pop( &mvhist->redo );
}

/* --------------------------------------------------------------
 * GSNode *mvhist_init_replay():
 *
 * Prepare the replay nested structure of the specified moves-history
 * object (mvhist) for first use. Return a pointer to the newly created
 * replay.stack, or NULL on error.
 *
 * NOTES (IMPORTANT!):
 *
 *     Preparation involves creating a new replay.stack as a reversed
 *     duplicate of mvhist's undo gsstack. The undo gsstack CANNOT be
 *     empty, because it always contains at least the 1st move of the
 *     game (since it is performed automatically). 
 *
 *     On the other hand, if the replay.stack in NOT already NULL,
 *     the function returns an error. This is an extra precaution
 *     for reducing chances of MEMORY-LEAKS, and for enforcing the
 *     GOOD PRACTICE of setting all pointers to NULL before using
 *     them.
 *
 *     In this "class", the replay.stack is ALWAYS either initialized
 *     to NULL via calloc() in new_mvhist(), or it is explicitly set
 *     to NULL every time it gets freed, via mvhist_cleanup_replay().
 *        
 *     On success, replay.nmoves is set equal to the count of nodes of
 *     the newly created replay.stack, replay.delay is set equal to the
 *     2nd argument of the function (NO SANITY CHECK, should be fixed?)
 *     and replay.itcount is set to 0.
 * --------------------------------------------------------------
 */
GSNode *mvhist_init_replay( MovesHistory *mvhist, unsigned int delay )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return NULL;
	}
	if ( NULL != mvhist->replay.stack ) {
		DBGF( "%s", "replay.stack is non-NULL. Cannot create replay!" );
		return NULL;
	}

	mvhist->replay.stack = gsstack_dup_reversed( mvhist->undo );
	if ( NULL == mvhist->replay.stack ) {
		DBGF( "%s", "gsstack_dup_reversed(mvhist->undo) failed!)" );
		return NULL;
	}

	mvhist->replay.nmoves  = gsstack_peek_count( mvhist->undo );
	mvhist->replay.itcount = 0;
	mvhist->replay.delay   = delay;

	return mvhist->replay.stack;
}

/* --------------------------------------------------------------
 int mvhist_cleanup_replay():
 *
 * Cleanup the replay nested structure of the specified moves-history
 * object (mvhist). Return 0 (false) on error, 1 (true) otherwise.
 *
 * NOTE: Cleanup involves, among other things, freeing up the
 *       replay.stack and setting its pointer equal to NULL.
 * --------------------------------------------------------------
 */
int mvhist_cleanup_replay( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "calloc() failed!" );
		return 0;  /* false */
	}

	mvhist->replay.stack   = gsstack_free( &mvhist->replay.stack );
	mvhist->replay.nmoves  = 0;
	mvhist->replay.itcount = 0;
	mvhist->replay.delay   = 750;

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * long int mvhist_peek_replay_stack_count():
 *
 * Return the value of the count field, stored at the top node of
 * the replay.stack of the specified moves-history object (mvhsit),
 * or 0 on error.
 *
 * NOTE: The count of nodes in a gsstack is 1-based.
 * --------------------------------------------------------------
 */
long int mvhist_peek_replay_stack_count( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;
	}
	return gsstack_peek_count( mvhist->replay.stack );
}

/* --------------------------------------------------------------
 * const GameState *mvhist_peek_replay_stack_state():
 *
 * Return a pointer to the game-state object stored at the top
 * node of the replay.stack of the specified moves-history object
 * (mvhist), or NULL on error.
 * --------------------------------------------------------------
 */
const GameState *mvhist_peek_replay_stack_state( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}
	return gsstack_peek_state( mvhist->replay.stack );
}

/* --------------------------------------------------------------
 * int mvhist_pop_replay_stack():
 *
 * Remove the top node of the replay.stack of the specified
 * moves-history object (mvhist). Return 0 (false) on error,
 * 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int mvhist_pop_replay_stack( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;  /* false */
	}

	return gsstack_pop( &mvhist->replay.stack );
}

/* --------------------------------------------------------------
 * (Getter) unsigned long int mvhist_get_replay_delay():
 *
 * Return the current value of the delay field, inside the replay
 * nested structure of the specified moves-history object (mvhist),
 * or 0 on error.
 * --------------------------------------------------------------
 */
unsigned long int mvhist_get_replay_delay( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;
	}

	return mvhist->replay.delay;
}

/* --------------------------------------------------------------
 * long int mvhist_get_replay_nmoves():
 *
 * Return the current value of the nmoves field, inside the replay
 * nested structure of the specified moves-history object (mvhist),
 * or -1 on error.
 * --------------------------------------------------------------
 */
long int mvhist_get_replay_nmoves( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return -1;
	}

	return mvhist->replay.nmoves;
}

/* --------------------------------------------------------------
 * long int mvhist_get_replay_itcount():
 *
 * Return the current value of the itcount field, inside the replay
 * nested structure of the specified moves-history object (mvhist),
 * or -1 on error.
 *
 * NOTE (IMPORTANT!):
 *
 *     The itcount field makes sense ONLY in replay-mode, when
 *     an iteration has been started on the replay.stack. It
 *     exists due to decoupling of the tui "class" from the
 *     gsstack "class", and thus from the replay.stack.
 *
 *     For example, without it, a tui object wouldn't be able to
 *     display the count of the current move in the replay-mode.
 *     However, this may be a design-flaw to be re-considered. 
 * --------------------------------------------------------------
 */
long int mvhist_get_replay_itcount( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return -1;
	}

	return mvhist->replay.itcount;
}

/* --------------------------------------------------------------
 * const GSNode *mvhist_iter_top_replay_stack():
 *
 * Return an (iterator) pointer to the top node of the replay.stack
 * of the specified moves-history object (mvhist), or NULL on error.
 *
 * NOTE: On success, the count of the top-node of the replay.stack
 *       is stored in the replay.itcount field. For details, see
 *       the comments of the function: mvhist_get_replay_itcount().
 * --------------------------------------------------------------
 */
const GSNode *mvhist_iter_top_replay_stack( MovesHistory *mvhist )
{
	const GSNode *it = NULL;

	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}

	it = gsstack_iter_top( mvhist->replay.stack );
	if ( it ) {
		mvhist->replay.itcount = gsstack_peek_count( it );
	}
	return it;
}

/* --------------------------------------------------------------
 * const GSNode *mvhist_iter_bottom_replay_stack():
 *
 * Return an (iterator) pointer to the bottom node of the replay.stack
 * of the specified moves-history object (mvhist), or NULL on error.
 *
 * NOTE: On success, the count of the bottom-node of the replay.stack
 *       is stored in the replay.itcount field. For details, see
 *       the comments of the function: mvhist_get_replay_itcount().
 * --------------------------------------------------------------
 */
const GSNode *mvhist_iter_bottom_replay_stack( MovesHistory *mvhist )
{
	const GSNode *it = NULL;

	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}

	it = gsstack_iter_bottom( mvhist->replay.stack );
	if ( it ) {
		mvhist->replay.itcount = gsstack_peek_count( it );
	}
	return it;
}

/* --------------------------------------------------------------
 * const GSNode *mvhist_iter_down_replay_stack():
 *
 * Given an iterator pointer (it) to a node of the replay.stack of
 * the specified moves-history object (mvhist), return an iterator
 * pointer to the node below it, or NULL on error.
 *
 * NOTES: On success, the count of the newly pointed node in
 *        the replay.stack is stored in the replay.itcount field.
 *        For details, see the comments of the function:
 *        mvhist_get_replay_itcount().
 *
 *        Iterator pointers are plain pointers to GSNode nodes.
 * --------------------------------------------------------------
 */
const GSNode *mvhist_iter_down_replay_stack(
	MovesHistory *mvhist,
	const GSNode *it
	)
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}

	it = gsstack_iter_down( it );
	if ( it ) {
		mvhist->replay.itcount = gsstack_peek_count( it );
	}
	return it;
}

/* --------------------------------------------------------------
 * const GSNode *mvhist_iter_up_replay_stack():
 *
 * Given an iterator pointer (it) to a node of the replay.stack of
 * the specified moves-history object (mvhist), return an iterator
 * pointer to the node above it, or NULL on error.
 *
 * NOTES: On success, the count of the newly pointed node in
 *        the replay.stack is stored in the replay.itcount field.
 *        For details, see the comments of the function:
 *        mvhist_get_replay_itcount().
 *
 *        Iterator pointers are plain pointers to GSNode nodes.
 * --------------------------------------------------------------
 */
const GSNode *mvhist_iter_up_replay_stack(
	MovesHistory *mvhist,
	const GSNode *it
	)
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return NULL;
	}

	it = gsstack_iter_up( it );
	if ( it ) {
		mvhist->replay.itcount = gsstack_peek_count( it );
	}
	return it;
}

/* --------------------------------------------------------------
 * (Getter) int mvhist_get_didundo():
 *
 * Return the current value of the boolean didundo field, inside
 * the replay nested structure of the specified moves-history
 * object (mvhist), or 0 (false) on error.
 * --------------------------------------------------------------
 */
int mvhist_get_didundo( const MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return 0;
	}
	return mvhist->didundo;
}

/* --------------------------------------------------------------
 * (Setter) int mvhist_set_didundo():
 *
 * Set the boolean value of the didundo field inside the
 * replay nested structure of the specified moves-history
 * object (mvhist), to the value specified by the 2nd
 * argument (didundo). Return 0 (false) on error, 1 (true)
 * otherwise.
 * --------------------------------------------------------------
 */
int mvhist_set_didundo( MovesHistory *mvhist, int didundo )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return 0;  /* false */
	}

	mvhist->didundo = didundo;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int _replay_append_to_fp():
 *
 * Serialize the replay nested structure of the specified moves-history
 * object (mvhist) and append it to the specified file (fp). Return 0
 * (false) on error, 1 (true) otherwise.
 *
 * NOTES:
 *
 *    The serialization first produces a text line of the form:
 *
 *    "replay.delay replay.nmoves replay.itcount\r\n"
 *
 *    Then it produces a series of text-lines, each one corresponding
 *    to a serialized node of the replay.stack. Each of those lines
 *    has the following form:
 *
 *    "game-state-meta-data@board-meta-data#board-tile-values\r\n"
 *
 *    For details about the serialized game-state-meta-data,
 *    board-meta-data and board-tile-values, see the function:
 *    gsstack_append_to_fp() (defined in the file: "gs.c")
 * --------------------------------------------------------------
 */
static inline int _replay_append_to_fp( const MovesHistory *mvhist, FILE *fp )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return 0;  /* false */
	}
	if ( NULL == fp ) {
		DBGF( "%s", "NULL pointer argument (fp)!" );
		return 0;  /* false */
	}

	/* mvhist->replay.delay
	 * + mvhist->replay.nmoves
	 * + mvhist->replay.itcount
	 */
	if ( fprintf(
		fp,
		"%lu %ld %ld\r\n",
		mvhist->replay.delay,
		mvhist->replay.nmoves,
		mvhist->replay.itcount
		) < 0
	){
		DBGF( "%s", "fprintf() failed!" );
		return 0;  /* false */
	}

	/* + mvhist->replay.stack */
	if ( !gsstack_append_to_fp(mvhist->replay.stack, fp) ) {
		DBGF( "%s", "gsstack_append_to_fp(mvhist->replay.stack) failed!" );
		return 0;  /* false */
	}

	return 1;
}

/* --------------------------------------------------------------
 * int mvhist_save_to_file():
 *
 * Serialize the specified moves-history object (mvhist) and write
 * it to the specified text file (fname). Return 0 (false) on error,
 * 1 (true) otherwise.
 *
 * NOTES:
 *
 *    The serialization first produces a text line of the form:
 *    "didundo\r\n"
 *
 *    Then it produces a series of text-lines, each one corresponding
 *    to a serialized node of the undo gsstack of the object. If the
 *    undo gsstack is empty, then a single line is produced instead:
 *    "NULL:\r\n"
 *
 *    Next it produces a series of text-lines, each one corresponding
 *    to a serialized node of the redo gsstack of the object. If the
 *    redo gsstack is empty, then once again just one single line is
 *    produced instead:
 *    "NULL:\r\n"
 *
 *    Finally, it produces a text-line with the serialized meta-data
 *    of the replay nested structure of the object, followed by a series
 *    of lines, each one corresponding to a serialized node of the
 *    replay.stack. If the replay.stack is empty, then again just a
 *    single line gets produced instead: "NULL:\r\n"
 *
 *    For details about the exact serializations, see the functions:
 *    - replay_append_to_fp()
 *    - gsstack_append_to_fp()   (defined in the file: "gs.c") 
 *    - gamestate_append_to_fp() (defined in the file: "gs.c") 
 *    - board_append_to_fp()     (defined in the file: "board.c") 
 * --------------------------------------------------------------
 */
int mvhist_save_to_file( const MovesHistory *mvhist, const char *fname )
{
	FILE *fp  = NULL;

	if ( NULL == mvhist || NULL == fname ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;  /* false */
	}

	fp = fopen( fname, "wb" );
	if ( NULL == fp ) {
		DBGF( "Could not write to file %s", fname );
		return 0;  /* false */
	}

	/* mvhist->didundo */
	if ( fprintf(fp, "%d\r\n", mvhist->didundo) < 0 ) {
		DBGF( "%s", "fprintf() failed!" );
		goto ret_failure;
	}

	/* + mvhist->undo (stack) */
	if ( !gsstack_append_to_fp(mvhist->undo, fp) ) {
		DBGF( "%s", "gsstack_append_to_fp(mvhist->undo) failed!" );
		goto ret_failure;
	}
	
	/* + mvhist->redo (stack) */
	if ( !gsstack_append_to_fp(mvhist->redo, fp) ) {
		DBGF( "%s", "gsstack_append_to_fp(mvhist->redo) failed!" );
		goto ret_failure;
	}

	/* + mvhist->replay */
	if ( !_replay_append_to_fp(mvhist, fp) ) {
		DBGF( "%s", "_replay_append_to_fp() failed!" );
		goto ret_failure;
	}

	fclose( fp );
	return 1;  /* true */

ret_failure:
	fclose( fp );
	return 0;  /* false */
}

/* --------------------------------------------------------------
 * int _load_stack_from_line_plus_fp_lines():
 *
 * Given a gsstack (stack), a text file pointer (fp) and a c-string
 * (line) with size at least (lnsize) including the NUL terminating
 * byte, de-serialize as a node the line, along with required file
 * lines after fp, while pushing them onto the stack. Reverse the
 * loaded stack before handing it back to the caller.
 *
 * Return 0 (false) on error, 1 (true) otherwise.
 *
 * NOTES (IMPORTANT!):
 *
 * 1. All pointer arguments of the function MUST be non-NULL and valid.
 * 2. The initial line and subsequent file lines MUST be already properly
 *    serialized as gsstack nodes.
 * 3. The initial line should have been already read from the file before
 *    being passed as an argument to this function. Thus, within the file,
 *    (fp) MUST initially point to the start of the NEXT line to be read
 *    (that is, excluding the line passed as an argument to the function).
 * 4. lnsize MUST be large enough to hold the LARGEST line in the file
 *    (including the NUL terminating byte).
 *
 * When ALL the above are true, the function knows how many lines to
 * read from the file while populating the stack, because their total
 * count is already stored in the beginning of the initial line that
 * has been passed as an argument to the function.
 *
 * It is part of the serialization of a gsstack node, to produce its
 * 1-based count inside the stack as the first thing in its serialized
 * text.
 * --------------------------------------------------------------
 */
static inline int _load_stack_from_line_plus_fp_lines(
	GSNode       **stack,
	char         *line,
	size_t       lnsize,
	FILE         *fp
	)
{
	long int i = 0;
	long int count;          /* total number of lines to load */
	GSNode *revstack = NULL; /* reversed stack */
	GSNode *node     = NULL; /* temporary node */

	if ( 0 == strcmp(line, "NULL:\n") ) {
		return 1;  /* true */
	}

	if ( sscanf(line, "%ld", &count) < 1 ) {
		DBGF( "%s", "sscanf() failed to read count of lines" );
		goto ret_failure;
	}
	for (i=0; i < count; i++) {
		node = new_gsnode_from_text( line );
		if (
		!gsstack_push( stack, gsstack_peek_state(node) )
		){
			DBGF("%s", "gsstack_push() failed!");
			free( node );
			goto ret_failure;
		}
		free( node );

		if ( i == count-1 ) {
			break;
		}
		if ( NULL == fgets(line, lnsize, fp) ) {
			DBGF( "%s", "fgets() failed while reading stack!" );
			goto ret_failure;
		}
		s_fixeol( line );
	}

	/* reverse the loaded stack */
	revstack = gsstack_dup_reversed( *stack );
	if ( NULL == revstack ) {
		DBGF( "%s", "gsstack_dup_reversed() failed!" );
		goto ret_failure;
	}
	gsstack_free( stack );
	*stack = revstack;

	return 1;  /* true */

ret_failure:
	free( node );
	gsstack_free( stack );
	return 0;
}

/* --------------------------------------------------------------
 * MovesHist *new_mvhist_from_file():
 *
 * De-serialize the contents of the specified text file (fname)
 * and load them into a newly created moves-histtoy object.
 * Return a pointer to the newly created object, or NULL on error.
 *
 * NOTE: The contents of the text file are expected to be already
 *       serialized, as described in the comments of the function:
 *       mvhist_save_to_file()
 * --------------------------------------------------------------
 */
#define MAX_LNSIZE  1024
MovesHistory *new_mvhist_from_file( const char *fname )
{
	FILE *fp = NULL;
	char line[MAX_LNSIZE] = {'\0'};  /* for reading single lines from file */
	MovesHistory *mvh = NULL;

	if ( NULL == fname ) {
		DBGF( "%s", "NULL pointer argument (fname)!" );
		return NULL;
	}

	fp = fopen( fname, "r" );
	if ( NULL == fp ) {
		DBGF( "Could not read from file: %s", fname );
		return NULL;
	}

	mvh = new_mvhist();
	if ( NULL == mvh ) {
		DBGF( "%s", "mvh = new_mvhist() failed!" );
		goto ret_failure;
	}

	/* read didundo value */
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read mvh->didundo line!" );
		goto ret_failure;
	}
	s_fixeol( line );

	if ( sscanf(line, "%d", &mvh->didundo) < 1 ) {
		DBGF( "%s", "sscanf(&mvh->didundo) failed!" );
		goto ret_failure;
	}

	/*
	 * read the undo-stack
	*/
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read undo-stack line" );
		goto ret_failure;
	}
	s_fixeol( line );
	if ( !_load_stack_from_line_plus_fp_lines(
		&mvh->undo,
		line,
		MAX_LNSIZE,
		fp
		)
	){
		DBGF(
		    "%s",
		    "_load_stack_from_line_plus_fp_lines(undo) failed!"
		    );
		goto ret_failure;
	}

	/*
	 * read the redo-stack
	 */
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read redo-stack line" );
		goto ret_failure;
	}
	s_fixeol( line );
	if ( !_load_stack_from_line_plus_fp_lines(
		&mvh->redo,
		line,
		MAX_LNSIZE,
		fp
		)
	){
		DBGF(
		    "%s",
		    "_load_stack_from_line_plus_fp_lines(redo) failed"
		    );
		goto ret_failure;
	}

	/*
	 * read the replay struct
	 */

	/* first the meta-data */
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read mvh->redo" );
		goto ret_failure;
	}
	s_fixeol( line );
	if ( sscanf(
		line,
		"%lu %ld %ld",
		&mvh->replay.delay,
		&mvh->replay.nmoves,
		&mvh->replay.itcount
		) < 3
	){
		DBGF( "%s", "sscanf() failed to read replay meta-data!" );
		goto ret_failure;
	}
	/* then the replay-stack */
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read redo-stack node" );
		goto ret_failure;
	}
	s_fixeol( line );
	if (
	!_load_stack_from_line_plus_fp_lines(
		&mvh->replay.stack,
		line,
		MAX_LNSIZE,
		fp
		)
	){
		DBGF(
		  "%s",
		  "_load_stack_from_line_plus_fp_lines(replay.stack) failed"
		  );
		goto ret_failure;
	}

	fclose( fp );

	return mvh;

ret_failure:
	fclose( fp );
	free( mvh );
	return NULL;
}
