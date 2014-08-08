/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: misc.h, gamedata.h
 * --------------------------------------------------------------
 *
 * Private implementation of the Gamedata "class". The accompanying
 * header file "gamedata.h" exposes publicly the "class" as an opaque
 * data-type.
 *
 * The existence of the Gamedata class is an attempt to conceptually
 * separate the GUI from the core-data of the main game. The main()
 * function is responsible for initially creating a game-data object,
 * and then pass it to the constructor of a gui-object.
 *
 * After that, the gui-object manipulates the game-data according to
 * the user input.
 *
 * For this stand-alone GTK+2 replayer this is a bit of a stretch,
 * because the actual game-data do NOT pre-exist. They are read
 * from replay-files, from within the gui-object.
 *
 * Nevertheless, it does help to at least separate core-data from
 * the GUI graphical elements (widgets) and their inner structures.
 * 
 ****************************************************************
 */

#define GAMEDATA_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>            /* memset(), memcpy(), strcmp(), strncpy() */
#include <stdbool.h>

#include "misc.h"
#include "gamedata.h"

#define _SZMAX_FLINE  (1023+1) /* max bytes to handle as file-line */

/* A _GamedataMove corresponds to a single line, being part of a sequence of
 * list-lines that are read from a replay-file (not all lines in a replay-file
 * are list-lines... for details see: gamedata_set_from_fname() ).
 *
 * A valid list-line of a non-NULL list has the following format:
 * 
8:0 0 0 0 4@5 65536 1 23 0#0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 4 0 0 0 0 0 0 0 0\r\n
aa:score bscore iswin prev next@dim sentinel nrandom nempty hasadjacent#tiles
*/
typedef struct {
	long int score;        /* current score */
	long int bscore;       /* best-score across multiple games */
	int      iswin;        /* has the sentinel value reached? */
	int      prevmv;       /* enum direction of previous move */
	int      nextmv;       /* enum direction of next move */
	int      dim;          /* square board's single-dimension */
	long int sentinel;     /* sentinel-tile value */
	int      nrandom;      /* number of spawned tiles */
	long int *tiles;       /* dynamic array of tile-values */
} _GamedataMove;

/* The Gamedata "class" consists of a dynamic array of moves,
 * along with some meta-data (it is exposed publicly as an
 * opaque data-type, via: gamedata.h).
 */
struct _Gamedata
{
	/* meta-data */
	char fname[SZMAX_DBGMSG];  /* name of loaded replay-file */
	int            didundo;    /* has the player un-done at least 1 move?*/
	long int       nmoves;     /* total moves */
	unsigned long  delay;      /* animation delay (secs) */
	int            gamewon;    /* was this game won? */

	/* dynamic array of the actual moves */
	_GamedataMove  *moves;
};

/* ---------------------------------------------------
 * (Just for debugging)
 * void dbg_gamedata_print_tiles_of_move():
 *
 * Given a game-data object (gd), print all tile-values
 * stored inside its specified move (imove).
 * ---------------------------------------------------
 */
void dbg_gamedata_print_tiles_of_move( Gamedata *gd, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return;
	}
	if ( NULL == gd->moves[imove].tiles ) {
		DBG_STDERR_MSG( "gd->moves[imove].tiles is NULL!");
		return;
	}
	if ( imove < 0 || imove > gd->nmoves-1 ) {
		DBG_STDERR_MSG( "Invalid imove!" );
		return;
	}

	int len = gd->moves[imove].dim * gd->moves[imove].dim;
	for (int i=0; i < len; i++) {
		printf( "%ld ", gd->moves[imove].tiles[i] );
		fflush( stdout );
	}
	putchar( '\n' );
}

/* ---------------------------------------------------
 * (Just for debugging)
 * void dbg_gamedata_print_tiles():
 *
 * Print all tile-values stored in the specified game-data object (gd).
 * ---------------------------------------------------
 */
void dbg_gamedata_print_tiles( Gamedata *gd )
{
	long int m = 0;   /* moves counter */

	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return;
	}

	for (m=0; m < gd->nmoves; m++) {
		int len = gd->moves[m].dim * gd->moves[m].dim;
		for (int i=0; i < len; i++) {
			printf( "%ld ", gd->moves[m].tiles[i] );
			fflush( stdout );
		}
		putchar( '\n' );
	}
}

