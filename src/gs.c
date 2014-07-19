/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 18, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: common.h, board.h, gs.h
 * --------------------------------------------------------------
 *
 * Private implementation of the GameState & GSNode "classes".
 * The accompanying header file "gs.h" exposes publicly both
 * "classes" as opaque data-types, along with some additional
 * constants.
 *
 * However, for the GSNode "class", its public interface uses a different
 * naming-convention for the exposed functions. Their names are prefixed
 * with "gsstack_" instead of the expected "gsnode_".
 *
 * This is BAD-PRACTICE, and should be fixed in future versions of the
 * code (most probably, a GSStack struct should wrap GSNode, along with
 * some meta-data, and then used for public exposure instead of GSNode).
 *
 * Functions with a "_" prefix in their names are meant to be private
 * in this source-module. They are usually inlined, without performing
 * any sanity check on their arguments.
 *
 *
 * The GameState "class"
 * ---------------------
 *
 * The game-state reflects the current state of the game at any given time.
 * It consists of a a board object along with some meta-data, such as the
 * current score & best-score, whether a winning-tile has been reached, and
 * towards what direction was the board played in the previous and/or the
 * next move (for the latter two, GS_MVDIR_XXXX enumerated values are exposed
 * publicly, in the header file: "gs.h").
 *
 * The main usefulness of the game-state is when the game is played in
 * normal-mode, but it is also used during replay-mode (because any move
 * in a replay overwrites the current game-state... this helps us utilizing
 * the replay-mode also as a load/save game interface).
 *
 *
 * The GSNode(gsstack) "class"
 * ---------------------------
 *
 * gsstacks are currently used by a moves-history object (see: mvhist.c)
 * for keeping track of game-states during normal game-play and replays.
 * The moves-history object utilizes 3 stacks: the Undo, the Redo and
 * the Replay stacks (for details, see the above mentioned file).
 *
 * A gsstack links together GSNode nodes, and actually it is implemented
 * as a doubly linked-list (not a stack) because it makes it simpler to
 * deal with replays.
 *
 * So, a GSNode node consists of its count inside the stack, a game-state
 * object, and links to the previous and the next node in the stack.
 *
 * The usual operations of a stack (push, pop, peek) are implemented
 * as expected, except that popping removes the top node of the stack
 * WITHOUT returning its contents (it only returns a boolean value,
 * indicating whether an error occurred or not). Thus, before popping
 * a stack, one should peek its contents via any of the corresponding,
 * publicly exposed functions.
 *
 * Another twist, is that any gsstack provides primitive support of
 * iterators. Without them, it would be a bit too messy to implement
 * the replay-mode of the game. Look for functions having the prefix
 * "gsstack_iter_" in their names.
 ****************************************************************
 */
 
#define GS_C

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <string.h>

#include "common.h"
#include "board.h"
#include "gs.h"

/* Validation macro for game-state's next & previous moves.
 * They are defined in "gs.h".
 */
#define _VALID_MVDIR(dir)           \
(                                   \
	(dir) == GS_MVDIR_NONE      \
	|| (dir) == GS_MVDIR_UP     \
	|| (dir) == GS_MVDIR_DOWN   \
	|| (dir) == GS_MVDIR_LEFT   \
	|| (dir) == GS_MVDIR_RIGHT  \
)

/* Macro for converting a move-direction to a c-string.
 * Move-directions are defined in "gs.h".
 */
#define _MVDIR_TO_LABEL(dir)                                      \
(                                                                 \
	(dir) == GS_MVDIR_UP                                      \
		? "UP"                                            \
		: (dir) == GS_MVDIR_DOWN                          \
			? "DOWN"                                  \
			: (dir) == GS_MVDIR_LEFT                  \
				? "LEFT"                          \
				: (dir) == GS_MVDIR_RIGHT         \
					? "RIGHT"                 \
					: (dir) == GS_MVDIR_NONE  \
						? "NONE"          \
						: "ERROR"         \
)

/* Private definition of the GameState "class" */
struct _GameState {
	Board    *board;
	long int score;        /* current score */
	long int bscore;       /* best-score across multiple games */
	int      prevmv;       /* direction of previous move */
	int      nextmv;       /* direction of next move */
	int      iswin;        /* has the sentinel value reached? */
};

