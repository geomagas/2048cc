/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 11, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: common.h, board.h
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#define BOARD_C

#include <stdlib.h>        /* rand(), malloc(), calloc(), realloc() */
#include <string.h>        /* memset(), memcpy() */

#include "common.h"
#include "board.h"

/* Macro for indexing the grid of the board (it's an 1D buffer). */
#define _IDX(i,j,ncols)    ( (i) * (ncols) + (j) )

/* Validation macro for the supported single-dimensions of a board.
 * They are defined in "board.h".
 */
#define _VALID_DIM(dim)          \
(                                \
	(dim) == BOARD_DIM_4     \
	|| (dim) == BOARD_DIM_5  \
	|| (dim) == BOARD_DIM_6  \
	|| (dim) == BOARD_DIM_8  \
)
enum {
	_VAL_SENTINEL_4 = 2048,  /* for 4x4 board (default) */
	_VAL_SENTINEL_5 = 65536, /* for 5x5 board */
	_VAL_SENTINEL_6 = 2048,  /* for 6x6 board */
	_VAL_SENTINEL_8 = 16384  /* for 8x8 board */
};

/* Count of tiles to be randomly generated after every successful move.
 * NOTE: An empty board starts with 2 * count of tiles.
 *       After that, each successful move generates 1 * count of tiles.
 */
enum {
	_NRANDOM_4 = 1,          /* default */
	_NRANDOM_6 = 2
};

/* Structure of a single tile.
 * Since it contains just one field, it could be omitted.
 * However, this design gives me flexibility to add more
 * fields if needed in future versions.
 */
struct _tile {
	int val;             /* the value of the tile */
};

typedef struct _board Board;
struct _board {
	int  dim;            /* single dimension (grid is a square) */
	int  sentinel;       /* sentinel value (e.g. for 4x4 it is 2048) */
	int  nrandom;        /* # of random generated tiles after a move */
	int  nempty;         /* # of currently empty slots */
	int  hasadjacent;    /* are there 2 adjacent tiles with equal val? */
	struct _tile *grid;
};

/* --------------------------------------------------------------
 * int _dim_to_sentinel():
 *
 * Return the sentinel-value that corresponds to the specified
 * single-dimension of the board. On error, assume a 4x4 board.
 * (The sentinel-value is the value which when reached in a tile,
 * the player wins the game).
 *
 * NOTE: For a 4x4 board, the sentinel-value is 2048, but for
 *       a 5x5 board the sentinel-value is 16384, etc. See the
 *       enumerated definitions _VAL_SENTINEL_X at the top of
 *       this file.
 * --------------------------------------------------------------
 */
static inline int _dim_to_sentinel( int dim )
{
	switch (dim) {
		case BOARD_DIM_4:
			return _VAL_SENTINEL_4;

		case BOARD_DIM_5:
			return _VAL_SENTINEL_5;

		case BOARD_DIM_6:
			return _VAL_SENTINEL_6;

		case BOARD_DIM_8:
			return _VAL_SENTINEL_8;
		default:
			break;
	}

	return _VAL_SENTINEL_4;
}

/* --------------------------------------------------------------
 * int _dim_to_nrandom():
 *
 * Return the count of tiles to be generated randomly after every
 * successful move, for a board with the specified single-dimension.
 * On error, assume a 4x4 board.
 *
 * NOTES: For most boards we generate 1 random tile after a move,
 *        but for a 6x6 board we generate 2 tiles after a move, etc.
 *        See the enumerated definitions _NRANDOM_X at the top of
 *        this file.
 *
 *        On an empty board (that is when starting a new game)
 *        the return value of this function should be doubled.
 * --------------------------------------------------------------
 */
static inline int _dim_to_nrandom( int dim )
{
	switch (dim) {
		case BOARD_DIM_4:
		case BOARD_DIM_5:
		case BOARD_DIM_8:
			return _NRANDOM_4;

		case BOARD_DIM_6:
			return _NRANDOM_6;

		default:
			break;
	}

	return _NRANDOM_4;
}

/* --------------------------------------------------------------
 * int board_generate_tile():
 *
 * Generate a random value between 2 and 4 and put it at a random
 * empty tile of the specified board. Return 0 (false) on error,
 * 1 (true) otherwise.
 * --------------------------------------------------------------
 */
static inline void _generate_tile( Board *board )
{
	int i,j,idx;
	const int DIM = board->dim;
	do {
		i = rand() % DIM;
		j = rand() % DIM;
		idx = _IDX(i,j,DIM);
	} while ( 0 != board->grid[idx].val );

	/* val must be 2 or 4 */
	board->grid[idx].val = (rand() % 2 == 0) ? 2 : 4;

	board->nempty--;

}

