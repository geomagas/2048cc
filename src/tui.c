/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 18, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: con_color.h, my.h, common.h, board.h,
 *               gs.h, mvhist.h, tui_skin.h
 * --------------------------------------------------------------
 *
 * Private implementation of the Tui "class".
 *
 * The "class" realizes a primitive text-user-interface for the game.
 * The accompanying header file "tui.h" exposes publicly the "class"
 * as an opaque data-type.
 *
 * Functions having the "tui_sys_" prefix in their names do NOT operate
 * on tui objects. They are mostly direct wrappers of functions that are
 * implemented in the file: my.c.  
 *
 * Functions with a "_" prefix in their names are meant to be private
 * in this source-module. They are usually inlined, without performing
 * any sanity check on their arguments.
 *
 * Layout Entities
 * ---------------
 *
 * The dimensions of the console/terminal window during launch,
 * is considered the full-screen (if it less than 80x24, the
 * constructor function returns an error).
 *
 * The left-half consists of the following entities (top to bottom):
 * - title-bar
 * - board of tiles
 * - scores-bar   (shows the current & best score)
 * - info-bar     (shows messages like the winning message)
 *
 * The right-half consists of the following entities (top to bottom):
 * - help-box     (shows the game instructions & commands)
 * - iobar2       (iobar supporting messages)
 * - iobar        (used for prompting user-input)
 *
 * The positions & dimensions of the above entities are stored in
 * corresponding _scrbox structs, which all together are kept in:
 * (_struct scrlayout) tui->layout;
 *
 * The split of the screen in 2 halves is only conceptual,
 * it is NOT implemented distinctly.
 *
 * Skins
 * ------
 *
 * Each of the layout entities has also a foreground and a
 * background color associated with it, via the currently
 * active skin.
 *
 * The skin is accessed via a pointer to an opaque object
 * of type TuiSkin: (TuiSkin *)tui->skin;
 *
 * It is implemented in a different source module (tui_skin.c)
 * 
 *****************************************************************
 */

#define TUI_C

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tui.h"
#include "tui_skin.h"
#include "my.h"
#include "con_color.h"
#include "common.h"
#include "board.h"
#include "gs.h"
#include "mvhist.h"

/* single screen-box (screen area) */
struct _scrbox {
	int x,y;
	int w,h;
};
/* screen-layout */
struct _scrlayout {
	struct _scrbox full;

	/* left half of the screen*/
	struct _scrbox titlebar;
	struct _scrbox board;
	struct _scrbox tile;
	struct _scrbox scoresbar;
	struct _scrbox infobar;

	/* right half of the screen*/
	struct _scrbox helpbox;
	struct _scrbox iobar2;
	struct _scrbox iobar;
};

/* text user interface */
struct _tui {
	GameState         *state;
	MovesHistory      *mvhist;
	struct _scrlayout layout;
	TuiSkin           *skin;
};

int tui_cls( const Tui *tui );

/* --------------------------------------------------------------
 * Clear the console screen to system colors.
 * --------------------------------------------------------------
 */
int tui_sys_cls( void )
{
	return my_cls();
}

/* --------------------------------------------------------------
 * Hide the console cursor.
 * --------------------------------------------------------------
 */
int tui_sys_cursor_off( void )
{
	return 	my_cursor_onoff(0);
}

/* --------------------------------------------------------------
 * Show the console cursor.
 * --------------------------------------------------------------
 */
int tui_sys_cursor_on( void )
{
	return 	my_cursor_onoff(1);
}

/* --------------------------------------------------------------
 * Get a key from stdin in unbuffered mode.
 * --------------------------------------------------------------
 */
int tui_sys_getkey( unsigned int *outKeyMask )
{
	return my_getch( outKeyMask );
}

/* --------------------------------------------------------------
 * Pause until the user presses a key.
 * --------------------------------------------------------------
 */
void tui_sys_press_a_key( void )
{
	unsigned int unused;

	printf( "press any key..." );
	fflush( stdout );

	my_getch( &unused );
}

/* --------------------------------------------------------------
 * Beep the PC speaker as may times as specified (ntimes).
 * --------------------------------------------------------------
 */
void tui_sys_beep( size_t ntimes )
{
	while ( ntimes-- > 0 ) {
		putchar( '\a' );
	}
	fflush( stdout );
}

/* --------------------------------------------------------------
 * Sleep for the specified milliseconds
 * --------------------------------------------------------------
 */
int tui_sys_sleep( unsigned long int msecs )
{
	return my_sleep_msecs( msecs );
}

/* --------------------------------------------------------------
 * char *_get_replay_fname_from_user():
 *
 * --------------------------------------------------------------
 */
char *_get_replay_fname_from_user( char *fname )
{
	size_t idxbeg = strlen( REPLAYS_FOLDER "/");/* star-index of pure fname */
	size_t szfname = SZMAX_FNAME - idxbeg;   /* remaining sz for pure fname*/
	char *cp = NULL;

	if ( NULL == fname ) {
		DBGF( "%s", "NULL pointer argument (fname)!" );
		return NULL;
	}

	strncpy( fname, REPLAYS_FOLDER "/", SZMAX_FNAME-1 );
	fname[ SZMAX_FNAME-1 ] = '\0';

	cp = &fname[idxbeg];
	s_getflushed( cp, szfname );

	return fname;
}