/* ---------------------------------------------------
 * (Private Destructor) _GamedataMove *_moves_free():
 *
 * Release all resources occupied by the first (nmoves) elements
 * of the specified dynamic array of moves (moves).
 * Return NULL, so the caller may assign it back to the pointer
 * of the dynamic array.
 *
 * NOTE: No sanity checks are performed on arguments.
 * ---------------------------------------------------
 */
_GamedataMove *_moves_free( _GamedataMove *moves, int nmoves )
{
	for (int i=0; i < nmoves; i++) {
		free( moves[i].tiles );
	}
	free( moves );

	return NULL;
}

/* ---------------------------------------------------
 * (Private Constructor) _GamedataMove *_new_moves_from_fp():
 *
 * Starting at a valid file-pointer (fp), opened in binary-mode,
 * create and return a pointer to a dynamic array consisting of
 * the specified number of _GamedataMove elements (nmoves), by
 * reading so many subsequent lines in the open file.
 * Return NULL on error.
 *
 * Each of the (nmoves) file-lines starting at (fp) are expected
 * to be of the following format:
 *
8:0 0 0 0 4@5 65536 1 23 0#0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 4 0 0 0 0 0 0 0 0\r\n
aa:score bscore iswin prev next@dim sentinel nrandom nempty hasadjacent#tile-values
 *
 * NOTE: No sanity checks are performed on arguments.
 * ---------------------------------------------------
 */
static inline _GamedataMove *_new_moves_from_fp(
	long int     nmoves,
	FILE         *fp
	)
{
	_GamedataMove *moves = NULL;        /* to be returned */
	long int i;
	char line[_SZMAX_FLINE] = {'\0'};
	char *cp = NULL;

	moves = calloc( nmoves, sizeof( *moves ) );
	if ( NULL == moves ) {
		DBG_STDERR_MSG( "calloc(moves) failed!" );
		return NULL;
	}

	for (i=0; i < nmoves; i++)
	{
		/* get next line in file */
		memset( line, 0, _SZMAX_FLINE );
		if ( NULL == s_fgets(line, _SZMAX_FLINE, fp) ) {
			DBG_STDERR_MSG(
				"s_fgets() failed to read 1st line of replay-list"
				);
			goto ret_failure;
		}

		/* skip until ':' (or end) */
		cp = line;
		while ( ':' != *cp && *cp ) { cp++; }
		if ( '\0' == *cp ) {
			DBG_STDERR_MSG( "':' was not found in line!" );
			goto ret_failure;
		}
		/* read: score, bscore, iswin, prev, next */
		if ( sscanf(
			++cp,
			"%ld %ld %d %d %d",
			&moves[i].score,
			&moves[i].bscore,
			&moves[i].iswin,
			&moves[i].prevmv,
			&moves[i].nextmv
			) < 5
		){
			DBG_STDERR_MSG(
				"sscanf(score, ..., nextmv) after ':' failed!"
				);
			goto ret_failure;
		}

		/* skip until '@' (or end) */
		while ( '@' != *cp && *cp ) { cp++; }
		if ( '\0' == *cp ) {
			DBG_STDERR_MSG( "'@' was not found in line!" );
			goto ret_failure;
		}
		/* read: dim sentinel nrandom */
		if ( sscanf(
			++cp,
			"%d %ld %d",
			&moves[i].dim,
			&moves[i].sentinel,
			&moves[i].nrandom
			) < 3
		){
			DBG_STDERR_MSG(
				"sscanf(dim, sentinel, nrandom) failed!"
				);
			goto ret_failure;
		}

		/* skip until '#' (or end) */
		while ( '#' != *cp && *cp ) { cp++; }
		if ( '\0' == *cp ) {
			DBG_STDERR_MSG( "'#' was not found in line!" );
			goto ret_failure;
		}
		/* read: tile-values */
		const int len = moves[i].dim * moves[i].dim;
		long int *buf = realloc( moves[i].tiles, len * sizeof( *buf ) );
		if ( NULL == buf ) {
			DBG_STDERR_MSG(
				"ralloc(moves[i].tiles) failed!"
				);
			_moves_free( moves, i);
			return NULL;
		}
		moves[i].tiles = buf;
		char *tokval = strtok( ++cp, " " );
		for (int n=0; n < len && tokval; n++) {
			if ( sscanf(tokval, "%ld", &moves[i].tiles[n]) < 1 ){
				DBG_STDERR_MSG(
					"sscanf(tiles[n]) failed!"
					);
				_moves_free( moves, i);
				return NULL;
			}
			tokval = strtok( NULL, " " );
		}
	}

	return moves;

ret_failure:
	free( moves );
	return NULL;
}