/* --------------------------------------------------------------
 * int _row_has_gaps_begend():
 *
 * Return 1 (true) if the specified row (i) has gaps (0 values) between
 * its elements. The row is scanned from left (beg) to right (end).
 * Return 0 (false) if no gaps were found.
 *
 * NOTE: Used exclusively in board_move_left(), so it depends
 *       heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline int _row_has_gaps_begend( const Board *board, int i )
{
	int idx;
	const int DIM = board->dim;
	const int BEG = _IDX(i,0,DIM);  /* idx of 1st elem in ith row */
	const int END = BEG + DIM-1;    /* idx of last elem in ith row */

	/* A gap is when 2 adjacent cells contain a 0 and a non-0
	 * value, respectively (note that END is an index, not a length).
	 */
	for (idx=BEG; idx < END; idx++)
	{
		if ( 0 == board->grid[idx].val
		&& 0 != board->grid[idx+1].val
		){
			return 1;
		}
	}

	return 0;
}

/* --------------------------------------------------------------
 * int _row_has_gaps_endbeg():
 *
 * Return 1 (true) if the specified row (i) has gaps (0 values) between
 * its elements. The row is scanned from right (end) to left (beg).
 * Return 0 (false) if no gaps were found.
 *
 * NOTE: Used exclusively in board_move_right(), so it depends
 *       heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline int _row_has_gaps_endbeg( const Board *board, int i )
{
	int idx;
	const int DIM = board->dim;
	const int BEG = _IDX(i,0,DIM);  /* idx of 1st elem in ith row */
	const int END = BEG + DIM-1;    /* idx of last elem in ith row */

	/* A gap is when 2 adjacent cells contain a 0 and a non-0
	 * value, respectively (note that END is an index, not a length).
	 */
	for (idx=END; idx > BEG; idx--)
	{
		if ( 0 == board->grid[idx].val
		&& 0 != board->grid[idx-1].val
		){
			return 1;
		}
	}

	return 0;
}

/* --------------------------------------------------------------
 * int _row_has_adjacent():
 *
 * Return 1 (true) if the specified row has adjacent cells with
 * equal non-0 values, otherwise return 0 (false).
 *
 * NOTE: Used exclusively in the functions board_move_left()
 *       and board_move_right(), so it depends heavily on
 *       assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline int _row_has_adjacent( const Board *board, int i )
{
	int idx;
	const int DIM = board->dim;
	const int BEG = _IDX(i,0,DIM);  /* idx of 1st elem in ith row */
	const int END = BEG + DIM-1;    /* idx of last elem in ith row */

	for (idx=BEG; idx < END; idx++)
	{
		if ( 0 == board->grid[idx].val ) {
			continue;
		}
		if ( board->grid[idx].val == board->grid[idx+1].val ) {
			return 1;
		}
	}
	return 0;
}

/* --------------------------------------------------------------
 * void _row_backup_nogaps_and_clear():
 *
 * Copy all non-0 values from the specified row to the specified
 * backup buffer, and then zero the original values in the row.
 *
 * NOTE: Used exclusively in the functions board_move_left()
 *       and board_move_right(), so it depends heavily on
 *       assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline void _row_backup_nogaps_and_clear(
	Board *board,
	int   i,
	int   backup[]
	)
{
	int k,idx;
	const int DIM = board->dim;
	const int BEG = _IDX(i,0,DIM);     /* idx of 1st elem in ith row */
	const int END_PLUS_ONE = BEG + DIM;/* idx of last+1 elem in ith row*/

	memset( backup, 0, DIM * sizeof(int) );
	for (idx=BEG, k=0; idx < END_PLUS_ONE; idx++)
	{
		if ( 0 != board->grid[idx].val ) {
			backup[k++] = board->grid[idx].val;
			board->grid[idx].val = 0;
		}
	}
}

/* --------------------------------------------------------------
 * void _row_restore_merged_nogaps_begend():
 *
 * Copy all non-0 values from the specified backup buffer to
 * the specified row (i) of the board. Traversing is done from
 * left (beg) to right (end). The backup buffer is expected to
 * contain already merged elements.
 *
 * NOTE: Used exclusively in the function board_move_left(), so
 *       it depends heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline void _row_restore_merged_nogaps_begend(
	Board *board,
	int   i,
	int   backup[]
	)
{
	int k,idx;
	const int DIM = board->dim;
	const int BEG = _IDX(i,0,DIM);     /* idx of 1st elem in ith row */

	for (k=0, idx=BEG; k < DIM; k++) {
		if ( 0 != backup[k] ) {
			board->grid[idx].val = backup[k];
			idx++;
		}
	}
}

/* --------------------------------------------------------------
 * void _row_restore_merged_nogaps_endbeg():
 *
 * Copy all non-0 values from the specified backup buffer to
 * the specified row (i) of the board. Traversing is done from
 * right (end) to left (beg). The backup buffer is expected to
 * contain already merged elements.
 *
 * NOTE: Used exclusively in the function board_move_right(),
 *       so it depends heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline void _row_restore_merged_nogaps_endbeg(
	Board *board,
	int   i,
	int   backup[]
	)
{
	int k,idx;
	const int DIM = board->dim;
	const int END = _IDX(i,DIM-1,DIM); /* idx of last elem in ith row */

	for (k=DIM-1, idx=END; k > -1; k--) {
		if ( 0 != backup[k] ) {
			board->grid[idx].val = backup[k];
			idx--;
		}
	}
}

