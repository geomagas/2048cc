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
aa:score bscore iswin prev next@dim sentinel nrandom nempty hasadjacent#tile-values
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
	int      *tiles;
};

struct _Gamedata {
	char fname[MAXSIZ_DBGMSG]; /* name of loaded file */
	int           didundo;
	long int      nmoves;      /* total moves */
//	int           dim;         /* square board's single-dimension */
//	int           sentinel;    /* sentinel-tile value */
//	int           nrandom;     /* number of spawned tiles */
	unsigned long delay;       /* animation delay (secs) */
	int           gamewon;     /* was this game won? */
	GamedataMove  *moves;
};

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
Gamedata *_copy( Gamedata *dst, const Gamedata *src )
{
	if ( NULL == dst || NULL == src ) {
		DBG_STDERR_MSG( "NULL pointer argument!" );
		return NULL;
	}

	if ( src->moves ) {
		_moves_free( dst->moves, dst->nmoves );

		dst->moves = calloc( src->nmoves, sizeof( *(dst->moves) ) );
		if ( NULL == dst->moves ) {
			DBG_STDERR_MSG( "calloc(dst->moves) failed!" );
			return NULL;
		}

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
					_moves_free( dst->moves, i-1 );
					return NULL;
				}
				memcpy(
					dst->moves[i].tiles,
					src->moves[i].tiles,
					sizeof( *(dst->moves[i].tiles) )
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

	strncpy( dst->fname, src->fname, MAXSIZ_DBGMSG-1 );
	dst->didundo = src->didundo;
	dst->nmoves  = src->nmoves;
//	int           dim;         /* square board's single-dimension */
//	int           sentinel;    /* sentinel-tile value */
//	int           nrandom;     /* number of spawned tiles */
	dst->delay   = src->delay;
	dst->gamewon = src->gamewon;

	return dst;
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
/*
8:0 0 0 0 4@5 65536 1 23 0#0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 4 0 0 0 0 0 0 0 0\r\n
aa:score bscore iswin prev next@dim sentinel nrandom nempty hasadjacent#tile-values
*/
static inline bool _set_moves_from_fp(
	GamedataMove *moves,
	long int     nmoves,
	FILE         *fp
	)
{
	long int i;
	char line[SZMAX_FLINE] = {'\0'};
	char dummy[SZMAX_FLINE] = {'\0'};
	char *tokens[2] = { NULL };
	int  ntokens    = 0;

	for (i=0; i < nmoves; i++)
	{
		/* get next line in file */
		memset( line, 0, SZMAX_FLINE );
		if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
			DBG_STDERR_MSG(
				"fgets() failed to read 1st line of replay-list"
				);
			return false;
		}
		s_fixeol( line );

		/* tokenize line on ':' (up to 2 tokens) */
		ntokens = s_tokenize( line, tokens, 2, ":" );
		if ( ntokens < 2 ) {
			DBG_STDERR_MSG(
				"s_tokenize(:) failed to produce 2 tokens"
				);
			return false;
		}
		/* from the 2nd token read: score, bscore, iswin, prev, next */
		if ( sscanf(
			tokens[1],
			"%ld %ld %d %d %d",
			&moves[i].score,
			&moves[i].bscore,
			&moves[i].iswin,
			&moves[i].prevmv,
			&moves[i].nextmv
			) < 5
		){
			DBG_STDERR_MSG( "sscanf() failed!" );
			return false;
		}

		/* tokenize 2nd token on '@' (up to 2 tokens) */
//		memset( dummy, 0, SZMAX_FLINE );
		strncpy( dummy, tokens[1], SZMAX_FLINE-1 );
		ntokens = s_tokenize( dummy, tokens, 2, "@" );
		if ( ntokens < 2 ) {
			DBG_STDERR_MSG(
				"s_tokenize(@) failed to produce 2 tokens"
				);
			return false;
		}
		/* from the new 2nd token read: dim sentinel nrandom */
		if ( sscanf(
			tokens[1],
			"%d %d %d",
			&moves[i].dim,
			&moves[i].sentinel,
			&moves[i].nrandom
			) < 3
		){
			DBG_STDERR_MSG(
				"sscanf(dim, sentinel, nrandom) failed!"
				);
			return false;
		}
		/* tokenize the new 2nd token on '#' (up to 2 tokens) */
//		memset( dummy, 0, SZMAX_FLINE );
		strncpy( dummy, tokens[1], SZMAX_FLINE-1 );
		ntokens = s_tokenize( dummy, tokens, 2, "#" );
		if ( ntokens < 2 ) {
			DBG_STDERR_MSG(
				"s_tokenize(#) failed to produce 2 tokens"
				);
			return false;
		}

		/* from the new 2nd token read: tile-values */
		const int len = moves[i].dim * moves[i].dim;
		moves[i].tiles = calloc( len, sizeof( moves[i].tiles ) );
		if ( NULL == moves[i].tiles ) {
			DBG_STDERR_MSG(
				"calloc(moves[i].tiles) failed!"
				);
			return false;
		}
		for (int n=0; n < len; n++) {
			if ( sscanf(tokens[1], "%d", &moves[i].tiles[n]) < 1 ){
				DBG_STDERR_MSG(
					"sscanf(tiles[n]) failed!"
					);
				free( moves[i].tiles );
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
	char     dummy[SZMAX_FLINE];

	if ( NULL == gd || NULL == fname ) {
		DBG_STDERR_MSG(
			"*** error: NULL pointer argument (gd OR fname)"
			);
		return false;
	}

	fp = fopen( fname, "r" );
	if ( NULL == fp ) {
		DBG_STDERR_MSG( "*** error: couldn't open requested file!" );
		return false;
	}

	memset( &test, 0, sizeof(test) );

	/*
	 * read didundo value
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

	/*
	 * skip the undo-list
	 */
	if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"fgets() failed to read the 1st line of the undo-list!"
			);
		goto ret_failure;
	}
	s_fixeol( line );
	if ( 0 != strcmp(line, "NULL:\n") ) {    /* non-NULL undo-list */
		long int count = 0;
		if ( sscanf(line, "%ld", &count) < 1 ) {
			DBG_STDERR_MSG(
				"sscanf() failed to read undo-list's # of nodes"
				);
			goto ret_failure;
		}
		/* skip remaining lines */
		for (long int i=1; i < count; i++) {
			if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
				DBG_STDERR_MSG(
					"fgets failed while skipping undo-list!"
					);
				goto ret_failure;
			}
		}
	}

	/*
	 * skip the redo-list
	 */
	if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"fgets() failed to read the 1st line of the redo-list!"
			);
		goto ret_failure;
	}
	s_fixeol( line );
	long int count = 0;
	if ( 0 != strcmp(line, "NULL:\n") ) {    /* non-NULL undo-list */
		if ( sscanf(line, "%ld", &count) < 1 ) {
			DBG_STDERR_MSG(
				"sscanf() failed to read redo-list's # of nodes"
				);
			goto ret_failure;
		}
		/* skip remaining lines */
		for (long int i=1; i < count; i++) {
			if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
				DBG_STDERR_MSG(
					"fgets failed while skipping redo-list!"
					);
				goto ret_failure;
			}
		}
	}

	/*
	 * read the replay data
	 */

	/*
	 * first the meta-data
	 */
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

	/*
	 * then the actual replay-list
	 */

	/* allocate space for nmoves moves */
	test.moves = calloc( test.nmoves, sizeof(GamedataMove) );
	if ( NULL == test.moves ) {
		DBG_STDERR_MSG(
			"calloc(test.nmoves) failed!"
			);
		goto ret_failure;
	}