/* --------------------------------------------------------------
 * int _printfxy():
 *
 * This function is an enhanced printf(). It expects 4 additional
 * leading arguments, specifying the foreground (fg) & background
 * (bg) colors for the output, along with its starting position on
 * the console screen (x,y).
 * --------------------------------------------------------------
 */
int _printfxy(
	const ConSingleColor fg,
	const ConSingleColor bg,
	int x,
	int y,
	const char *fmt,
	...
	)
{
	int  ret = 0;
	char *txtout = NULL;
	va_list vargs;

	va_start( vargs, fmt );
	txtout = vprintf_to_text( fmt, vargs );
	va_end( vargs );
	if ( NULL == txtout ) {
		return -1;
	}

	my_gotoxy(x,y);
	CONOUT_NPRINTF(
		ret,        /* gets updated to the return value of printf() */
		fg, bg,
		"%s",
		txtout
		);
	free( txtout );

	return ret;
}

/* --------------------------------------------------------------
 * int _put_hspan_centered():
 *
 * Using the given foreground & background colors (fg, bg), and
 * starting at the current position of the console cursor, center
 * and display the specified cstring (text) across the specified
 * number of columns (ncols). Return 0 on error, 1 otherwise.
 * --------------------------------------------------------------
 */
static int _put_hspan_centered(
	const ConSingleColor fg,
	const ConSingleColor bg,
	const char *text,
	int ncols
	)
{
	int ycurs = my_gety();
	int xcurs = my_getx();
	int x;
	int len;

	if ( NULL == text ) {
		return 0;
	}

	len = strlen( text );
	if ( len >= ncols ) {
		CONOUT_PRINTF( fg, bg, "%s", text );
		return 1;
	}

	CONOUT_PAINT_NTIMES(bg, ncols);

	x = xcurs + (ncols - len) / 2;
	my_gotoxy( x, ycurs );
	CONOUT_PRINTF( fg, bg, "%s", text );

	return 1;
}

/* --------------------------------------------------------------
 * int _putxy_hspan_centered():
 *
 * Using the given foreground & background colors (fg, bg), and
 * starting at the specified cursor position (x,y), center and
 * display the specified cstring (text) across the specified
 * number of columns (ncols). Return 0 on error, 1 otherwise.
 * --------------------------------------------------------------
 */
int _putxy_hspan_centered(
	const ConSingleColor fg,
	const ConSingleColor bg,
	int x,
	int y,
	const char *text,
	int ncols
	)
{
	int len;

	if ( NULL == text ) {
		return 0;
	}

	my_gotoxy( x,y );

	len = strlen( text );
	if ( len >= ncols ) {
		CONOUT_PRINTF( fg, bg, "%s", text );
		return 1;
	}

	CONOUT_PAINT_NTIMES( bg, ncols );

	x += (ncols - len) / 2;
	my_gotoxy( x, y );
	CONOUT_PRINTF( fg, bg, "%s", text );

	return 1;
}

/* --------------------------------------------------------------
 * int _int_count_digits():
 *
 * Return the digits count of the given integer (0 counts as 1 digit,
 * for negative numbers the minus sign in not counted).
 *
 * NOTE: Used only in this module, in the function: _draw_tileval_at_xy().
 * --------------------------------------------------------------
 */
static int _int_count_digits( int num )
{
	int i = 0;

	if ( 0 == num ) {
		return 1;
	}
	while ( num ) {
		num /= 10;
		i++;
	}
	return i;
}

/* --------------------------------------------------------------
 * const ConColors *_tileval_to_colors():
 *
 * Depending on the specified tile-value (val), query the skin of
 * the specified tui for the corresponding foreground & background
 * colors, and return them.
 *
 * These colors are returned in the form of pointer to a ConColors
 * structure, which is available through the con_color.h preprocessor
 * interface.
 *
 * NOTE: Used exclusively in the function _draw_tileval_at_xy(),
 *       so it depends heavily on assumptions made in that context.
 * --------------------------------------------------------------
 */
static inline const ConColors *_tileval_to_colors( int val, const Tui *tui )
{
	switch ( val )
	{
		case 0:
			return tui_skin_get_colors_tile0( tui->skin );
		case 2:
			return tui_skin_get_colors_tile2( tui->skin );
		case 4:
			return tui_skin_get_colors_tile4( tui->skin );
		case 8:
			return tui_skin_get_colors_tile8( tui->skin );
		case 16:
			return tui_skin_get_colors_tile16( tui->skin );
		case 32:
			return tui_skin_get_colors_tile32( tui->skin );
		case 64:
			return tui_skin_get_colors_tile64( tui->skin );
		case 128:
			return tui_skin_get_colors_tile128( tui->skin );
		case 256:
			return tui_skin_get_colors_tile256( tui->skin );
		case 512:
			return tui_skin_get_colors_tile512( tui->skin );
		case 1024:
			return tui_skin_get_colors_tile1024( tui->skin );
		case 2048:
			return tui_skin_get_colors_tile2048( tui->skin );
		default:
			return tui_skin_get_colors_tilebig( tui->skin );
	}

	return NULL;
}