/* ---------------------------------------------------
 * bool _copy():
 *
 * Deep copy source game-data (src) to destination game-data (dst).
 * Return false on error, true otherwise.
 *
 * NOTE (IMPORTANT!):
 *    On failure, dst may have been modified. If the original
 *    is important, we must pass a copy of it into this function.
 * ---------------------------------------------------
 */
static inline bool _copy( Gamedata *dst, const Gamedata *src )
{
	/* sanity checks */
	if ( NULL == dst || NULL == src ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return false;
	}

	/*
	 * when non-NULL src->moves, then dst->moves gets freed,
	 * gets freshly allocated & gets a deep copy of src->moves.
	 */
	if ( src->moves ) {

		/* free and reallocate dst->moves */
		_moves_free( dst->moves, dst->nmoves );
		dst->moves = calloc( src->nmoves, sizeof( *(dst->moves) ) );
		if ( NULL == dst->moves ) {
			DBG_STDERR_MSG( "calloc(dst->moves) failed!" );
			return false;
		}

		/* deep copy src->moves to dst->moves */
		for (int i=0; i < src->nmoves; i++)
		{
			if ( NULL == src->moves[i].tiles ) {
				dst->moves[i].tiles = NULL;
			}
			else {
				free( dst->moves[i].tiles );

				int len= src->moves[i].dim * src->moves[i].dim;
				dst->moves[i].tiles
					= calloc(
						len,
						sizeof(*(dst->moves[i].tiles))
					);
				if ( NULL == dst->moves[i].tiles ) {
					DBG_STDERR_MSG(
						"calloc(dst->moves[i].tiles) "
						"failed!"
						);
					_moves_free( dst->moves, i );
					return false;
				}
				memcpy(
					dst->moves[i].tiles,
					src->moves[i].tiles,
					len * sizeof( *(dst->moves[i].tiles) )
					);
			}

			dst->moves[i].score    = src->moves[i].score;
			dst->moves[i].bscore   = src->moves[i].bscore;
			dst->moves[i].iswin    = src->moves[i].iswin;
			dst->moves[i].prevmv   = src->moves[i].prevmv;
			dst->moves[i].nextmv   = src->moves[i].nextmv;
			dst->moves[i].dim      = src->moves[i].dim;
			dst->moves[i].sentinel = src->moves[i].sentinel;
			dst->moves[i].nrandom  = src->moves[i].nrandom;
		}
	}

	strncpy( dst->fname, src->fname, SZMAX_DBGMSG-1 );
	dst->didundo = src->didundo;
	dst->nmoves  = src->nmoves;
	dst->delay   = src->delay;
	dst->gamewon = src->gamewon;

	return true;
}

/* ---------------------------------------------------
 * (Destructor) Gamedata *gamedata_free():
 *
 * Release all resources occupied by the specified
 * game-data object pointer (gd), and return NULL
 * (so the caller may assign it back to the object
 * pointer).
 * ---------------------------------------------------
 */
Gamedata *gamedata_free( Gamedata *gd )
{
	if ( gd ) {
		_moves_free( gd->moves, gd->nmoves );
		free( gd );
	}
	return NULL;
}

/* ---------------------------------------------------
 * (Constructor) Gamedata *make_gamedata():
 *
 * Reserve zero'ed memory for a game-data object,
 * and return a pointer to it, or NULL on error.
 *
 * NOTE: On success, the caller should initialize
 *       the object with sensible values.
 * ---------------------------------------------------
 */
Gamedata *make_gamedata( void )
{
	Gamedata *gd = calloc( 1, sizeof(*gd) );
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "*** error: calloc() failed!");
		return NULL;
	}
	return gd;
}

