/****************************************************************
 * This file is part of the "2048 Console Clone" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a2
 * Date:         July 9, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: common.h, gs.h, mvhist.h
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#define MVHIST_C

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gs.h"
#include "mvhist.h"

/* Holds the undo and redo stacks (see also: gs.c) */
struct _MovesHistory {
	int    didundo;    /* has the player done at least 1 undo? */
	GSNode *undo;      /* undo stack (stores game-states) */
	GSNode *redo;      /* redo stack (stores game-states) */
	struct {
		unsigned long int delay; /* time delay during replay (msecs)*/
		long int nmoves;         /* length of replay-stack */
		long int itcount;        /* count of node under iterator */
		GSNode   *stack;         /* the replay-stack */
	} replay;
};

/* --------------------------------------------------------------
 * void new_mvhist():
 *
 * Create a MovesHistory object with default values in memory and
 * return a pointer to it, or NULL on error.
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
 * void mvhist_free():
 *
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
 * void mvhist_reset():
 *
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
 * GSNode *mvhist_new_replay_stack():
 *
 * --------------------------------------------------------------
 */
GSNode *mvhist_new_replay_stack( MovesHistory *mvhist, unsigned int delay )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
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
 int mvhist_free_replay_stack():
 *
 * --------------------------------------------------------------
 */
int mvhist_free_replay_stack( MovesHistory *mvhist )
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
 * const GameState *mvhist_peek_replay_stack_state():
 *
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
 * --------------------------------------------------------------
 */
int mvhist_pop_replay_stack( MovesHistory *mvhist )
{
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)" );
		return 0;  /* false */
	}

	if ( !gsstack_pop(&mvhist->replay.stack) ) {
		return 0;  /* false */
	}

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * unsigned long int mvhist_get_replay_delay():
 *
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
 * const GSNode *mvhist_iter_top_replay_stack():
 *
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
 * int mvhist_get_didundo():
 *
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
 * int mvhist_set_didundo():
 *
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
 * char *_replay_to_text():
 *
 * --------------------------------------------------------------
 */