/* --------------------------------------------------------------
 * int _draw_tileval_at_xy():
 *
 * Draw the specified tile-value at the specified position (x,y)
 * on the console screen. Return 0 (false) on error, 1 (true)
 * otherwise.
 *
 * NOTE: Used exclusively in the function tui_draw_board(),
 *       so it depends heavily on assumptions made in that
 *       context.
 * --------------------------------------------------------------
 */
static inline int _draw_tileval_at_xy(
	const Tui *tui,
	int       tileval,
	int       x,
	int       y
	)
{
	int i;
	const ConColors *tc;                  /* tile colors */
	const int wtile = tui->layout.tile.w; /* drawing width of any tile */
	const int htile = tui->layout.tile.h; /* drawing height of any tile*/

	tc  = _tileval_to_colors( tileval, tui );

	/* first draw the tile box */
	for (i=0; i < htile; i++) {
		my_gotoxy( x, y+i );
		CONOUT_PAINT_NTIMES( tc->bg, wtile );
	}

	/* then print tile value at the center */
	int vw = _int_count_digits( tileval ); /* val width */
	int cx = x + (wtile - vw) / 2;         /* centered x of val */
	int cy = y + htile/2;                  /* centered y of val */
	my_gotoxy( cx, cy );
	CONOUT_PRINTF( tc->fg, tc->bg, "%d", tileval );

	return 1;
}

/* --------------------------------------------------------------
 * void _clear_iobar():
 *
 * Clear the 1st io-bar of the specified tui object.
 * --------------------------------------------------------------
 */
static void _clear_iobar( const Tui *tui )
{
	const ConColors *cc = tui_skin_get_colors_iobar( tui->skin );
	my_gotoxy(
		tui->layout.iobar.x,
		tui->layout.iobar.y
		);
	CONOUT_PAINT_NTIMES(
		cc->bg,
		tui->layout.iobar.w
		);
}

/* --------------------------------------------------------------
 * void _clear_iobar2():
 *
 * Clear the 2nd io-bar of the specified tui object.
 * --------------------------------------------------------------
 */
static void _clear_iobar2( const Tui *tui )
{
	const ConColors *cc = tui_skin_get_colors_iobar2( tui->skin );
	my_gotoxy(
		tui->layout.iobar2.x,
		tui->layout.iobar2.y
		);
	CONOUT_PAINT_NTIMES(
		cc->bg,
		tui->layout.iobar2.w
		);
}

/* --------------------------------------------------------------
 * void _scrbox_set():
 *
 * Set the specified starting coords & dimensions (x,y,w,h)
 * of the specified screen-box object (box).
 * --------------------------------------------------------------
 */
static void _scrbox_set( struct _scrbox *box, int x, int y, int w, int h )
{
	box->x = x;
	box->y = y;
	box->w = w;
	box->h = h;
}

/* --------------------------------------------------------------
 * void _scrbox_tile_set_from_dim():
 *
 * Set the dimensions of the specified screen-box object (tile)
 * according to the specified single-dimension (dim) of a square
 * board.
 *
 * NOTES: The purpose of this function is to set the drawing width
 *        and height of any single tile, assuming that it will be
 *        part of a square board with the specified single-dimension.
 *
 *        The values of the assigned width & height are hard-coded,
 *        so that when all the tiles of the board are drawn on the
 *        console screen, the board will fit both horizontally and
 *        vertically to a conceptually predefined area on the screen.
 *
 *        This conceptually predefined area has been determined by
 *        trial & error observations during game-play, until the 
 *        hard-coded tile dimensions gave the desired visual result.
 *
 *        The functions is exclusively used by the func: _init_layout()
 *        which is in turn used for initializing all the screen-layout
 *        entities of the tui.
 * --------------------------------------------------------------
 */
static void _scrbox_tile_set_from_dim( struct _scrbox *box, int dim )
{
	int w,h;

	switch ( dim ) {
		case BOARD_DIM_4:
			w = 9;
			h = 5;
			break;

		case BOARD_DIM_5:
			w = 7;
			h = 3;
			break;

		case BOARD_DIM_6:
			w = 6;
			h = 3;
			break;

		case BOARD_DIM_8:
			w = 5;
			h = 2;
			break;

		default:
			w = -1;
			h = -1;
			DBGF( "tui does NOT support %dx%d boards!", dim, dim );
			break;
	}

	_scrbox_set( box, -1,-1, w,h );
}

/* --------------------------------------------------------------
 * int _init_layout():
 *
 * Initialize the screen layout of the specified tui object.
 * Entities that depend on the game board, are set according
 * to the 2nd argument, while the rest are set to default values.
 * Return 0 (false) if the the current dimensions of the console
 * window is less than 80x24 characters.
 * --------------------------------------------------------------
 */