#if 1
	if  ( !_set_moves_from_fp( test.moves, test.nmoves, fp) ) {
		DBG_STDERR_MSG( "_set_moves_from_fp() failed" );
		goto ret_failure;
	}
#else
	/* 1st line of replay-list */
	if ( NULL == fgets(line, SZMAX_FLINE, fp) ) {
		DBG_STDERR_MSG(
			"fgets() failed to read 1st line of replay-list"
			);
		goto ret_failure;
	}
	s_fixeol( line );

	/* tokenize line on ':' (up to 2 tokens) */
	char *tokens[2] = { NULL };
	int  ntokens = s_tokenize( line, tokens, 2, ":" );
	if ( ntokens < 2 ) {
		DBG_STDERR_MSG( "s_tokenize(line) failed to produce 2 tokens" );
		goto ret_failure;
	}
	/* from the 2nd token, get the 3rd value (gamewon) */
	if ( sscanf(
		tokens[1],
		"%s %s %d",
		dummy, dummy, &test.gamewon
		) < 3
	){
		DBG_STDERR_MSG( "sscanf() failed to read the gamewon value!" );
		goto ret_failure;
	}
	/* tokenize 2nd token on '@' (up to 2 tokens) */
	strncpy( dummy, tokens[1], SZMAX_FLINE-1);
	ntokens = s_tokenize( dummy, tokens, 2, "@" );
	if ( ntokens < 2 ) {
		DBG_STDERR_MSG( "s_tokenize(line) failed to produce 2 tokens" );
		goto ret_failure;
	}
	/* from the 2nd token, get the first 3 values */
	if ( sscanf(
		tokens[1],
		"%d %d %d",
		&test.dim, &test.sentinel, &test.nrandom
		) < 3
	){
		DBG_STDERR_MSG( "sscanf() failed to read the gamewon value!" );
		goto ret_failure;
	}
#endif

	/* everything ok, update gd */
	_copy( gd, &test );

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