/* Private definition of the GSNode "class"
 * (single node for game-state stacks)
 */
struct _GSNode{
	long int       count;   /* node's count (1-based) */
	GameState      *state;  /* the game-state stored in the node*/
	struct _GSNode *down;   /* previous node (towards bottom) */
	struct _GSNode *up;     /* next node (towards top) */
};

/* --------------------------------------------------------------
 * GameState *new_gamestate():
 *
 * The game-state constructor instantiates a new game-state object
 * in memory, creates a square board of the specified single-dimension
 * for it (dim), initializes the object to default values and returns
 * a pointer to the new object, or NULL on error.
 * --------------------------------------------------------------
 */
GameState *new_gamestate( int dim )
{
	GameState *state = calloc(1, sizeof(*state) );
	if ( NULL == state ) {
		DBGF( "%s", "calloc failed!" );
		return NULL;
	}

	state->board = new_board();
	if ( NULL == state->board ) {
		DBGF( "%s", "new_board() failed!" );
		free( state );
		return NULL;
	}

	if ( !board_resize_and_reset(state->board, dim) ) {
		DBGF( "board_resize_and_reset(state->board, %d) failed!", dim );
		board_free( state->board );
		free( state );
		return NULL;
	}

	state->score  = 0;
	state->bscore = 0;
	state->iswin  = 0;  /* false */
	state->prevmv = GS_MVDIR_NONE;
	state->nextmv = GS_MVDIR_NONE;

	return state;
}

/* --------------------------------------------------------------
 * GameState *gamestate_free():
 *
 * The game-state destructor releases all resources occupied by the
 * specified game-state object, and it returns NULL (so the caller
 * may assign it back to the object pointer).
 * --------------------------------------------------------------
 */
GameState *gamestate_free( GameState *state )
{
	if ( state ) {
		board_free( state->board );
		free( state );
	}
	return NULL;
}

/* --------------------------------------------------------------
 * int gamestate_copy():
 *
 * Copy the specified source game-state object (src) into the
 * specified destination game-state object (dst). Return 0 (false)
 * on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int gamestate_copy( GameState *dst, const GameState *src )
{
	if ( NULL == dst ) {
		DBGF( "%s", "NULL pointer argument (dst)" );
		return 0;  /* false */
	}
	if ( NULL == src ) {
		DBGF( "%s", "NULL pointer argument (src)" );
		return 0;  /* false */
	}

	if ( !board_copy(dst->board, src->board) ) {
		DBGF( "%s", "board_copy() failed!" );
		return 0;  /* false */
	}
	dst->score  = src->score;
	dst->bscore = src->bscore;
	dst->iswin  = src->iswin;
	dst->prevmv = src->prevmv;
	dst->nextmv = src->nextmv;

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int board_reset():
 *
 * Reset the specified game-state object (state) for a new game.
 * Return 0 (false) on error, 1 (true) otherwise.
 *
 * NOTE: Resetting a game-state does NOT change its best-score.
 * --------------------------------------------------------------
 */
int gamestate_reset( GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument (state)!" );
		return 0;  /* false */
	}
	board_reset( state->board );
	board_generate_ntiles(
		state->board,
		2 * board_get_nrandom( state->board )
		);
	state->score  = 0;
	state->iswin  = 0;  /* false */
	state->prevmv = GS_MVDIR_NONE;
	state->nextmv = GS_MVDIR_NONE;

	return 1;
}

/* --------------------------------------------------------------
 * (Getter) Board *gamestate_get_board():
 *
 * Return a pointer to the current board object of the specified
 * game-state object (state) or NULL on error.
 * --------------------------------------------------------------
 */
const Board *gamestate_get_board( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	return state->board;
}

/* --------------------------------------------------------------
 * (Getter) long int gamestate_get_score():
 *
 * Return the current value of the score field, of the specified
 * game-state object (state), or LONG_MAX on error.
 * --------------------------------------------------------------
 */
long int gamestate_get_score( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return LONG_MAX;
	}
	return state->score;
}

