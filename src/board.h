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

#ifndef BOARD_H
#define BOARD_H

/* supported singe-dimensions for the game board */
enum {
	BOARD_DIM_4 = 4,    /* 4x4 board (default) */
	BOARD_DIM_5 = 5,    /* 5x5 board */
	BOARD_DIM_6 = 6,    /* 6x6 board */
	BOARD_DIM_8 = 8     /* 8x8 board */
};


#ifndef BOARD_C
typedef struct _board Board;  /* forward declaration of opaque data type */

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

extern char  *board_to_text( const Board *board );
extern Board *new_board_from_text( char *text );

extern int   dbg_board_dump( const Board *board );
extern int   dbg_board_generate_tile( Board *board, int i, int j );
#endif

#endif