/* ---------------------------------------------------
 * bool _fp_skip_list():
 *
 * Given a valid pointer to a file opened in binary-mode
 * (fp), and given that this file pointer already points
 * to the start of the 1st line in a list of move-lines,
 * skip all subsequent lines until the list gets exhausted.
 * Return false on error, true otherwise.
 *
 * NOTES:
 *     Replay-files contain, among other things, lists of
 *     moves (lines) for the Undo, the Redo and the Replay
 *     lists. If any of those lists is empty, then it is
 *     represented inside the file with just one line:
 *     "NULL:\r\n"
 *
 *     This function is used by the function: gamedata_set_from_fname()
 *     for skipping the Undo and the Redo lists (one at a time),
 *     since they are useless to the replayer.
 *
 *     It knows how many lines to skip, because the total
 *     count of lines in a list is recorded at the start
 *     of the 1st line of the list (this count decreases
 *     sequentially in subsequent lines).
 *
 *     As a final note, the function uses s_fgets() to skip
 *     a line, which contrary to fgets() it drops the EOL
 *     sequence. So when checking for empty lists, we simply
 *     compare against "NULL:" (instead of "NULL:\r\n").
 * ---------------------------------------------------
 */
static inline bool _fp_skip_list( FILE *fp )
{
	char line[_SZMAX_FLINE] = {'\0'};

	if ( NULL == s_fgets(line, _SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"s_fgets() failed to read the 1st line of the list!"
			);
		return false;
	}

	if ( 0 != strcmp(line, "NULL:") ) {   /* non-NULL list */
		long int count = 0;
		if ( sscanf(line, "%ld", &count) < 1 ) {
			DBG_STDERR_MSG(
				"sscanf() failed to read list's # of nodes"
				);
			return false;
		}
		/* skip remaining lines */
		for (long int i=1; i < count; i++) {
			if ( NULL == s_fgets(line, _SZMAX_FLINE, fp) ) {
				DBG_STDERR_MSG(
					"s_fgets failed while skipping list!"
					);
				return false;
			}
		}
	}

	return true;
}

/* ---------------------------------------------------
 * (Initializer) bool gamedata_set_from_fname():
 *
 * Given a pointer to an already allocated game-data object (gd),
 * and the filename of an existing replay-file (fname), initialize
 * the game-data object according to the values stored inside the
 * replay-file. Return false on error, true otherwise.
 *
 * NOTES: This function is actually the replay-file parser.
 *        It uses the functions: _new_moves_from_fp() and
 *        _fp_skip_list(), so you may want to have a look
 *        at them too.
 * ---------------------------------------------------
 */