/* --------------------------------------------------------------
 * int _col_has_gaps_begend():
 *
 * Return 1 (true) if the specified board column (j) has any gaps
 * (0 values) between its elements. The column is scanned from
 * top (beg) to bottom (end).
 * Return 0 (false) if no gaps were found.
 *
 * NOTE: Used exclusively in board_move_up(), so it depends heavily
 *       on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline int _col_has_gaps_begend( const Board *board, int j )
{
	int idx;
	const int DIM = board->dim;
	const int BEG = _IDX(0,j,DIM);    /* idx of 1st elem in jth column */
	const int END = _IDX(DIM-1,j,DIM);/* idx of last elem in jth column*/

	/* A gap is when 2 adjacent cells contain a 0 and a non-0
	 * value, respectively.
	 */
	for (idx=BEG; idx < END; idx += DIM)
	{
		if ( 0 == board->grid[idx].val
		&& 0 != board->grid[idx+DIM].val
		){
			return 1;
		}
	}

	return 0;
}

/* --------------------------------------------------------------
 * int _col_has_gaps_endbeg():
 *
 * Return 1 (true) if the specified board column (j) has any gaps
 * (0 values) between its elements. The column is scanned from
 * bottom (end) to top (beg). Return 0 (false) if no gaps were
 * found.
 *
 * NOTE: Used exclusively in board_move_down(), so it depends
 *       heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline int _col_has_gaps_endbeg( const Board *board, int j )
{
	int idx;
	const int DIM = board->dim;
	const int BEG = _IDX(0,j,DIM);    /* idx of 1st elem in jth column */
	const int END = _IDX(DIM-1,j,DIM);/* idx of last elem in jth column*/

	/* A gap is when 2 adjacent cells contain a 0 and a non-0
	 * value, respectively.
	 */
	for (idx=END; idx > BEG; idx -= DIM)
	{
		if ( 0 == board->grid[idx].val
		&& 0 != board->grid[idx-DIM].val
		){
			return 1;
		}
	}

	return 0;
}

/* --------------------------------------------------------------
 * int _col_has_adjacent():
 *
 * Return 1 (true) if the specified board column (j) has adjacent
 * cells with equal non-0 values, otherwise return 0 (false).
 *
 * NOTE: Used exclusively in the functions board_move_up() and
 *       board_move_down(), so it depends heavily on assumptions
 *       made in that context.
 * --------------------------------------------------------------
 */
static inline int _col_has_adjacent( const Board *board, int j )
{
	int idx;
	const int DIM = board->dim;
	const int BEG = _IDX(0,j,DIM);    /* idx of 1st elem in jth column */
	const int END = _IDX(DIM-1,j,DIM);/* idx of last elem in jth column*/

	for (idx=BEG; idx < END; idx += DIM)
	{
		if ( 0 == board->grid[idx].val ) {
			continue;
		}
		if ( board->grid[idx].val == board->grid[idx+DIM].val ) {
			return 1;
		}
	}

	return 0;
}

/* --------------------------------------------------------------
 * void _col_backup_nogaps_and_clear():
 *
 * Copy all non-0 values from the specified board column (j) to
 * the specified backup buffer, and then zero the original values
 * in the column.
 *
 * NOTE: Used exclusively in the functions board_move_up() and
 *       board_move_down(), so it depends heavily on assumptions
 *       made in that context.
 * --------------------------------------------------------------
 */
static inline void _col_backup_nogaps_and_clear(
	Board *board,
	int   j,
	int   backup[]
	)
{
	int idx,k;
	const int DIM = board->dim;
	const int BEG = _IDX(0,j,DIM);    /* idx of 1st elem in jth column */
	const int END_PLUS_ONE = _IDX(DIM,j,DIM);/* idx of last+1 elem in jth column*/

	memset( backup, 0, DIM * sizeof(int) );
	for (idx=BEG, k=0; idx < END_PLUS_ONE; idx += DIM)
	{
		if ( 0 != board->grid[idx].val ) {
			backup[k++] = board->grid[idx].val;
			board->grid[idx].val = 0;
		}
	}
}

/* --------------------------------------------------------------
 * void _col_restore_merged_nogaps_begend():
 *
 * Copy all non-0 values from the specified backup buffer to
 * the specified column (j) of the board. Traversing is done
 * from left (beg) to right (end) in the backup buffer, and
 * from top (beg) to bottom (end) in the column. The backup
 * buffer is expected to contain already merged elements.
 *
 * NOTE: Used exclusively in the function board_move_up(), so
 *       it depends heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline void _col_restore_merged_nogaps_begend(
	Board *board,
	int   j,
	int   backup[]
	)
{
	int k,idx;
	const int DIM = board->dim;
	const int BEG = _IDX(0,j,DIM);  /* idx of 1st elem in jth column */

	for (k=0,idx=BEG; k < DIM; k++) {
		if ( 0 != backup[k] ) {
			board->grid[idx].val = backup[k];
			idx += DIM;
		}
	}

}