/* --------------------------------------------------------------
 * (Getter) long int gamestate_get_bestscore():
 *
 * Return the current value of the best-score field, of the
 * specified game-state object (state), or LONG_MAX on error.
 * --------------------------------------------------------------
 */
long int gamestate_get_bestscore( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return LONG_MAX;
	}

	return state->bscore;
}

/* --------------------------------------------------------------
 * (Getter) int gamestate_get_iswin():
 *
 * Return the current value of the boolean iswin field, of the
 * specified game-state object (state), or 0 (false) on error.
 * --------------------------------------------------------------
 */
int gamestate_get_iswin( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0  /* false */;
	}

	return state->iswin;
}

/* --------------------------------------------------------------
 * (Getter) int gamestate_get_prevmove():
 *
 * Return the current enumerated value of the prevmv field, of the
 * specified game-state object (state), or GS_MVDIR_NONE on error.
 * --------------------------------------------------------------
 */
int gamestate_get_prevmove( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return GS_MVDIR_NONE;
	}

	return state->prevmv;
}

/* --------------------------------------------------------------
 * (Getter) const char *gamestate_get_prevmove_label():
 *
 * Return a pointer to the internal c-string corresponding to the
 * current enumerated value of the prevmv field, of the specified
 * game-state object (state), or NULL on error.
 *
 * NOTE (IMPORTANT!):
 *
 *   The caller should NOT attempt to modify the contents of
 *   the c-string pointed to by the return value of the function.
 * --------------------------------------------------------------
 */
const char *gamestate_get_prevmove_label( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	return _MVDIR_TO_LABEL( state->prevmv );
}

/* --------------------------------------------------------------
 * (Getter) const char *gamestate_get_nextmove_label():
 *
 * Return a pointer to the internal c-string corresponding to the
 * current enumerated value of the nextmv field, of the specified
 * game-state object (state), or NULL on error.
 *
 * NOTE (IMPORTANT!):
 *
 *   The caller should NOT attempt to modify the contents of
 *   the c-string pointed to by the return value of the function.
 * --------------------------------------------------------------
 */
const char *gamestate_get_nextmove_label( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	return _MVDIR_TO_LABEL( state->nextmv );
}

/* --------------------------------------------------------------
 * (Setter) int gamestate_set_board_reference():
 *
 * Change the internal pointer that points to the board object
 * of the specified game-state object (state), so it points to
 * the board object specified by the 2nd argument of the function.
 * Return 0 (false) on error, 1 (true) otherwise.
 *
 * NOTES (IMPORTANT!):
 *
 *    If not properly used, this function may not only LEAK memory
 *    but it may also cause the game to CRASH.
 *
 *    It must be CRYSTAL CLEAR that the function does NOT copy the
 *    specified board object into the specified game-state. Rather,
 *    it is brutally changing the internal pointer of the game-state
 *    board, so it points to the board specified by the 2nd argument.
 *    No copying, no freeing, no nothing!
 *
 *    Currently, the function is used exclusively by the function:
 *    tui_update_board_reference() which is defined in the file:
 *    "tui.c".
 * --------------------------------------------------------------
 */
int gamestate_set_board_reference( GameState *state, Board *board )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument (state)!" );
		return 0; /* false */
	}
	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument (board)!" );
		return 0; /* false */
	}

	state->board = board;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * (Setter) int gamestate_set_score():
 *
 * Set the value of the score field, of the specified game-state
 * object (state) to the value specified by the 2nd argument (score).
 * Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int gamestate_set_score( GameState *state, long int score )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0; /* false */
	}

	state->score = score;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * (Setter) int gamestate_set_bestscore():
 *
 * Set the value of the best-score field, of the specified game-state
 * object (state) to the value specified by the 2nd argument (bscore).
 * Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int gamestate_set_bestscore( GameState *state, long int bscore )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0; /* false */
	}

	state->bscore = bscore;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * (Setter) int gamestate_set_iswin():
 *
 * Set the boolean value of the iswin field, of the specified
 * game-state object (state) to the value specified by the 2nd
 * argument (iswin). Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int gamestate_set_iswin( GameState *state, int iswin )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0; /* false */
	}

	state->iswin = iswin;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * (Setter) int gamestate_set_prevmove():
 *
 * Set the enumerated value of the prevmv field, of the specified
 * game-state object (state) to the enumerated value specified by
 * the 2nd argument (prevmv). Return 0 (false) on error, 1 (true)
 * otherwise.
 * --------------------------------------------------------------
 */
