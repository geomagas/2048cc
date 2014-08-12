#define GAMEDATA_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>       /* memset(), memcpy(), strcmp(), strncpy() */
#include <stdbool.h>

#include "misc.h"
#include "gamedata.h"

#define SZMAX_FLINE  1024

/*
8:0 0 0 0 4@5 65536 1 23 0#0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 4 0 0 0 0 0 0 0 0\r\n
aa:score bscore iswin prev next@dim sentinel nrandom nempty hasadjacent#tiles
*/
struct _GamedataMove {
	long int score;        /* current score */
	long int bscore;       /* best-score across multiple games */
	int      iswin;        /* has the sentinel value reached? */
	int      prevmv;       /* direction of previous move */
	int      nextmv;       /* direction of next move */
	int      dim;          /* square board's single-dimension */
	int      sentinel;     /* sentinel-tile value */
	int      nrandom;      /* number of spawned tiles */
	int      *tiles;       /* dynamic array of tile-values */
};

struct _Gamedata {
	char fname[SZMAX_DBGMSG]; /* name of loaded replay-file */
	int           didundo;
	long int      nmoves;      /* total moves */
	unsigned long delay;       /* animation delay (secs) */
	int           gamewon;     /* was this game won? */
	GamedataMove  *moves;
};

void gamedata_print_tiles( Gamedata *gd );

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GamedataMove *_moves_free( GamedataMove *moves, int nmoves )
{
	for (int i=0; i < nmoves; i++) {
		free( moves[i].tiles );
	}
	free( moves );

	return NULL;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
/*
8:0 0 0 0 4@5 65536 1 23 0#0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 4 0 0 0 0 0 0 0 0\r\n
aa:score bscore iswin prev next@dim sentinel nrandom nempty hasadjacent#tile-values
*/
static inline GamedataMove *_new_moves_from_fp(
	long int     nmoves,
	FILE         *fp
	)
{
	GamedataMove *moves = NULL;        /* to be returned */
	long int i;
	char line[SZMAX_FLINE] = {'\0'};
	char *cp = NULL;

	moves = calloc( nmoves, sizeof( *moves ) );
	if ( NULL == moves ) {
		DBG_STDERR_MSG(
			"calloc(moves) failed!"
			);
		return NULL;
	}

	for (i=0; i < nmoves; i++)
	{
		/* get next line in file */
		memset( line, 0, SZMAX_FLINE );
		if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
			DBG_STDERR_MSG(
				"fgets() failed to read 1st line of replay-list"
				);
			goto ret_failure;
		}
		s_fixeol( line );

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
			"%d %d %d",
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
		int *buf = realloc( moves[i].tiles, len * sizeof( *buf ) );
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
			if ( sscanf(tokval, "%d", &moves[i].tiles[n]) < 1 ){
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
 * Deep copy source game-data (src) to destination
 * game-data (dst). Return false on error, true otherwise.
 *
 * NOTE (IMPORTANT!):
 *    On failure, dst may have been modified. If the original
 *    is important, we must pass a copy of it to this function.
 * ---------------------------------------------------
 */
bool _copy( Gamedata *dst, const Gamedata *src )
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
				int len = src->moves[i].dim * src->moves[i].dim;

				dst->moves[i].tiles
				= calloc(
					len,
					sizeof( *(dst->moves[i].tiles) )
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
 * 
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
 * 
 * ---------------------------------------------------
 */
Gamedata *new_gamedata( void )
{
	Gamedata *gd = calloc( 1, sizeof(*gd) );
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "*** error: calloc() failed!");
		return NULL;
	}
	return gd;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
static inline bool _fp_skip_list( FILE *fp )
{
	char line[SZMAX_FLINE] = {'\0'};

	if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"fgets() failed to read the 1st line of the list!"
			);
		return false;
	}
	s_fixeol( line );
	if ( 0 != strcmp(line, "NULL:\n") ) {    /* non-NULL undo-list */
		long int count = 0;
		if ( sscanf(line, "%ld", &count) < 1 ) {
			DBG_STDERR_MSG(
				"sscanf() failed to read list's # of nodes"
				);
			return false;
		}
		/* skip remaining lines */
		for (long int i=1; i < count; i++) {
			if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
				DBG_STDERR_MSG(
					"fgets failed while skipping list!"
					);
				return false;
			}
		}
	}

	return true;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
bool gamedata_set_from_fname( Gamedata *gd, const char *fname )
{
	Gamedata test;
	FILE     *fp = NULL;
	char     line[SZMAX_FLINE] = {'\0'};

	/* sanity checks */
	if ( NULL == gd || NULL == fname ) {
		DBG_STDERR_MSG(
			"*** error: NULL pointer argument (gd OR fname)"
			);
		return false;
	}

	/*
	 * IMPORTANT: Open fname for reading in TEXT mode
	 *            so s_fixeol() will work as expected. 
	 */
	fp = fopen( fname, "r" );
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
	if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG( "fgets() failed to read the didundo line!" );
		goto ret_failure;
	}
	s_fixeol( line );
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

	if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"fgets() failed to read replay's meta-data line"
			);
		goto ret_failure;
	}
	s_fixeol( line );

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
//gamedata_print_tiles( gd );
	fclose( fp );
	return true;

ret_failure:
	_moves_free( test.moves, test.nmoves );
	fclose( fp );
	return false;
}

/* ---------------------------------------------------
 * 
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
 * 
 * ---------------------------------------------------
 */
int gamedata_get_didundo( Gamedata *gd )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;  /* false */
	}
	return gd->didundo;  /* true */
}

/* ---------------------------------------------------
 * 
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
 * 
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
 * 
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
 * 
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
 * 
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
 * 
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
 * 
 * ---------------------------------------------------
 */
int gamedata_get_tile_of_move( Gamedata *gd, int itile, long int imove )
{
	if ( NULL == gd ) {
		DBG_STDERR_MSG( "NULL pointer argument (gd) !");
		return 0;
	}

	if ( NULL == gd->moves ) {
		DBG_STDERR_MSG( "gd->moves is NULL!");
		return 0;
	}
	if ( NULL == gd->moves[imove].tiles ) {
		DBG_STDERR_MSG( "gd->moves[imove].tiles is NULL!");
		return 0;
	}

	return gd->moves[imove].tiles[itile];
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
void gamedata_print_tiles_of_move( Gamedata *gd, long int imove )
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
		printf( "%d ", gd->moves[imove].tiles[i] );
		fflush( stdout );
	}
	putchar( '\n' );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
void gamedata_print_tiles( Gamedata *gd )
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
			printf( "%d ", gd->moves[m].tiles[i] );
			fflush( stdout );
		}
		putchar( '\n' );
	}
}