bool gamedata_set_from_fname( Gamedata *gd, const char *fname )
{
	Gamedata test;
	FILE     *fp = NULL;
	char     line[_SZMAX_FLINE] = {'\0'};

	/* sanity checks */
	if ( NULL == gd || NULL == fname ) {
		DBG_STDERR_MSG(
			"*** error: NULL pointer argument (gd OR fname)"
			);
		return false;
	}

	/*
	 * IMPORTANT!
	 * Open fname for reading in BINARY mode,
	 * so that s_fgets() will work as expected.
	 */
	fp = fopen( fname, "rb" );
	if ( NULL == fp ) {
		DBG_STDERR_MSG( "*** error: couldn't open requested file!" );
		return false;
	}

	/*
	 * IMPORTANT!
	 */
	memset( &test, 0, sizeof(test) );
	strncpy( test.fname, fname, SZMAX_DBGMSG );

	/*
	 * read the didundo value
	 */
	if ( NULL == s_fgets(line, _SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG( "s_fgets() failed to read the didundo line!" );
		goto ret_failure;
	}
	if ( sscanf(line, "%d", &test.didundo) < 1 ) {
		DBG_STDERR_MSG( "sscanf(&didundo) failed!" );
		goto ret_failure;
	}

	/* skip the undo-list */
	if ( !_fp_skip_list(fp) ) {
		DBG_STDERR_MSG(
			"_fp_skip_list() failed for the undo-list!"
			);
		goto ret_failure;
	}

	/*  skip the redo-list */
	if ( !_fp_skip_list(fp) ) {
		DBG_STDERR_MSG(
			"_fp_skip_list() failed for the redo-list!"
			);
		goto ret_failure;
	}

	/* read the replay meta-data */

	if ( NULL == s_fgets(line, _SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"s_fgets() failed to read replay's meta-data line"
			);
		goto ret_failure;
	}
	if ( sscanf(
		line,
		"%lu %ld",
		&test.delay,
		&test.nmoves
		) < 2
	){
		DBG_STDERR_MSG( "sscanf() failed to read replay meta-data!" );
		goto ret_failure;
	}

	/* read the actual moves of the replay-list */
	test.moves = _new_moves_from_fp( test.nmoves, fp );
	if ( NULL == test.moves ) {
		DBG_STDERR_MSG(
			"_new_moves_from_fp(test.nmoves) failed!"
			);
		goto ret_failure;
	}

	/* everything ok, update gd */
	_copy( gd, &test );
	if ( gd->nmoves > 0 && gd->moves ) {
		gd->gamewon = gd->moves[ gd->nmoves-1 ].iswin;
	}
//dbg_gamedata_print_tiles( gd );
	fclose( fp );
	return true;

ret_failure:
	_moves_free( test.moves, test.nmoves );
	fclose( fp );
	return false;
}

/* ---------------------------------------------------
 * (Getter) char *gamedata_get_fname():
 *
 * Return a pointer to the current fname c-string field of
 * the specified game-data object (gd), or NULL on error.
 *
 * NOTE: The fname c-string reflects the filename
 *       of the currently loaded replay-file.
 * ---------------------------------------------------
 */
char *gamedata_get_fname( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return NULL;
	}
	return gd->fname;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_didundo():
 *
 * Return the current boolean value of the didundo field,
 * of the specified game-data object (gd), or 0 (false)
 * on error.
 * 
 * NOTE: The didundo field dictates whether the player
 *       has un-done at least one move in the recorded
 *       game.
 * ---------------------------------------------------
 */
int gamedata_get_didundo( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;  /* false */
	}
	return gd->didundo;
}

/* ---------------------------------------------------
 * (Getter) long int gamedata_get_nmoves():
 *
 * Return the current value of the nmoves field,
 * of the specified game-data object (gd), or 0
 * on error.
 * 
 * NOTE: The nmoves field holds the total count
 *       of moves available in the recorded game.
 * ---------------------------------------------------
 */
long int gamedata_get_nmoves( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}
	return gd->nmoves;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_dim():
 *
 * Return the current value of the dim field, of the
 * specified game-data object (gd), or 0 on error.
 * 
 * NOTES:
 *     The dim field holds the single-dimension
 *     of the square board, in the recorded game.
 *
 *     This info is available in every single move,
 *     so we simply return it from the first one.
 * ---------------------------------------------------
 */
int gamedata_get_dim( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}

	return gd->moves[0].dim;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_sentinel():
 *
 * Return the current value of the sentinel field, of
 * the specified game-data object (gd), or 0 on error.
 * 
 * NOTES:
 *     The sentinel field holds the value of
 *     the winning tile, in the recorded game.
 *
 *     This info is available in every single move,
 *     so we simply return it from the first one.
 * ---------------------------------------------------
 */
int gamedata_get_sentinel( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}

	return gd->moves[0].sentinel;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_nrandom():
 *
 * Return the current value of the nrandom field, of
 * the specified game-data object (gd), or 0 on error.
 * 
 * NOTES:
 *     The nrandom field holds the count of randomly
 *     spawned tiles after every successful move, in
 *     the recorded game.
 *
 *     This info is available in every single move,
 *     so we simply return it from the first one.
 * ---------------------------------------------------
 */
int gamedata_get_nrandom( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}

	return gd->moves[0].nrandom;
}

/* ---------------------------------------------------
 * (Getter) unsigned long int gamedata_get_delay():
 *
 * Return the current value of the delay field, of
 * the specified game-data object (gd), or 0 on error.
 * 
 * NOTE: The delay field holds the replay playback
 *       delay in milliseconds, for the recorded game.
 * ---------------------------------------------------
 */
unsigned long int gamedata_get_delay( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}
	return gd->delay;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_gamewon():
 *
 * Return the current boolean value of the gamewon field,
 * of the specified game-data object (gd), or 0 (false)
 * on error.
 * 
 * NOTE: The gamewon field dictates whether the
 *       recorded game has been won by the player.
 * ---------------------------------------------------
 */