int gamestate_set_prevmove( GameState *state, int prevmv )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0; /* false */
	}
	if ( !_VALID_MVDIR(prevmv) ) {
		DBGF( "Invalid move-direction (%d)!", prevmv );
		return 0; /* false */
	}

	state->prevmv = prevmv;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * (Setter) int gamestate_set_nextmove():
 *
 * Set the enumerated value of the nextmv field, of the specified
 * game-state object (state) to the enumerated value specified by
 * the 2nd argument (nextmv). Return 0 (false) on error, 1 (true)
 * otherwise.
 * --------------------------------------------------------------
 */
int gamestate_set_nextmove( GameState *state, int nextmv )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0; /* false */
	}
	if ( !_VALID_MVDIR(nextmv) ) {
		DBGF( "Invalid move-direction (%d)!", nextmv );
		return 0; /* false */
	}

	state->nextmv = nextmv;
	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int gamestate_append_to_fp():
 *
 * Serialize the specified game-state object (state) and append
 * it to the specified file (fp). Return 0 (false) on error, 1
 * (true) otherwise.
 *
 * NOTES:
 *    The serialization produces a text line of the form:
 *    "score bscore iswin prevmv nextmv@board-meta-data#board-tile-values\r\n"
 *
 *    For details about the serialized board-meta-data and
 *    board-tile-values, see the function: board_append_to_fp()
 *    defined in the file: board.c
 * --------------------------------------------------------------
 */
int gamestate_append_to_fp( const GameState *state, FILE *fp )
{
	if ( NULL == fp ) {
		DBGF( "%s", "NULL pointer argument (fp)!" );
		return 0;  /* false */
	}

	/* a NULL state is printed as "NULL@\r\n" */
	if ( NULL == state ) {
		if ( fprintf(fp, "%s", "NULL@\r\n") < 7 ) {
			DBGF( "%s", "fprintf() failed!" );
			return 0;  /* false */
		}
	}

	/* state->score + state->bscore + state->iswin
	 * + state->prevmv + state->nextmv
	 */
	if ( fprintf(
		fp, 
		"%ld %ld %d %d %d@",
		state->score,
		state->bscore,
		state->iswin,
		state->prevmv,
		state->nextmv
		) < 0
	){
		DBGF( "%s", "fprintf() failed!" );
		return 0;  /* false */
	}

	/* + state->board */
	if ( !board_append_to_fp(state->board, fp) ) {
		DBGF( "%s", "board_append_to_fp() failed!" );
		return 0;  /* false */
	}

	return 1;
}

/* --------------------------------------------------------------
 * GameState *new_gamestate_from_text():
 *
 * De-serialize the specified text and load it in a newly created
 * game-state object. Return a pointer to the new object, or NULL
 * on error.
 *
 * NOTES:
 *    Text is expected to be already serialized as following:
 *    "score bscore iswin prevmv nextmv@board-meta-data#board-tile-values\r\n"
 *
 *    For details about the serialized board-meta-data and
 *    board-tile-values, see the function: board_append_to_fp()
 *    defined in the file: board.c
 * --------------------------------------------------------------
 */
