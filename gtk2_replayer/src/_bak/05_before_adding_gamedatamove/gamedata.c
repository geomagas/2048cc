#define GAMEDATA_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>       /* memset(), memcpy(), strcmp(), strncpy() */
#include <stdbool.h>

#include "misc.h"
#include "gamedata.h"

struct _GamedataMove {
	long int score;        /* current score */
	long int bscore;       /* best-score across multiple games */
	int      iswin;        /* has the sentinel value reached? */
	int      prevmv;       /* direction of previous move */
	int      nextmv;       /* direction of next move */
	int      *grid;
};

struct _Gamedata {
	char fname[MAXSIZ_DBGMSG]; /* name of loaded file */
	int           didundo;
	long int      nmoves;      /* total moves */
	int           dim;         /* square board's single-dimension */
	int           sentinel;    /* sentinel-tile value */
	int           nrandom;     /* number of spawned tiles */
	unsigned long delay;       /* animation delay (secs) */
	int           gamewon;     /* was this game won? */
	GamedataMove  *moves;
};

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
Gamedata *gamedata_free( Gamedata *gd )
{
	if ( gd ) {
		free( gd );
	}
	return gd;
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
#define MAX_LNSIZE  1024
bool gamedata_set_from_fname( Gamedata *gd, const char *fname )
{
	Gamedata test;
	FILE     *fp = NULL;
	char     line[MAX_LNSIZE] = {'\0'};
	char     dummy[MAX_LNSIZE];

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
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
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
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
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
			if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
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
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
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
			if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
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
	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
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

	if ( NULL == fgets(line, MAX_LNSIZE, fp) ) {
		DBG_STDERR_MSG(
			"fgets() failed to read 1st line of replay-list"
			);
		goto ret_failure;
	}
	s_fixeol( line );

	char *tokens[2] = { NULL };
	int  ntokens = 0;

	/* tokenize line on ':' (up to 2 tokens) */
	ntokens = s_tokenize( line, tokens, 2, ":" );
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
	strncpy( dummy, tokens[1], MAX_LNSIZE-1);
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

	/* everything ok, update gd */
	memcpy( gd, &test, sizeof(Gamedata) );
	strncpy( gd->fname, fname, MAXSIZ_DBGMSG-1 );

	fclose( fp );
	return true;

ret_failure:
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
	return gd->dim;
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
	return gd->sentinel;
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
	return gd->nrandom;
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