/* --------------------------------------------------------------
 * void _col_restore_merged_nogaps_endbeg():
 *
 * Copy all non-0 values from the specified backup buffer to
 * the specified column (j) of the board. Traversing is done
 * from right (end) to left (beg) in the backup buffer, and
 * from bottom (end) to top (beg) in the column. The backup
 * buffer is expected to contain already merged elements.
 *
 * NOTE: Used exclusively in the function board_move_down(), so
 *       it depends heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline void _col_restore_merged_nogaps_endbeg(
	Board *board,
	int   j,
	int   backup[]
	)
{
	int k,idx;
	const int DIM = board->dim;
	const int END = _IDX(DIM-1,j,DIM); /* idx of last elem in jth column*/

	for (k=DIM-1, idx=END; k > -1; k--) {
		if ( 0 != backup[k] ) {
			board->grid[idx].val = backup[k];
			idx -= DIM;
		}
	}
}

/* --------------------------------------------------------------
 * int _merge_adjacent_from_beg():
 *
 * Starting from the beginning of the specified backup buffer,
 * merge adjacent cells if they have equal non-0 values.
 * Return 1 (true) if at least one merge was made, or 0 (false)
 * otherwise.
 *
 * NOTES:
 * 1.  Used exclusively in the functions board_move_up() and
 *     board_move_left(), so it depends heavily on assumptions
 *     made in that context. For example, for those two functions,
 *     it is very IMPORTANT that the backup buffer is scanned
 *     from start to end (and not the other way around).
 *
 * 2.  After each merge, the score of the player, his winning
 *     status and the board counter of empty slots are updated
 *     and passed to the caller via the function's corresponding
 *     arguments.
 *
 * 3.  After each merge, the 2nd element of the merged pair is
 *     zeroed inside the backup buffer, meaning that gaps are
 *     created. This in turn means, that when the backup buffer
 *     will be copied back to the board, those gaps should be
 *     skipped.
 * --------------------------------------------------------------
 */
static inline int _merge_adjacent_from_beg(
	int      backup[],
	Board    *board,
	long int *score,
	int      *won
	)
{
	int k;
	int ret = 0;
	const int DIM      = board->dim;
	const int SENTINEL = board->sentinel;

	for (k=0; k < DIM-1; k++)
	{
		if ( 0 == backup[k] ) {
			continue;
		}
		if ( backup[k] == backup[k+1] ) {
			backup[k] += backup[k+1];
			backup[k+1] = 0;
			if ( backup[k] <= SENTINEL ) {
				(*score) += backup[k];
			}
			if ( SENTINEL == backup[k] ) {
				*won = 1;  /* true */
			}
			k++;
			board->nempty++;
			ret = 1;  /* true */

		}
	}
	return ret;
}

/* --------------------------------------------------------------
 * int _merge_adjacent_from_end():
 *
 * Starting from the end of the specified backup buffer, merge adjacent
 * cells if they have equal non-0 values. Return 1 (true) if at least
 * one merge was made, or 0 (false) otherwise.
 *
 * NOTES:
 *
 * 1.    !!IMPORTANT!!
 *       Read the comments of the function _merge_adjacent_from_beg()
 *       which does a similar job but it scans the backup buffer from
 *       end to start.
 *
 * 2.    This function is used exclusively in the functions board_move_down()
 *       & board_move_right(), so it performs merges from end to start.
 * --------------------------------------------------------------
 */
static inline int _merge_adjacent_from_end(
	int      backup[],
	Board    *board,
	long int *score,
	int      *won
	)
{
	int k;
	int ret = 0;
	const int DIM      = board->dim;
	const int SENTINEL = board->sentinel;

	for (k=DIM-1; k > 0; k--)
	{
		if ( 0 == backup[k] ) {
			continue;
		}
		if ( backup[k] == backup[k-1] ) {
			backup[k] += backup[k-1];
			backup[k-1] = 0;
			if ( backup[k] <= SENTINEL ) {
				(*score) += backup[k];
			}
			if ( SENTINEL == backup[k] ) {
				*won = 1;  /* true */
			}
			k--;
			board->nempty++;
			ret = 1;  /* true */

		}
	}
	return ret;
}

/* --------------------------------------------------------------
 * int _has_hadjacent():
 *
 * Return 1 (true) if the specified board has equal, non-0, adjacent
 * cells in any of its rows and columns. Otherwise return 0 (false).
 * --------------------------------------------------------------
 */
static inline int _has_adjacent( const Board *board )
{
	int i,j;

	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}

	/* horizontally */
	for (i=0; i < board->dim; i++) {
		if ( _row_has_adjacent(board,i) ) {
			return 1;
		}
	}

	/* vertically */
	for (j=0; j < board->dim; j++) {
		if ( _col_has_adjacent(board,j) ) {
			return 1;
		}
	}

	return 0;
}

/* --------------------------------------------------------------
 * struct _tile *_grid_resize():
 *
 * --------------------------------------------------------------
 */
static inline struct _tile *_grid_resize( struct _tile *grid, int dim )
{
	struct _tile *test = realloc( grid, dim * dim * sizeof(*test) );
	if ( NULL == test ) {
		DBGF( "%s", "realloc failed (grid)!" );
		return NULL;
	}
	return test;
}

/* --------------------------------------------------------------
 * int _init():
 *
 * Initialize the specified board with default values.
 * Return 0 on error, 1 otherwise.
 * --------------------------------------------------------------
 */