GameState *new_gamestate_from_text( char *text )
{
	Board     *board = NULL;      /* temporary board */
	GameState *state = NULL;      /* new game-state to be returned */
	char *tokens[2] = { NULL };
	int  ntokens=0, n=0;

	if ( NULL == text ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	/* tokenize text on '@' (up to 2 tokens) */
	ntokens = s_tokenize( text, tokens, 2, "@" );
	if ( ntokens < 2 ) {
		DBGF( "%s", "s_tokenize(text) failed to produce 2 tokens" );
		return NULL;
	}

	/*
	 * From the 2nd text-token, create a temp board. Then create the
	 * new game-state, copy the temp board into it & free the temp board.
	 */
	board = new_board_from_text( tokens[1] );
	if ( NULL == board ) {
		DBGF( "%s", "new_board_from_text() failed!" );
		goto ret_failure;
	}
	state = new_gamestate( board_get_dim(board) );
	if ( NULL == state ) {
		DBGF( "%s", "new_gamestate(board->dim) failed!" );
		goto ret_failure;
	}
	if ( !board_copy( state->board, board ) ) {
		DBGF( "%s", "board_copy(state->board, board) failed!" );
		goto ret_failure;
	}
	board = board_free( board );

	/* from the 1st token, get the state meta-data
	 * (score, bscore, iswin, prevmv)
         */
	n = sscanf(
		tokens[0],
		"%ld %ld %d %d %d",
		&state->score,
		&state->bscore,
		&state->iswin,
		&state->prevmv,
		&state->nextmv
		);
	if ( n < 5 ) {
		DBGF( "%s", "sscanf(tokens[0]) failed!" );
		goto ret_failure;
	}

	return state;

ret_failure:
	board_free( board );
	gamestate_free( state );
	return NULL;
}

/* --------------------------------------------------------------
 * GSNode *_make_node():
 *
 * Reserve memory for a gsstack node, with its state object having
 * a square board of the specified single-dimension (dim), and
 * return a pointer to it, or NULL on error.
 * --------------------------------------------------------------
 */
static inline GSNode *_make_node( int dim )
{
	GSNode *node = calloc( 1, sizeof(*node) );
	if ( NULL == node ) {
		DBGF( "%s", "calloc failed!" );
		return NULL;
	}

	node->state = new_gamestate( dim );
	if ( NULL == node->state ) {
		DBGF( "new_gamestate(%d) failed!", dim );
		free( node );
		return NULL;
	}

	return node;
}

/* --------------------------------------------------------------
 * GSNode *_node_free():
 *
 * Releases all resources occupied by the specified gsstack node
 * and return NULL (so the caller may assign it back to the node
 * pointer).
 * --------------------------------------------------------------
 */
static inline GSNode *_node_free( GSNode *node )
{
	if ( node ) {
		gamestate_free( node->state );
		free( node );
	}

	return NULL;
}

/* --------------------------------------------------------------
 * int gsstack_push():
 *
 * Push the specified game-state (state) to the top of the
 * specified gsstack (stack). Return 0 (false) on error,
 * 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int gsstack_push( GSNode **stack, const GameState *state )
{
	int dim;
	GSNode *newnode = NULL;

	if ( NULL == stack || NULL == state ) {
		DBGF( "%s", "NULL pointer argument" );
		return 0;  /* false */
	}

	/* alloc mem for new node */
	dim = board_get_dim( state->board );
	newnode = _make_node( dim );
	if ( NULL == newnode ) {
		DBGF( "_make_node(%d) failed!", dim );
		return 0;  /* false */
	}

	/* copy state to newnode & push the latter onto the stack */
	gamestate_copy( newnode->state, state );
	if ( NULL == (*stack) ) {          /* on empty stack */
		newnode->count = 1;
		newnode->down  = NULL;
		newnode->up    = NULL;
	}
	else {                             /* on non-empty stack */
		newnode->count = 1 + (*stack)->count;
		newnode->down  = *stack;
		newnode->up    = NULL;
		(*stack)->up   = newnode;
	}
	*stack = newnode;

	return 1;  /* true */

}

/* --------------------------------------------------------------
 * long int gsstack_peek_count():
 *
 * Return the value of the count field, of the top node of the
 * specified gsstack (stack), or 0 on error.
 *
 * NOTE: The count of nodes in a gsstack is 1-based.
 * --------------------------------------------------------------
 */
long int gsstack_peek_count( const GSNode *stack )
{
	if ( NULL == stack ) {
		return 0;
	}

	return stack->count;
}

/* --------------------------------------------------------------
 * const GameState *gsstack_peek_state():
 *
 * Return a pointer to the game-state object stored at the top
 * node of the specified gsstack (stack), or NULL on error.
 * --------------------------------------------------------------
 */
const GameState *gsstack_peek_state( const GSNode *stack )
{
	if ( NULL == stack ) {
		return NULL;
	}

	return stack->state;
}