static int _init_layout( Tui *tui, const Board *board )
{
	/* for brevity later on (full-screen box) */
	int sx = 0;
	int sy = 0;
	int sw = my_console_width();
	int sh = my_console_height();

	/* demand a console window no smaller than 80x24 */
	if ( (sw && sw < 80) || (sh && sh < 24) ) {
		DBGF( "%s", "Console window is smaller than 80x24!" );
		return 0;  /* false */
	}

	/*
	 * First set the tiles' width & height, based on the
	 * single-dimension of the game board. These values
	 * are used for setting the board screen-box, which
	 * in turn is used in most of the remaining calculations.
	 */
	_scrbox_tile_set_from_dim(
		&tui->layout.tile,
		board_get_dim( board )
		);

	/* for brevity later on (board box) */
	const int bx = sx + 1;
	const int by = sx + 1;
	const int bw = board_get_dim(board) * tui->layout.tile.w;
	const int bh = board_get_dim(board) * tui->layout.tile.h;     

	/* for brevity later on (help-box) */
	const int hx = 41;
	const int hy = sy;
	const int hw = 38;
	const int hh = 22;     

	/* full-screen */
	_scrbox_set(
		&tui->layout.full,
		sx, sy,
		sw, sh
		);

	/*
	 * Left half of the screen.
	 */

	/* title-bar (h-aligned to the board, 1 line above it) */
	_scrbox_set(
		&tui->layout.titlebar,
		bx, (by - 1),
		bw, 1
		);

	/* board (fixed top-left corner) */
	_scrbox_set(
		&tui->layout.board,
		bx, by,
		bw, bh
		);

	/* scores-bar (h-aligned to the board, 2 lines below it) */
	_scrbox_set(
		&tui->layout.scoresbar,
		bx, (1 + by + bh),
		bw, 1
		);

	/* info-bar (h-aligned to the board, 1 line below scores-bar) */
	_scrbox_set(
		&tui->layout.infobar,
		bx, 1 + tui->layout.scoresbar.y,//(sh - 1),
		bw, 1
		);

	/*
	 * Right half of the screen.
	 */

	/* help-box (fixed at the middle of the 1st line of the screen) */
	_scrbox_set(
		&tui->layout.helpbox,
		hx, hy,
		hw, hh
		);

	/* iobar2 (h-aligned to help-box, 1 line below it) */
	_scrbox_set(
		&tui->layout.iobar2,
		hx, hy + hh, // tui->layout.infobar.y - 1, //(sh - 2),
		hw, 1
		);
	/* iobar (h-aligned to help-box, 1 line below iobar2) */
	_scrbox_set(
		&tui->layout.iobar,
		hx, 1 + tui->layout.iobar2.y, //(sh - 1),
		hw, 1
		);

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * Tui *tui_release():
 *
 * The tui destructor releases the memory reserved for the specified
 * Tui object, it restores the console colors, it enables the cursor,
 * and finally it returns NULL (so the caller may assign it to the
 * object pointer).
 * --------------------------------------------------------------
 */
Tui *tui_release( Tui *tui )
{
	if ( tui ) {
		tui_skin_release( tui->skin );
		free( tui );
	}

	CONOUT_RESTORE();    /* restore console colors */
	tui_sys_cursor_on(); /* ensure the cursor is enabled */
	tui_sys_cls();       /* clear screen to system's default colors */

	return NULL;
}

/* --------------------------------------------------------------
 * Tui *new_tui():
 *
 * The tui constructor reserves memory for a Tui object, it initializes
 * it to default values, it saves the console colors, it hides the cursor,
 * it clears the screen (using the default skin's bg color), and finally
 * it returns a pointer to the created Tui object.
 * On error, it returns NULL.
 * --------------------------------------------------------------
 */
Tui *new_tui( GameState *state, MovesHistory *mvhist )
{
	Tui *tui           = NULL;
	const Board *board = NULL;

	if ( NULL == state ) {
		DBGF( "%s", "NULL pointer argument (state)!" );
		return NULL;
	}
	if ( NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument (mvhist)!" );
		return NULL;
	}

	board = gamestate_get_board( state );
	if ( NULL == board ) {
		DBGF( "%s", "The game-state contains a NULL board!" );
		return NULL;
	}

	tui = calloc( 1, sizeof(*tui) );
	if ( NULL == tui ) {
		DBGF( "%s", "calloc() failed!" );
		return NULL;
	}

	CONOUT_INIT();

	tui->skin = new_tui_skin();
	if ( NULL == tui->skin ) {
		DBGF( "%s", "skin allocation failed!" );
		tui_release( tui );    /* this also calls CONOUT_RESTORE() */
		return NULL;
	}

	if ( !_init_layout(tui, board) ) {
		tui_release( tui );    /* this also calls CONOUT_RESTORE() */
		return NULL;
	}

	tui->state  = state;
	tui->mvhist = mvhist;

	tui_sys_cursor_off();
	tui_cls( tui );

	return tui;
}

/* --------------------------------------------------------------
 * int tui_update_board_reference():
 *
 * Update the board referencing pointer of the specified tui object,
 * so it points to the board object specified by the second argument.
 * 
 * NOTES: This function should be called after the board of the
 *        game has been resized. A pointer to the resized board
 *        object is passed as the second argument.
 *
 *        Besides updating the internal board reference of the
 *        tui object, the function also re-initializes the screen
 *        layout of the tui object, according to the resized board.
 *
 *        The board reference is initially assigned during the creation
 *        of a tui object (via new_tui() ), but if later-on the original
 *        board gets externally resized via realloc(), it may have been
 *        moved to a different location in memory, because that's how
 *        realloc() works.
 *
 *        So it is essential to call this function every time the
 *        original board gets resized.
 * --------------------------------------------------------------
 */
int tui_update_board_reference( Tui *tui, Board *board )
{
	if ( NULL == tui || NULL == board ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;  /* false */
	}
	if ( !_init_layout(tui, board) ) {
		DBGF( "%s", "Layout initialization failed!" );
		return 0;  /* false */
	}
	gamestate_set_board_reference( tui->state, board );
//	tui->board = board;

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int tui_update_mvhist_reference():
 *
 * --------------------------------------------------------------
 */
int tui_update_mvhist_reference( Tui *tui, MovesHistory *mvhist )
{
	if ( NULL == tui || NULL == mvhist ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;  /* false */
	}
//	if ( !_init_layout(tui, board) ) {
//		DBGF( "%s", "Layout initialization failed!" );
//		return 0;  /* false */
//	}
	tui->mvhist = mvhist;

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * int tui_cls():
 *
 * Clear the console screen using the screen background color of
 * the specified tui object.
 *
 * NOTE: This is different from the function: tui_sys_cls() which
 *       clears the console screen using the background color of
 *       the underlying system.
 * --------------------------------------------------------------
 */
int tui_cls( const Tui *tui )
{
	int w,h;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return 0;  /* false */
	}

	w  = my_console_width();
	h  = my_console_height();
	cc = tui_skin_get_colors_screen( tui->skin );

	my_gotoxy(0,0);
	while ( h-- > -1 ) {
		CONOUT_PAINT_NTIMES( cc->bg, w );
	}
	my_gotoxy(0,0);

	return 1;  /* true */
}

/* --------------------------------------------------------------
 * void tui_draw_titlebar():
 *
 * Draw the game title on the console screen, using the colors and
 * the positions dictated by the corresponding layout & skin entities
 * of the specified tui object.
 *
 * NOTE: Since we have a rather primitive text-user-interface,
 *       the function displays a fixed text.
 *
 *       Advanced interfaces utilize complex entities (widgets)
 *       which among several other things contain internally
 *       their text, their colors, their screen positions, and
 *       any other attribute of theirs. Then each attribute can
 *       be set separately, before drawing the widget on the
 *       screen.
 * --------------------------------------------------------------
 */
void tui_draw_titlebar( const Tui *tui )
{
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	cc = tui_skin_get_colors_titlebar( tui->skin );
	_putxy_hspan_centered(
		cc->fg,
		cc->bg,
		tui->layout.titlebar.x,
		tui->layout.titlebar.y,
		"2048 Console Clone",
		tui->layout.titlebar.w     /* # of spanning columns */
		);
}

/* --------------------------------------------------------------
 * void tui_draw_scoresbar():
 *
 * Draw on the console screen the scores-bar of the specified tui
 * object, containing the specified score and best-score (bscore).
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_scoresbar( const Tui *tui )
{
	int nchars;
	int x, y;                   /* scoresbar top-left position */
	const ConColors *sb = NULL; /* scoresbar colors */

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	x  = tui->layout.scoresbar.x;
	y  = tui->layout.scoresbar.y;
	sb = tui_skin_get_colors_scoresbar( tui->skin );

	nchars =  _printfxy(
			sb->fg, sb->bg,
			x, y,
			"Score: %-6ld",
			gamestate_get_score( tui->state )
			);

	x += (nchars + 3);
	_printfxy(
		sb->fg, sb->bg,
		x, y,
		"Best: %-6ld",
		gamestate_get_bestscore( tui->state )
		);
}

/* --------------------------------------------------------------
 * void tui_draw_help():
 * 
 * Draw on the console screen the help-box of the specified tui
 * object.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_help( const Tui *tui, int isenabledcommands )
{
	int x,y, wspan;
	const ConColors *hb = NULL;  /* help box colors */
	const ConColors *hh = NULL;  /* help header colors */
	const ConColors *hc = NULL;  /* help commands colors */
	const ConColors *hf = NULL;  /* help footer colors */

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	x = tui->layout.helpbox.x;
	y = tui->layout.helpbox.y;
	wspan = tui->layout.helpbox.w;

	hb  = tui_skin_get_colors_help_box( tui->skin );
	hh  = tui_skin_get_colors_help_header( tui->skin );
	hc  = isenabledcommands
		? tui_skin_get_colors_help_commands( tui->skin )
		: tui_skin_get_colors_help_commands_disabled( tui->skin )
		;
	hf  = tui_skin_get_colors_help_footer( tui->skin );

	/* header */
	_putxy_hspan_centered(
		hh->fg, hh->bg,
		x,y,
		"HOW TO PLAY",
		wspan
		);

	/* normal contents of help-box */

	y += 2;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "Use your arrow keys to move the tiles."
		);
	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "If 2 tiles with the same number touch,"
		);

	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "they are summed up & merged into one!"
		);
	y += 2;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "If the tiles're already stacked-up to"
		);
	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "the given direction, with no adjacent"
		);
	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "equal tiles, then nothing happens."
		);

	y += 2;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "After a move, a new tile is generated"
		);
	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "randomly, having either 2 or 4."
		);

	y += 2;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "The game ends if a tile reaches "
		);
	CONOUT_PRINTF(
		hc->fg, hc->bg,
		"%d",
		board_get_sentinel( gamestate_get_board(tui->state) )
		);

	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "or when there are no moves available"
		);

	y++;
	_printfxy(
		hb->fg, hb->bg,
		x, y,
		"%s", "at any direction."
		);

	/* commands */
	y += 2;
	_printfxy(
		hc->fg, hc->bg,
		x, y,
		"%s", "S)kin  U)ndo  Re)do  Rep)lay  R)eset"
		);
	y++;
	_printfxy(
		hc->fg, hc->bg,
		x, y,
		"%s", "4)x4   5)x5   6)x6   8)x8     Q)uit"
		);

	/* footer */
	y += 2;
	_putxy_hspan_centered(
		hf->fg, hf->bg,
		x,y,
		"free software (c) 2014 migf1",
		wspan
		);
}