static inline void _init( Board *board )
{
	//memset( board->grid, 0, BOARD_DIM_4 * BOARD_DIM_4 * sizeof(struct _tile) );
	board->dim         = BOARD_DIM_4;
	board->nempty      = BOARD_DIM_4 * BOARD_DIM_4;
	board->sentinel    = _VAL_SENTINEL_4;
	board->nrandom     = _NRANDOM_4;
	board->hasadjacent = 0;  /* false */
}

/* --------------------------------------------------------------
 * Board *make_board():
 *
 * --------------------------------------------------------------
 */
Board *make_board( int dim )
{
	Board *board = NULL;

	if ( !_VALID_DIM(dim) ) {
		DBGF( "Invalid board dimension (%d)", dim);
		return NULL;
	}

	board = calloc( 1, sizeof(*board) );
	if ( NULL == board ) {
		DBGF( "%s", "calloc failed (board)!" );
		return NULL;
	}

	board->grid = calloc( dim * dim, sizeof(struct _tile) );
	if ( NULL == board->grid ) {
		DBGF( "%s", "calloc failed (board->grid)!" );
		free( board );
		return NULL;
	}

	return board;
}

/* --------------------------------------------------------------
 * Board *new_board():
 *
 * The board constructor reserves memory for a new board object,
 * initializes it to default values and returns a pointer to it,
 * or NULL on error.
 *
 * NOTE: All new board objects are initialized with a 4x4 grid.
 * --------------------------------------------------------------
 */
Board *new_board( void )
{
	Board *board = make_board( BOARD_DIM_4 );
	if ( NULL == board ) {
		DBGF( "make_board(%d) failed!", BOARD_DIM_4 );
		return NULL;
	}

	_init( board );

	return board;
}

/* --------------------------------------------------------------
 * Board *board_free():
 *
 * The board destructor releases all resources occupied by the
 * specified board object, and it returns NULL (so the caller
 * may assign it back to the object pointer).
 * --------------------------------------------------------------
 */
Board *board_free( Board *board )
{
	if ( board ) {
		free( board->grid );
		free( board );
	}
	return NULL;
}

/* --------------------------------------------------------------
 * int board_reset():
 *
 * Reset the board for a new game. Return 0 on error, 1 otherwise.
 * NOTE: Resetting the board does NOT change the following fields:
 *       - board->dim
 *       - board->sentinel
 *       - board->nrandom
 * --------------------------------------------------------------
 */
int board_reset( Board *board )
{
	int len;      /* length of the grid (dim * dim) */

	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}
	if ( NULL == board->grid ) {
		DBGF( "%s", "Board grid is not created (NULL)!" );
		return 0;
	}

	len = board->dim * board->dim;
	if ( board->grid ) {
		memset( board->grid, 0, len * sizeof(struct _tile) );
	}
	board->nempty      = len;
	board->hasadjacent = 0;  /* false */

	return 1;
}

/* --------------------------------------------------------------
 * int board_resize():
 *
 * Resize the grid of the specified board according to the specified
 * single-dimension (dim). Return 0 on error, 1 otherwise.
 * --------------------------------------------------------------
 */
int board_resize_and_reset( Board *board, int dim )
{
	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}
	if ( !_VALID_DIM(dim) ) {
		DBGF( "Invalid dimension (%d)!", dim );
		return 0;
	}

	/* realloc board->grid ONLY if needed */
	if ( dim != board->dim )
	{
		struct _tile *test = realloc(
					board->grid,
					dim * dim * sizeof(*test)
					);
		if ( NULL == test ) {
			DBGF( "%s", "realloc failed (board->grid)!" );
			return 0;
		}
		board->grid = test;
	}

	/* reset the board */

	memset( board->grid, 0, dim * dim * sizeof( *(board->grid) ) );

	board->dim         = dim;
	board->nempty      = dim * dim;
	board->sentinel    = _dim_to_sentinel(dim);
	board->nrandom     = _dim_to_nrandom(dim);
	board->hasadjacent = 0;  /* false */

	return 1;
}

/* --------------------------------------------------------------
 * int board_copy():
 *
 * --------------------------------------------------------------
 */
int board_copy( Board *dst, const Board *src )
{
	if ( NULL == dst || NULL == src ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;  /* false */
	}

	if ( NULL == src->grid ) {
		free( dst->grid );
		dst->grid = NULL;
	}
	else {
		int len = src->dim * src->dim;
		if ( src->dim != dst->dim ) {
			dst->grid = _grid_resize( dst->grid, src->dim );
			if ( NULL == dst->grid ) {
				DBGF( "%s", "_grid_resize() failed!" );
				return 0;  /* false */
			}
		}
		memcpy( dst->grid, src->grid, len * sizeof(struct _tile) );
	}

	dst->dim         = src->dim;
	dst->sentinel    = src->sentinel;
	dst->nrandom     = src->nrandom;
	dst->nempty      = src->nempty;
	dst->hasadjacent = src->hasadjacent;

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int board_generate_ntiles():
 *
 * Generate (n) random values, each one being either 2 or 4,
 * and put them at (n) random empty tiles, in the specified
 * board. Return 0 (false) on error, 1 (true) otherwise.

 * NOTE: This function is a publicly exported wrapper of
 *       the function: _generate_tile(). It accepts one
 *       extra argument (n) and it performs sanity checks.
 * --------------------------------------------------------------
 */
int board_generate_ntiles( Board *board, int n )
{
	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}
	if ( !_VALID_DIM(board->dim) ) {
		DBGF( "Invalid grid dimension (%d)", board->dim );
		return 0;
	}
	if ( 0 == board->nempty ) {
		DBGF( "%s", "The board is full!" );
		return 0;
	}

	if ( n > board->nempty ) {
		n = board->nempty;
	}
	while ( n-- > 0 ) {
		_generate_tile( board );
	}

	return 1;
}