/* --------------------------------------------------------------
 * int gsstack_pop():
 *
 * Remove the top node of the specified gsstack (stack).
 * Return 0 (false) on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int gsstack_pop( GSNode **stack )
{
	GSNode *temp = NULL;

	if ( NULL == stack ) {
		DBGF( "%s", "NULL pointer argument (stack)" );
		return 0;  /* false */
	}

	if ( NULL == *stack ) {
		return 0;  /* false */
	}

	temp = *stack;
	*stack = temp->down;
	if ( *stack ) {
		(*stack)->up = NULL;
	}
	_node_free( temp );

	return 1;  /* true */

}

/* --------------------------------------------------------------
 * GSNode *gsstack_dup_reversed():
 *
 * Create a new gsstack which is the reversed duplicate of the
 * specified gsstack (stack), and return a pointer to the top
 * node of the new gsstack, or NULL on error.
 * --------------------------------------------------------------
 */
GSNode *gsstack_dup_reversed( const GSNode *stack )
{
	GSNode *ret = NULL;

	if ( NULL == stack ) {
		DBGF( "%s", "NULL pointer argument (stack)" );
		return NULL;
	}

	while ( NULL != stack ) {
		gsstack_push( &ret, stack->state );
		stack = stack->down;
	}

	return ret;

}

/* --------------------------------------------------------------
 * const GSNode *gsstack_iter_top():
 *
 * Return a pointer to the top node of the specified gsstack
 * (stack), or NULL on error.
 * --------------------------------------------------------------
 */
const GSNode *gsstack_iter_top( const GSNode *stack )
{
	return stack;
}

/* --------------------------------------------------------------
 * const GSNode *gsstack_iter_bottom():
 *
 * Return a pointer to the bottom node of the specified gsstack
 * (stack), or NULL on error.
 * --------------------------------------------------------------
 */
const GSNode *gsstack_iter_bottom( const GSNode *stack )
{
	if ( NULL == stack ) {
		return NULL;
	}

	while ( stack->down ) {
		stack = stack->down;
	}
	return stack;
}

/* --------------------------------------------------------------
 * const GSNode *gsstack_iter_down():
 *
 * Given a pointer to a gsstack node (it) return a pointer to
 * the node below it, or NULL in error. 
 * --------------------------------------------------------------
 */
const GSNode *gsstack_iter_down( const GSNode *it )
{
	if ( NULL == it ) {
		return NULL;
	}
	return it->down;
}

/* --------------------------------------------------------------
 * const GSNode *gsstack_iter_up():
 *
 * Given a pointer to a gsstack node (it) return a pointer to
 * the node above it, or NULL in error. 
 * --------------------------------------------------------------
 */
const GSNode *gsstack_iter_up( const GSNode *it )
{
	if ( NULL == it ) {
		return NULL;
	}
	return it->up;
}

/* --------------------------------------------------------------
 * GSNode *gsstack_free():
 *
 * Destroy all nodes of the specified gsstack (stack), and return
 * NULL (so the caller may assign it back to the original pointer).
 * --------------------------------------------------------------
 */
GSNode *gsstack_free( GSNode **stack )
{
	if ( NULL == stack ) {
		DBGF( "%s", "NULL pointer argument (stack)" );
		return NULL;
	}

	while ( gsstack_pop(stack) ) {
		;  /* void */
	}
	return *stack;
}

/* --------------------------------------------------------------
 * int gsstack_append_to_fp():
 *
 * Serialize all nodes of the specified gsstack (stack) and append
 * them to the specified file (fp). Return 0 (false) on error,
 * 1 (true) otherwise.
 *
 * NOTES:
 *    The serialization of each node produces a text line of the form:
 *    "count:state-meta-data@state-board-meta-data#state-board-tile-values\r\n"
 *
 *    For details about the serialization see also the functions:
 *    gamestate_append_to_fp()
 *    board_append_to_fp() (defined in the file: board.c)
 * --------------------------------------------------------------
 */