/* --------------------------------------------------------------
 * Clear and draw on the console screen the info-bar of the
 * specified tui object.
 * --------------------------------------------------------------
 */
void tui_clear_infobar( const Tui *tui )
{
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	my_gotoxy(
		tui->layout.infobar.x,
		tui->layout.infobar.y
		);
	cc = tui_skin_get_colors_infobar( tui->skin );
	CONOUT_PAINT_NTIMES(
		cc->bg,
		tui->layout.infobar.w
		);
}

/* --------------------------------------------------------------
 * void tui_draw_infobar_boardinfo():
 *
 * Draw on the console screen the info-bar of the specified tui
 * object, containing board information.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_infobar_boardinfo( const Tui *tui )
{
	const Board *board  = NULL;
	const ConColors *cc = NULL;
	char txtout[BUFSIZ] = {'\0'};
	int dim;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}
	if ( NULL == tui->state ) {
		DBGF( "%s", "tui->state is NULL!" );
		return;
	}

	tui_clear_infobar( tui );

	board = gamestate_get_board( tui->state );
	dim = board_get_dim( board );
	snprintf(
		txtout,
		BUFSIZ,
		"%dx%d board | target: %d | %d random",
		dim,
		dim,
		board_get_sentinel( board ),
		board_get_nrandom( board )
		);

	cc = tui_skin_get_colors_infobar( tui->skin );

	_putxy_hspan_centered(
		cc->fg,
		cc->bg,
		tui->layout.infobar.x,
		tui->layout.infobar.y,
		txtout,
		tui->layout.infobar.w    /* # of spanning columns */
		);
}