/* --------------------------------------------------------------
 * int board_has_room():
 *
 * Return 1 (true) if there's at least one empty slot in the
 * specified board. Otherwise (or on error) return 0 (false).
 * --------------------------------------------------------------
 */
int board_has_room( const Board *board )
{
	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}
	return (board->nempty) > 0;
}

/* --------------------------------------------------------------
 * int board_has_hadjacent():
 *
 * Return 1 (true) if the specified board has adjacent tiles with
 * equal, non-0 values in any of its rows and columns. Otherwise
 * 0return 0 (false).
 *
 * NOTE: This function is a publicly exported wrapper
 *       of the function: _has_adjacent()
 * --------------------------------------------------------------
 */
int board_has_adjacent( const Board *board )
{
	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}

	return _has_adjacent( board );
}

/* --------------------------------------------------------------
 * int board_move_up():
 *
 * Play a move towards the top of the board. On success, return 1
 * (true) so the caller can generate a new board element. Return 0
 * (false) if no move is available.
 *
 * The player score and his winning status are updated & returned to
 * the caller via the argument pointers score and won, respectively
 * (the latter is a boolean which is set to 1 if the score reaches
 * the winning SENTINEL value (this is 2048 with a 4x4 board).
 *
 * ALGORITHM:
 *
 * The algorithm is brutally naive, so feel free to improve it.
 *
 * For each column of the board...
 * a. First, all the non-0 elements are copied into a temp buffer,
 *    so the temp buffer has no gaps (zeros) between its elements.
 *    At the same time, the original non-0 elements of the row are
 *    cleared (zeroed).
 *
 * b. In the temp buffer, starting from left to right, if there are
 *    adjacent cells with equal non-0 values, they are merged into
 *    the first of the two cells, and the second cell is zeroed.
 *    Thus at the end, there may be gaps between the elements of
 *    the temp buffer.
 *
 * c. Starting from left to right, copy all non-0 elements of the
 *    temp buffer back into the board column (top to bottom).
 * --------------------------------------------------------------
 */
int board_move_up( Board *board, long int *score, int *won )
{
	int j;
	int *temp = NULL;
	const int DIM = board->dim;
	int moved = 0;                   /* return value */

	/* alloc temp buffer */
	temp = malloc( DIM * sizeof(*temp) );
	if ( NULL == temp ) {
		DBGF( "%s", "temp malloc failed!" );
		return 0;
	}

	/* for every column (j) */
	for ( j=0; j < DIM; j++)
	{
		if ( _col_has_gaps_begend(board, j) ) {
			moved = 1;
		}

		/* backup column (j) to temp buffer */
		_col_backup_nogaps_and_clear( board,j, temp );

		/* merge temp's adjacent cells (from left to right) */
		if ( _merge_adjacent_from_beg(temp, board, score, won) ) {
			moved = 1;
		}

		/* restore non-0, merged values from backup to column
		 * (left to right in backup, top to bottom in column)
		 */
		_col_restore_merged_nogaps_begend( board,j, temp );
	}

	free( temp );

	/* has the board adjacent equal cells even after merging? */
	board->hasadjacent = _has_adjacent( board );

	return moved;
}

/* --------------------------------------------------------------
 * int board_move_down():
 *
 * Play a move towards the bottom of the board. On success, return 1
 * (true) so the caller can generate a new board element. Return 0
 * (false) if no move was moved.
 *
 * ALGORITHM: Similar to board_move_up(), but traversing & merging
 *            is done from right to left in the temp buffer, and
 *            from bottom to top in the columns of the board.
 * --------------------------------------------------------------
 */
int board_move_down( Board *board, long int *score, int *won )
{
	int j;
	int *temp = NULL;
	const int DIM = board->dim;
	int moved = 0;

	/* alloc temp buffer */
	temp = malloc( DIM * sizeof(*temp) );
	if ( NULL == temp ) {
		DBGF( "%s", "temp malloc failed!" );
		return 0;
	}

	/* for every column (j) */
	for ( j=0; j < DIM; j++)
	{
		if ( _col_has_gaps_endbeg(board, j) ) {
			moved = 1;
		}

		/* backup column (j) to temp */
		_col_backup_nogaps_and_clear( board,j, temp );

		/* merge temp's adjacent (from right to left) */
		if ( _merge_adjacent_from_end(temp, board, score, won) ) {
			moved = 1;
		}

		/* restore non-0, merged values from backup to column
		 * (right to left in backup, bottom to top in column)
		 */
		_col_restore_merged_nogaps_endbeg( board,j, temp );
	}

	free( temp );

	/* has the board adjacent equal cells even after merging? */
	board->hasadjacent = _has_adjacent( board );

	return moved;
}