int gsstack_append_to_fp( const GSNode *stack, FILE *fp )
{
	if  ( NULL == fp ) {
		DBGF( "%s", "NULL pointer argument (fp)" );
		return 0;  /* false */
	}

	/* a NULL stack is printed as "NULL:\r\n" */
	if ( NULL == stack ) {
		if ( fprintf(fp, "%s", "NULL:\r\n" ) != 7  ) {
			DBGF( "%s", "fprintf() failed!" );
			return 0;  /* false */
		}
	}

	while ( NULL != stack )
	{
		/* node's count */
		if ( fprintf(fp, "%ld:", stack->count) < 0 ) {
			DBGF( "%s", "fprintf() failed!" );
			return 0;  /* false */
		}

		/* + node's state */
		if ( !gamestate_append_to_fp(stack->state, fp) ) {
			DBGF( "%s", "gamestate_append_to_fp() failed!" );
			return 0;  /* false */
		}

		stack = stack->down;
	}

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * GSNode *new_gsnode_from_text():
 *
 * De-serialize the specified text and load it in a newly created
 * gsstack node. Return a pointer to the new node, or NULL on error.
 *
 * NOTES:
 *    Text is expected to be already serialized as following:
 *    "count:state-meta-data@state-board-meta-data#state-board-tile-values\r\n"
 *
 *    For details about the serialization see also the functions:
 *    gsstack_append_to_fp()
 *    gamestate_append_to_fp()
 *    board_append_to_fp() (defined in the file: board.c)
 * --------------------------------------------------------------
 */
GSNode *new_gsnode_from_text( char *text )
{
	GameState *state = NULL;      /* temporary state */
	GSNode    *node  = NULL;      /* new node to be returned */
	char *tokens[2] = { NULL };
	int  ntokens=0, n=0;

	if ( NULL == text ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	/* tokenize text on ':' (up to 2 tokens) */
	ntokens = s_tokenize( text, tokens, 2, ":" );
	if ( ntokens < 2 ) {
		DBGF( "%s", "s_tokenize(text) failed to produce 2 tokens" );
		return NULL;
	}

	/*
	 * From the 2nd text-token, create a temp gamesstate. Then create
	 * the new node, copy the temp game-state into it and free the temp
	 * game-state.
	 */
	state = new_gamestate_from_text( tokens[1] );
	if ( NULL == state ) {
		DBGF( "%s", "new_gamestate_from_text(tokens[1]) failed!" );
		goto ret_failure;
	}
	node = _make_node( board_get_dim( gamestate_get_board(state) ) );
	if ( NULL == node ) {
		DBGF( "%s", "_make_node failed!" );
		goto ret_failure;
	}

	if ( !gamestate_copy(node->state, state) ) {
		DBGF( "%s", "gamestate_copy(node->state, state) failed!" );
		goto ret_failure;
	}
	state = gamestate_free( state );

	/* from the 1st token, get the node meta-data (count) */
	n = sscanf( tokens[0], "%ld", &node->count );
	if ( n < 1 ) {
		DBGF( "%s", "sscanf(tokens[0]) failed!" );
		goto ret_failure;
	}

	return node;

ret_failure:
	gamestate_free( state );
	_node_free( node );
	return NULL;
}

/* --------------------------------------------------------------
 * For DEBUGGING Purposes: ***
 *
 * Dump the contents of the specified node on stdout.
 * --------------------------------------------------------------
 */
void dbg_gsnode_dump( GSNode *node )
{
	putchar( '\n' );
	if ( !node ) {
		puts( "GSNode is empty" );
		return;
	}

	printf( "count: %ld\n", node->count );
	puts( "state:" );
	dbg_board_dump( node->state->board );
	printf( "\tscore: %ld\n", node->state->score );
	printf( "\tbscore: %ld\n", node->state->bscore );
	printf( "\tnextmv: %d\n", node->state->prevmv );

	printf( "&down: 0x%p\n", (void *)(node->down) );
	printf( "&up  : 0x%p\n", (void *)(node->up) );
}

/* --------------------------------------------------------------
 * For DEBUGGING Purposes: ***
 *
 * Dump the contents of the specified stack on stdout.
 * --------------------------------------------------------------
 */
void dbg_gsstack_dump( GSNode *stack )
{
	putchar( '\n' );
	if ( !stack ) {
		puts( "stack is empty" );
		return;
	}

	while ( stack ) {
		dbg_gsnode_dump( stack );
		stack = stack->down;
	}
}
