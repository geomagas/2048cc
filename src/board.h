/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 20, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * --------------------------------------------------------------
 *
 * The public interface of the Board "class".
 ****************************************************************
 */

#ifndef BOARD_H
#define BOARD_H

/* The "class" is forward-declared as an opaque data-type */
typedef struct _board Board;

/* Supported singe-dimensions for the game board. */
enum {
	BOARD_DIM_4 = 4,    /* 4x4 board (default) */
	BOARD_DIM_5 = 5,    /* 5x5 board */
	BOARD_DIM_6 = 6,    /* 6x6 board */
	BOARD_DIM_8 = 8     /* 8x8 board */
};

#ifndef BOARD_C
extern Board *make_board( int dim );
extern Board *new_board( void );
extern Board *board_free( Board *board );

extern int   board_reset( Board *board );
extern int   board_resize_and_reset( Board *board, int dim );
extern int   board_copy( Board *dst, const Board *src );
extern int   board_generate_ntiles( Board *board, int n );

extern int   board_has_room( const Board *board );
extern int   board_has_adjacent( const Board *board );

extern int   board_move_up( Board *board, long int *score, int *won );
extern int   board_move_down( Board *board, long int *score, int *won );
extern int   board_move_left( Board *board, long int *score, int *won );
extern int   board_move_right( Board *board, long int *score, int *won );

extern int   board_get_dim( const Board *board );
extern int   board_get_sentinel( const Board *board );
extern int   board_get_nrandom( const Board *board );
extern int   board_get_tile_value( const Board *board, int i, int j );
extern int   board_get_nempty( const Board *board );

extern int    board_append_to_fp( const Board *board, FILE *fp );
extern Board *new_board_from_text( char *text );

extern int   dbg_board_dump( const Board *board );
extern int   dbg_board_generate_tile( Board *board, int i, int j );
#endif

#endif