/* --------------------------------------------------------------
 * int board_move_left():
 *
 * Play a move towards the left edge of the board. On success,
 * return 1 (true) so the caller can generate a new board element.
 * Return 0 (false) if no move was moved.
 *
 * ALGORITHM: Similar to board_move_up(), but dealing with rows
 *            instead of columns. Traversing & merging is always
 *            done from left to right.
 * --------------------------------------------------------------
 */
int board_move_left( Board *board, long int *score, int *won  )
{
	int i;
	int *temp = NULL;
	const int DIM = board->dim;
	int moved = 0;

	/* alloc temp buffer */
	temp = malloc( DIM * sizeof(*temp) );
	if ( NULL == temp ) {
		DBGF( "%s", "temp malloc failed!" );
		return 0;
	}

	/* for every row (i) */
	for (i=0; i < DIM; i++)
	{
		if ( _row_has_gaps_begend(board,i) ) {
			moved = 1;
		}

		/* backup row (i) to temp buffer */
		_row_backup_nogaps_and_clear( board,i, temp );

		/* merge temp's adjacent cells (from left to right) */
		if ( _merge_adjacent_from_beg(temp, board, score, won) ) {
			moved = 1;
		}

		/* restore non-0, merged values from backup (left to right)*/
		_row_restore_merged_nogaps_begend( board,i, temp );
	}

	free( temp );

	/* has the board adjacent equal cells even after merging? */
	board->hasadjacent = _has_adjacent( board );

	return moved;
}

/* --------------------------------------------------------------
 * int board_move_right():
 *
 * Play a move towards the right edge of the board. On success,
 * return 1 (true) so the caller can generate a new board element.
 * Return 0 (false) if no move was moved.
 *
 * ALGORITHM: Similar to board_move_left(), but traversing
 *            and merging is always done from right to left.
 * --------------------------------------------------------------
 */
int board_move_right( Board *board, long int *score, int *won )
{
	int i;
	int *temp = NULL;
	const int DIM = board->dim;
	int moved = 0;

	/* alloc temp buffer */
	temp = malloc( DIM * sizeof(*temp) );
	if ( NULL == temp ) {
		DBGF( "%s", "temp malloc failed!" );
		return 0;
	}

	/* for every row */
	for ( i=0; i < DIM; i++)
	{
		if ( _row_has_gaps_endbeg(board,i) ) {
			moved = 1;
		}

		/* backup row (i) to temp buffer */
		_row_backup_nogaps_and_clear( board,i, temp );

		/* merge temp's adjacent cells (from right to left) */
		if ( _merge_adjacent_from_end(temp, board, score, won) ) {
			moved = 1;
		}

		/* restore non-0, merged values from backup (right to left)*/
		_row_restore_merged_nogaps_endbeg( board,i, temp );
	}

	free( temp );

	/* has the board adjacent equal cells even after merging? */
	board->hasadjacent = _has_adjacent( board );

	return moved;
}

/* --------------------------------------------------------------
 * int board_get_dim():
 * Getter
 * --------------------------------------------------------------
 */
int board_get_dim( const Board *board )
{
	return board->dim;
}

/* --------------------------------------------------------------
 * int board_get_dim():
 * Getter
 * --------------------------------------------------------------
 */
int board_get_sentinel( const Board *board )
{
	return board->sentinel;
}

/* --------------------------------------------------------------
 * int board_get_nrandom():
 * Getter
 * --------------------------------------------------------------
 */
int board_get_nrandom( const Board *board )
{
	return board->nrandom;
}

/* --------------------------------------------------------------
 * int board_get_nempty():
 * Getter
 * --------------------------------------------------------------
 */
int board_get_nempty( const Board *board )
{
	return board->nempty;
}

/* --------------------------------------------------------------
 * int board_get_tile_value():
 * Getter
 * --------------------------------------------------------------
 */
int board_get_tile_value( const Board *board, int i, int j )
{
	return board->grid[ _IDX(i,j,board->dim) ].val;
}


/* --------------------------------------------------------------
 * char *_grid_to_text():
 *
 * --------------------------------------------------------------
 */
static inline char *_grid_to_text( const struct _tile *grid, int dim )
{
	int i, len;
	char *txtout = NULL, *test = NULL;

	/* a NULL grid is printed as "NULL\n" */
	if ( NULL == grid ) {
		txtout = printf_to_text( "%s\r\n", "NULL" );
		if ( NULL == txtout ) {
			DBGF( "%s", "printf_to_text() failed!" );
		}
		return txtout;
	}

	len = dim * dim;
	for (i=0; i < len; i++)
	{
		test = printf_to_text(
			"%s%d ",
			NULL == txtout ? "\0" : txtout,
			grid[i].val
			);
		if ( NULL == test ) {
			DBGF( "%s", "printf_to_text() failed!" );
			goto ret_failure;
		}
		txtout = test;
	}

	test = printf_to_text( "%s\r\n", NULL == txtout ? "\0" : txtout );
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;

	return txtout;

ret_failure:
	free( txtout );
	return NULL;
}