static inline char *_replay_to_text( const MovesHistory *mvhist )
{
	char *txtout = NULL, *test = NULL, *tmp = NULL;

	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return NULL;
	}

	/* mvhist->replay.delay
	 * + mvhist->replay.nmoves
	 * + mvhist->replay.itcount
	 */
	test = printf_to_text(
		"%s%lu %ld %ld\r\n",
		NULL == txtout ? "\0" : txtout,
		mvhist->replay.delay,
		mvhist->replay.nmoves,
		mvhist->replay.itcount
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;

	/* + mvhist->replay.stack */
	tmp = gsstack_to_text( mvhist->replay.stack );
	if ( NULL == tmp ) {
		DBGF( "%s", "gsstack_to_text() failed!" );
		goto ret_failure;
	}
	test = printf_to_text(
		"%s%s",
		NULL == txtout ? "\0" : txtout,
		tmp
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;
	free( tmp );

	return txtout;

ret_failure:
	free( tmp );
	free( txtout );
	return NULL;
}

/* --------------------------------------------------------------
 * char *mvhist_to_text():
 *
 * --------------------------------------------------------------
 */
char *mvhist_to_text( const MovesHistory *mvhist )
{
	char *txtout = NULL, *test = NULL, *tmp = NULL;

	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return NULL;
	}

	/* mvhist->didundo */
	test = printf_to_text(
		"%s%d\r\n",
		NULL == txtout ? "\0" : txtout,
		mvhist->didundo
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;

	/* + mvhist->undo (stack) */
	tmp = gsstack_to_text( mvhist->undo );
	if ( NULL == tmp ) {
		DBGF( "%s", "gsstack_to_text() failed!" );
		goto ret_failure;
	}
	test = printf_to_text(
		"%s%s",
		NULL == txtout ? "\0" : txtout,
		tmp
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;
	free( tmp );

	/* + mvhist->redo (stack) */
	tmp = gsstack_to_text( mvhist->redo );
	if ( NULL == tmp ) {
		DBGF( "%s", "gsstack_to_text() failed!" );
		goto ret_failure;
	}
	test = printf_to_text(
		"%s%s",
		NULL == txtout ? "\0" : txtout,
		tmp
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;
	free( tmp );

	/* + mvhist->replay */
	tmp = _replay_to_text( mvhist );
	if ( NULL == tmp ) {
		DBGF( "%s", "_replay_to_text() failed!" );
		goto ret_failure;
	}
	test = printf_to_text(
		"%s%s",
		NULL == txtout ? "\0" : txtout,
		tmp
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;
	free( tmp );

	return txtout;

ret_failure:
	free( tmp );
	free( txtout );
	return NULL;
}

/* --------------------------------------------------------------
 * int mvhist_save_to_file():
 *
 * --------------------------------------------------------------
 */
int mvhist_save_to_file( const MovesHistory *mvhist, const char *fname )
{
	FILE *fp  = NULL;
	char *tmp = NULL;

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
	if ( fprintf(fp, "%d\r\n", mvhist->didundo) < 1 ) {
		DBGF( "%s", "fprintf() failed!" );
		goto ret_failure;
	}

	/* + mvhist->undo (stack) */
	tmp = gsstack_to_text( mvhist->undo );
	if ( NULL == tmp ) {
		DBGF( "%s", "gsstack_to_text(mvhist->undo) failed!" );
		goto ret_failure;
	}
	if ( fprintf(fp, "%s", tmp) < 1 ) {
		DBGF( "%s", "fprintf() failed!" );
		goto ret_failure;
	}
	free( tmp );

	/* + mvhist->redo (stack) */
	tmp = gsstack_to_text( mvhist->redo );
	if ( NULL == tmp ) {
		DBGF( "%s", "gsstack_to_text(mvhist->redo) failed!" );
		goto ret_failure;
	}
	if ( fprintf(fp, "%s", tmp) < 1 ) {
		DBGF( "%s", "fprintf() failed!" );
		goto ret_failure;
	}
	free( tmp );

	/* + mvhist->replay */
	tmp = _replay_to_text( mvhist );
	if ( NULL == tmp ) {
		DBGF( "%s", "_replay_to_text() failed!" );
		goto ret_failure;
	}
	if ( fprintf(fp, "%s", tmp) < 1 ) {
		DBGF( "%s", "fprintf() failed!" );
		goto ret_failure;
	}
	free( tmp );

	fclose( fp );
	return 1;  /* true */

ret_failure:
	fclose( fp );
	free( tmp );
	return 0;  /* false */
}

/* --------------------------------------------------------------
 * int _load_stack_from_fp_line():
 *
 * --------------------------------------------------------------
 */
static inline int _load_stack_from_fp_line(
	GSNode       **stack,
	FILE         *fp,
	char         *line,
	size_t       lnsize
	)
{
	int i = 0;
	long int count;
	GSNode *node = NULL;

	if ( 0 == strcmp(line, "NULL:\n") ) {
		return 1;  /* true */
	}

	if ( sscanf(line, "%ld", &count) < 1 ) {
		DBGF( "%s", "sscanf() failed to read count of stack" );
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
	GSNode *revstack = gsstack_dup_reversed( *stack );
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

	/* read the undo-stack */
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read undo-stack line" );
		goto ret_failure;
	}
	s_fixeol( line );
	if (!_load_stack_from_fp_line(&mvh->undo, fp, line, MAX_LNSIZE)){
		DBGF( "%s", "_load_stack_from_fp_line(undo) failed!" );
		goto ret_failure;
	}

	/* read the redo-stack */
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBGF( "%s", "fgets() failed to read redo-stack line" );
		goto ret_failure;
	}
	s_fixeol( line );
	if (!_load_stack_from_fp_line(&mvh->redo, fp, line, MAX_LNSIZE)){
		DBGF( "%s", "_load_stack_from_fp_line(redo) failed" );
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
		DBGF( "%s", "fgets() failed to read redo-stack line" );
		goto ret_failure;
	}
	s_fixeol( line );
	if (!_load_stack_from_fp_line(&mvh->replay.stack, fp, line, MAX_LNSIZE)) {
		DBGF( "%s", "_load_stack_from_fp_line(replay.stack) failed" );
		goto ret_failure;
	}

	fclose( fp );

	return mvh;

ret_failure:
	fclose( fp );
	free( mvh );
	return NULL;
}