/* --------------------------------------------------------------
 * void tui_draw_infobar_winmsg():
 *
 * Draw on the console screen the info-bar of the specified tui
 * object, containing the winning message.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_infobar_winmsg( const Tui *tui )
{
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	tui_clear_infobar( tui );

	cc = tui_skin_get_colors_infobar( tui->skin );
	_putxy_hspan_centered(
		cc->fg,
		cc->bg,
		tui->layout.infobar.x,
		tui->layout.infobar.y,
		"You won the game!",
		tui->layout.infobar.w    /* # of spanning columns */
		);
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_undo():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * prompting for undo confirmation. Return the key pressed by the
 * player, or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_undo( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );

	tui_sys_cursor_on();
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"%s", "Undo cancels best-score. Do it (y/)? "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_newgame():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * prompting for a new game. Return the key pressed by the player,
 * or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_newgame( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );

	tui_sys_cursor_on();
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"%s", "Start a new game (y/)? "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_watchreplay():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * containing the game-over message, and prompting for any keypress.
 * Return the key pressed by the player, or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_watchreplay( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );

	tui_sys_cursor_on();
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"%s", "GAME OVER! Watch replay (y/)? "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_watchreplay():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * containing the not-yet-implemented message, and prompting for any
 * keypress. Return the key pressed by the player, or TUI_KEY_NUL on
 * error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_notyet( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );

	tui_sys_cursor_on();
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"%s", "Not implemented yet! Press a key... "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_replaycommand():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * prompting for  replay command. Return the key pressed by the player,
 * or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_replaycommand(
	const Tui    *tui,
	unsigned int *keymask
	)
{
	int ret = TUI_KEY_NUL;
//	const GSNode *itnext = NULL;
//	const char *nextmvLabel = NULL;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}
	if ( NULL == keymask ) {
		DBGF( "%s", "NULL pointer argument (keymask)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );

	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"S)ave  L)oad  B)ack       :%s|%s",
		gamestate_get_prevmove_label( tui->state ),
		gamestate_get_nextmove_label( tui->state )
		);
	*keymask = TUI_KEYMASK_RESET;
	ret = tui_sys_getkey( keymask );

	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_savereplay():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * asking confirmation for saving a new replay. Return the key pressed
 * by the player, or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_savereplay( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );
	tui_sys_cursor_on();

	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"Will be saved as shown above (y/)? "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_loadreplay():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * asking confirmation for loading a new replay. Return the key pressed
 * by the player, or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_loadreplay( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );
	tui_sys_cursor_on();

	tui_sys_beep(1);
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"%s",
		"GAME WILL BE REPLACED! Do it (y/)? "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * int tui_draw_iobar_prompt_loadreplay_nofile():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * informing that the requested file could not be opened. Return the
 * key pressed by the player, or TUI_KEY_NUL on error.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
int tui_draw_iobar_prompt_loadreplay_nofile( const Tui *tui )
{
	int ret = TUI_KEY_NUL;
	unsigned int unused;
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return TUI_KEY_NUL;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );
	tui_sys_cursor_on();

	tui_sys_beep(1);
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"%s",
		"Replay file NOT found! Press any key "
		);
	ret = tui_sys_getkey( &unused );

	tui_sys_cursor_off();
	_clear_iobar( tui );

	return ret;
}

