/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 11, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: common.h, board.h, gs.h
 * --------------------------------------------------------------
 *
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

#define _VALID_MVDIR(dir)           \
(                                   \
	(dir) == GS_MVDIR_NONE      \
	|| (dir) == GS_MVDIR_UP     \
	|| (dir) == GS_MVDIR_DOWN   \
	|| (dir) == GS_MVDIR_LEFT   \
	|| (dir) == GS_MVDIR_RIGHT  \
)

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

/* Game-state at any time */
struct _GameState {
	Board    *board;
	long int score;        /* current score */
	long int bscore;       /* best-score across multiple games */
	int      prevmv;       /* direction of previous move */
	int      nextmv;       /* direction of next move */
	int      iswin;        /* has the sentinel value reached? */
};

/* Single node for game-state stacks */
struct _GSNode{
	long int       count;   /* node's count (1-based) */
	GameState      *state;  /* the game-state stored in the node*/
	struct _GSNode *down;   /* previous node (towards bottom) */
	struct _GSNode *up;     /* next node (towards top) */
};

/* --------------------------------------------------------------
 *
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
 *
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
 *
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
 *
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
 *
 * --------------------------------------------------------------
 */
Board *gamestate_get_board( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	return state->board;
}

/* --------------------------------------------------------------
 *
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
 *
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
 *
 * --------------------------------------------------------------
 */
int gamestate_get_iswin( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}

	return state->iswin;
}

/* --------------------------------------------------------------
 *
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
 *
 * --------------------------------------------------------------
 */
const char *gamestate_get_prevmove_label( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return GS_MVDIR_NONE;
	}

	return _MVDIR_TO_LABEL( state->prevmv );
}

/* --------------------------------------------------------------
 *
 * --------------------------------------------------------------
 */
const char *gamestate_get_nextmove_label( const GameState *state )
{
	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument!" );
		return GS_MVDIR_NONE;
	}

	return _MVDIR_TO_LABEL( state->nextmv );
}

/* --------------------------------------------------------------
 *
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
 *
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
 *
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
 *
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
 *
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
 *
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
 * --------------------------------------------------------------
 */
int gamestate_append_to_fp( const GameState *state, FILE *fp )
{
	if ( NULL == fp ) {
		DBGF( "%s", "NULL pointer argument (fp)!" );
		return 0;  /* false */
	}

	/* a NULL state is printed as "NULL@\n" */
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
 * NOTES: The expected format of the text is the following:
 *        "score bscore iswin prevmv@board-meta-data#board-tile-values"
 * --------------------------------------------------------------
 */
GameState *new_gamestate_from_text( char *text )
{
	Board     *board = NULL;      /* temporary board */
	GameState *state = NULL;      /* new gamestate to be returned */
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
	 * new gamestate, copy the temp board into it & free the temp board.
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
 *
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
 *
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
 *
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
 *
 * --------------------------------------------------------------
 */
const GSNode *gsstack_peek( const GSNode *stack )
{
	return stack;
}

/* --------------------------------------------------------------
 *
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
 *
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
 *
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
 *
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
 *
 * --------------------------------------------------------------
 */
const GSNode *gsstack_iter_top( const GSNode *stack )
{
	return stack;
}

/* --------------------------------------------------------------
 *
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
 *
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
 *
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
 *
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
 *
 * --------------------------------------------------------------
 */
int gsstack_append_to_fp( const GSNode *stack, FILE *fp )
{
	if  ( NULL == fp ) {
		DBGF( "%s", "NULL pointer argument (fp)" );
		return 0;  /* false */
	}

	/* a NULL stack is printed as "NULL:\n" */
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
 * NOTES: The expected format of the text is the following:
 *        "count:state-meta-data@state-board-meta-data#state-board-tile-values"
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
	 * the new node, copy the temp gamestate into it and free the temp
	 * gamestate.
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