/* --------------------------------------------------------------
 * char *board_to_text():
 *
 * --------------------------------------------------------------
 */
char *board_to_text( const Board *board )
{
	char *txtout = NULL, *test = NULL, *tmp = NULL;

	/* a NULL board is printed as "NULL#\n" */
	if ( NULL == board ) {
		txtout = printf_to_text( "%s\n", "NULL#" );
		if ( NULL == txtout ) {
			DBGF( "%s", "printf_to_text() failed!" );
		}
		return txtout;
	}

	/* board->dim
	 * + board->sentinel
	 * + board->nrandom
	 * + board->nempty
	 * + board->hasadjacent
	 */
	test = printf_to_text(
		"%s%d %d %d %d %d#",
		NULL == txtout ? "\0" : txtout,
		board->dim,
		board->sentinel,
		board->nrandom,
		board->nempty,
		board_has_adjacent(board)
		);
	if ( NULL == test ) {
		DBGF( "%s", "printf_to_text() failed!" );
		goto ret_failure;
	}
	txtout = test;

	/* + board->grid */
	tmp = _grid_to_text( board->grid, board->dim );
	if ( NULL == tmp ) {
		DBGF( "%s", "_grid_to_text() failed!" );
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
 * Board *new_board_from_text():
 *
 * NOTES: The expected format of the text is the following:
 *        "dim sentinel nrandom nempty hasadjacent#tile-values"
 * --------------------------------------------------------------
 */
Board *new_board_from_text( char *text )
{
	Board *board = NULL;
	char  *tokens[2] = { NULL };
	int   ntokens = 0;
	int   n=0, len=0;

	if ( NULL == text ) {
		DBGF( "%s", "NULL pointer argument!" );
		return NULL;
	}

	ntokens = s_tokenize( text, tokens, 2, "#" );
	if ( ntokens < 2 ) {
		DBGF( "%s", "s_tokenize(text) failed to produce 2 tokens" );
		return NULL;
	}

	board = new_board();
	if ( NULL == board ) {
		DBGF( "%s", "new_board() failed!" );
		return NULL;
	}

	/* from the 1st token, get the meta-data */
	n = sscanf(
		tokens[0],
		"%d %d %d %d %d",
		&board->dim,
		&board->sentinel,
		&board->nrandom,
		&board->nempty,
		&board->hasadjacent
		);
	if ( n < 5 ) {
		DBGF( "%s", "sscanf(tokens[0]) failed!" );
		goto ret_failure;
	}

	/* resize the grid according to board->dim */
	if ( !_VALID_DIM(board->dim) ) {
		DBGF( "Invalid grid-dimension (%d)!", board->dim );
		goto ret_failure;
	}
	board->grid = _grid_resize( board->grid, board->dim );
	if ( NULL == board->grid ) {
		DBGF( "%s", "_grid_resize() failed!" );
		goto ret_failure;
	}

	/* from the 2nd token, get the tile-values into the grid */
	len = board->dim * board->dim;
	char *tokval = strtok(tokens[1], " ");
	for (n=0; n < len && tokval; n++)
	{
		int m = sscanf( tokval, "%d", &board->grid[n].val );
		if ( m < 1 ) {
			DBGF( "%s", "sscanf(tokval) failed!" );
			goto ret_failure;
		}
		tokval = strtok(NULL, " ");
	}

	return board;

ret_failure:
	board_free( board );
	return NULL;
}

/* --------------------------------------------------------------
 * For DEBUGGING Purposes: ***
 *
 * Generate a random value between 2 and 4 and put it at the
 * specified slot of the board. Return 0 on error, 1 otherwise.
 * --------------------------------------------------------------
 */
int dbg_board_generate_tile( Board *board, int i, int j )
{
	int dim;

	if ( NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}
	if ( board->nempty < 1 ) {
		DBGF( "%s", "The board is full!" );
		return 0;
	}

	/* val must be 2 or 4 */
	dim = board->dim;
	board->grid[_IDX(i,j,dim)].val = (rand() % 2 == 0) ? 2 : 4;

	board->nempty--;

	return 1;
}

/* --------------------------------------------------------------
 * For DEBUGGING Purposes: ***
 *
 * Dump the contents of the specified board on stdout.
 * --------------------------------------------------------------
 */
void dbg_board_dump( Board *board )
{
	putchar( '\n' );
	if ( !board ) {
		puts( "Board is empty" );
		return;
	}

	printf( "\t&board: 0x%p\n", (void *)board );

	printf( "\t\tdim: %d\n", board->dim );
	printf( "\t\tsentinel: %d\n", board->sentinel );
	printf( "\t\tnrandom: %d\n", board->nrandom );
	printf( "\t\tnempty: %d\n", board->nempty );
	printf( "\t\thasadjacent: %d\n", board->hasadjacent );

	puts( "\t\tboard->grid" );
	int dim = board->dim;
	for (int i=0; i < dim; i++) {
		printf( "\t\t\t" );
		for (int j=0; j < dim; j++) {
			printf(
				"%4d ",
				board->grid[_IDX(i,j,dim)].val
				);
			fflush( stdout );
		}
		putchar( '\n' );
	}
}