/* --------------------------------------------------------------
 * void tui_draw_iobar2_replaynavigation():
 *
 * Draw on the console screen the io-bar2 of the specified tui object,
 * containing the replay navigation commands, along with move-counter
 * info.
 *
 * NOTES: Used in replay-mode.
 * 
 *        Read the comments of the function: tui_draw_titlebar()
 *        for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar2_replaynavigation( const Tui *tui )
{
	const ConColors *cc = NULL;     /* iobar2 colors */
	const ConColors *hc = NULL;     /* help-box colors */
	long int nmoves=0, imove=0, nchars=0;
	long int nredo = 0;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	cc = tui_skin_get_colors_iobar2( tui->skin );
	hc = tui_skin_get_colors_help_box( tui->skin );

	nmoves = mvhist_get_replay_nmoves( tui->mvhist );
	imove  = nmoves - mvhist_get_replay_itcount( tui->mvhist ) + 1;
	nredo  = mvhist_peek_redo_stack_count( tui->mvhist );

	_clear_iobar2( tui );
	nchars = _printfxy(
			cc->fg,
			cc->bg,
			tui->layout.iobar2.x,
			tui->layout.iobar2.y,
			"Home  <-  p)lay  ->  End  :%ld/%ld",
			imove, nmoves
			);

	/* if redo-stack, show its count */
	if ( 0 != nredo )
	{
		my_gotoxy(
			tui->layout.iobar2.x + nchars,
			tui->layout.iobar2.y
			);
		CONOUT_PRINTF(
			hc->fg,
			hc->bg,
			"+%ld",
			nredo
			);
	}
}

/* --------------------------------------------------------------
 * void tui_draw_iobar2_savereplayname():
 *
 * Draw on the console screen the io-bar2 of the specified tui object,
 * containing the filename to save current replay into.
 *
 * NOTES: Used in replay-mode.
 *
 *        Read the comments of the function: tui_draw_titlebar()
 *        for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar2_savereplayname( const Tui *tui, const char *fname )
{
	const ConColors *cc = NULL;

	if ( NULL == tui || NULL == fname ) {
		DBGF( "%s", "NULL pointer argumen" );
		return;
	}

	cc = tui_skin_get_colors_iobar2( tui->skin );

	_clear_iobar2( tui );
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar2.x,
		tui->layout.iobar2.y,
		"As: %s",
		fname
		);
}

/* --------------------------------------------------------------
 * void tui_draw_iobar2_movescounter():
 *
 * Draw on the console screen the io-bar-2 of the specified tui
 * object, containing the moves counter.
 *
 * NOTES: Used when game is over. Normally the moves-counter is
 *        shown in iobar, but when game is over the iobar shows
 *        the gameover msg/prompt.
 *
 *        Read the comments of the function: tui_draw_titlebar()
 *        for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar2_movescounter( const Tui *tui )
{
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	cc = tui_skin_get_colors_iobar2( tui->skin );

	_clear_iobar2( tui );
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar2.x,
		tui->layout.iobar2.y,
		"Moves: %ld",
		mvhist_peek_undo_stack_count( tui->mvhist )
		);
}

/* --------------------------------------------------------------
 * void tui_draw_iobar2_mainmenu():
 *
 * Draw on the console screen the io-bar-2 of the specified tui
 * object, containing the moves counter.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar2_mainmenu( const Tui *tui )
{
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	cc = tui_skin_get_colors_iobar2( tui->skin );

	_clear_iobar2( tui );
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar2.x,
		tui->layout.iobar2.y,
		"%s",
		"Main Menu"
		);
}

/* --------------------------------------------------------------
 * void tui_draw_iobar_autoreplayinfo():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * containing replay information.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar_autoreplayinfo( const Tui *tui )
{
	const ConColors *cc = NULL;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );

	_clear_iobar( tui );
	_printfxy(
		cc->fg,
		cc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"Moves delay: %.2f secs    :%s|%s",
		mvhist_get_replay_delay( tui->mvhist ) / 1000.f,
		gamestate_get_prevmove_label( tui->state ),
		gamestate_get_nextmove_label( tui->state )
		);
}

/* --------------------------------------------------------------
 * void tui_draw_iobar_movescounter():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * containing the moves-counter.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar_movescounter( const Tui *tui )
{
	const ConColors *cc = NULL;    /* iobar colors */
	const ConColors *hc = NULL;    /* help-box colors */
	long int nredo = 0, nchars = 0;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	cc = tui_skin_get_colors_iobar( tui->skin );
	hc = tui_skin_get_colors_help_box( tui->skin );
	nredo  = mvhist_peek_redo_stack_count( tui->mvhist );

	_clear_iobar( tui );
	nchars = _printfxy(
			cc->fg,
			cc->bg,
			tui->layout.iobar.x,
			tui->layout.iobar.y,
			"Moves: %ld",
			mvhist_peek_undo_stack_count( tui->mvhist )
			);

	/* if redo-stack, show its count */
	if ( 0 != nredo )
	{
		my_gotoxy(
			tui->layout.iobar.x + nchars,
			tui->layout.iobar.y
			);
		CONOUT_PRINTF(
			hc->fg,
			hc->bg,
			"+%ld",
			nredo
			);
	}
}