int gamedata_get_gamewon( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}
	return gd->gamewon;
}

/* ---------------------------------------------------
 * (Getter) long int gamedata_get_tile_of_move():
 *
 * Given a pointer to a game-data object (gd), the index
 * of a recorded move (imove) inside the object, and the
 * index of a tile (itile) in the board of the recorded
 * move, return the value of the tile, or 0 on error.
 * ---------------------------------------------------
 */
long int gamedata_get_tile_of_move( Gamedata *gd, int itile, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}
	if ( imove < 0 || imove > gd->nmoves - 1 ) {
		DBG_STDERR_MSG( "Invalid imove!");
		return 0;
	}
	if ( NULL == gd->moves[imove].tiles ) {
		DBG_STDERR_MSG( "gd->moves[imove].tiles is NULL!");
		return 0;
	}
	const int len = gd->moves[0].dim * gd->moves[0].dim;
	if ( itile < 0 || itile > len-1 ) {
		DBG_STDERR_MSG( "Invalid itile!");
		return 0;
	}

	return gd->moves[imove].tiles[itile];
}

/* ---------------------------------------------------
 * (Getter) long int gamedata_get_score_of_move():
 *
 * Given a pointer to a game-data object (gd), and the
 * index of a recorded move (imove) inside the object,
 * return the score value of the recorded move, or 0
 * on error.
 * ---------------------------------------------------
 */
long int gamedata_get_score_of_move( Gamedata *gd, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}

	if ( imove < -1 || imove > gd->nmoves - 1 ) {
		DBG_STDERR_MSG( "invalid imove!");
		return 0;
	}
	else if ( imove == -1 ) {
		return 0;
	}

	return gd->moves[imove].score;
}

/* ---------------------------------------------------
 * (Getter) long int gamedata_get_bestscore_of_move():
 *
 * Given a pointer to a game-data object (gd), and the
 * index of a recorded move (imove) inside the object,
 * return the best-score value of the recorded move,
 * or 0 on error.
 * ---------------------------------------------------
 */
long int gamedata_get_bestscore_of_move( Gamedata *gd, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}

	if ( imove < -1 || imove > gd->nmoves - 1 ) {
		DBG_STDERR_MSG( "invalid imove!");
		return 0;
	}
	else if ( imove == -1 ) {
		return 0;
	}

	return gd->moves[imove].bscore;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_prevmv_of_move():
 *
 * Given a pointer to a game-data object (gd), and the
 * index of a recorded move (imove) inside the object,
 * return the enumerated direction value of the prevmv
 * field of the recorded move, or GAMEDATA_MVDIR_NONE
 * on error.
 *
 * NOTE: The prevmv field describes the direction at
 *       which the board was played in the previous
 *       move.
 * ---------------------------------------------------
 */
int gamedata_get_prevmv_of_move( Gamedata *gd, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return GAMEDATA_MVDIR_NONE;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return GAMEDATA_MVDIR_NONE;
	}

	if ( imove < -1 || imove > gd->nmoves - 1 ) {
		DBG_STDERR_MSG( "invalid imove!");
		return GAMEDATA_MVDIR_NONE;
	}
	else if ( imove == -1 ) {
		return GAMEDATA_MVDIR_NONE;
	}

	return gd->moves[imove].prevmv;
}

/* ---------------------------------------------------
 * (Getter) int gamedata_get_prevmv_of_move():
 *
 * Given a pointer to a game-data object (gd), and the
 * index of a recorded move (imove) inside the object,
 * return the enumerated direction value of the nextmv
 * field of the recorded move, or GAMEDATA_MVDIR_NONE
 * on error.
 *
 * NOTE: The nextmv field describes the direction at
 *       which the board was played for yielding the
 *       next move.
 * ---------------------------------------------------
 */
int gamedata_get_nextmv_of_move( Gamedata *gd, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return GAMEDATA_MVDIR_NONE;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return GAMEDATA_MVDIR_NONE;
	}

	if ( imove < -1 || imove > gd->nmoves - 1 ) {
		DBG_STDERR_MSG( "invalid imove!");
		return GAMEDATA_MVDIR_NONE;
	}
	else if ( imove == -1 ) {
		return GAMEDATA_MVDIR_NONE;
	}

	return gd->moves[imove].nextmv;
}