/* --------------------------------------------------------------
 * void tui_draw_iobar_savingreplay():
 *
 * Draw on the console screen the io-bar of the specified tui object,
 * containing a msg while saving a replay-file.
 *
 * NOTE: Read the comments of the function: tui_draw_titlebar()
 *       for details about the primitiveness of the implementation.
 * --------------------------------------------------------------
 */
void tui_draw_iobar_savingreplay( const Tui *tui )
{
	const ConColors *sc = NULL;    /* screen colors */

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)" );
		return;
	}

	sc = tui_skin_get_colors_screen( tui->skin );

	_clear_iobar( tui );
	_printfxy(
		sc->fg,
		sc->bg,
		tui->layout.iobar.x,
		tui->layout.iobar.y,
		"Saving, please wait..."
		);
}

/* --------------------------------------------------------------
 * void tui_prompt_replay_fname_to_load():
 *
 * Ask user for a replay-file to load, using the colors of the iobar
 * if the specified tui object. The typed-in filename, is stored in
 * the fname argument. 
 *
 * NOTES: Read the comments of the function: tui_draw_titlebar()
 *        for details about the primitiveness of the implementation.
 *
 *        Contrary to similar functions, this one is NOT using a
 *        screen-layout entity. Instead, it performs the required
 *        i/o right below the main screen of the game.
 * --------------------------------------------------------------
 */
void tui_prompt_replay_fname_to_load( const Tui *tui, char *fname )
{
	const char *prompt = "Type the name of the replay-file to load: ";
	int y;                          /* current cursor y position */

	if ( NULL == tui || NULL == fname ) {
		DBGF( "%s", "NULL pointer argument" );
		return;
	}

	tui_sys_cursor_on();

	puts( "\n" );
	system( LS_REPLAYS );
	putchar( '\n' );

	/* remember cursor's y-position */
	y = my_gety();

	/* clear the line a cursor's y-position */
	CONOUT_PAINT_NTIMES( BG_DEFAULT, my_console_width() );

	/* display the actual prompt */
	my_gotoxy( 0,y );
	_put_hspan_centered(
		FG_DEFAULT,
		BG_DEFAULT,
		prompt,
		strlen( prompt )
		);

	_get_replay_fname_from_user( fname );
	tui_sys_cursor_off();

	return;
}

/* --------------------------------------------------------------
 * int tui_draw_board():
 *
 * Draw on the console screen the board which is referenced
 * internally by the specified tui object. Return 0 (false)
 * on error, 1 (true) otherwise.
 * --------------------------------------------------------------
 */
int tui_draw_board( const Tui *tui )
{
	const Board *board = NULL;/* tui's internal board reference */ 
	int i,j;                  /* for indexing the board tiles */
	int y,x;                  /* top-left tile pos for looping over tiles*/
	int wtile, htile;         /* single-tile drawing width & height */
	int dim;

	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}

	board = gamestate_get_board( tui->state );
	wtile = tui->layout.tile.w;
	htile = tui->layout.tile.h;
	dim = board_get_dim( board );

	y = tui->layout.board.y;
	for (i=0; i < dim; i++, y += htile )
	{
		x = tui->layout.board.x;
		for (j=0; j < dim; j++, x += wtile)
		{
			_draw_tileval_at_xy(
				tui,
				board_get_tile_value( board, i,j ),
				x, y
				);
		}
	}
	return 1;
}

/* --------------------------------------------------------------
 * void tui_redraw():
 *
 * Redraw all the screen entities of the specified tui object.
 * --------------------------------------------------------------
 */
void tui_redraw( const Tui *tui, int isenabledcommands )
{
	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument (tui)!" );
		return;
	}

	tui_draw_titlebar( tui );
	tui_draw_help( tui, isenabledcommands );
	tui_draw_board( tui );
	tui_draw_scoresbar( tui );
	tui_draw_infobar_boardinfo( tui );
	tui_draw_iobar2_mainmenu( tui );
	tui_draw_iobar_movescounter( tui );
//	_clear_iobar2( tui );
//	_clear_iobar( tui );
}

/* --------------------------------------------------------------
 * int tui_cycle_skin():
 *
 * Apply to the specified tui object, the next available skin.
 * --------------------------------------------------------------
 */
int tui_cycle_skin( Tui *tui )
{
	if ( NULL == tui ) {
		DBGF( "%s", "NULL pointer argument!" );
		return 0;
	}

	tui_skin_cycle( tui->skin );

	return 1;
}
