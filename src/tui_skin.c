/****************************************************************
 * This file is part of the "2048 Console Clone" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a1
 * Date:         July 8, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: con_color.h, common.h
 * --------------------------------------------------------------
 *
 * Private implementation of colorized skins for the text user
 * interface used in the game.
 *
 * Skin Entities
 * -------------
 *
 * A skin has a unique id ( _SKIN_ID_XXX) and consists of several
 * colorized entities, which are of type ConColors (this type wraps
 * a foreground and a background color, and it is available through
 * con_color.h).
 *
 * Most of these entities, are conceptual counterparts of the
 * layout-entities defined in the file: tui.c, but there is no
 * direct interconnection among them (i.e. some layout-entities
 * may not be skinnable, and vice versa).
 *
 * All the entities (along with the skin id) are bundled in the
 * _tuiskin struct, which is exposed publicly as an opaque type,
 * called: TuiSkin.
 *
 * The skins can by manipulated externally only via the public
 * functions of this interface. They expect a TuiSkin pointer
 * as their first argument (except the constructor, of course).
 *
 * Adding a New Skin (say XXX)
 * ---------------------------
 *
 * 1. Add a unique enumerated id _SKIN_ID_XXX just before _MAXSKINS.
 *
 * 2. Write a corresponding _set_xxx() function to colorize the
 *    skin entities (see the function: _set_dark() for an example).
 *
 *    2a. Optionally, define 2 different versions of the function
 *        _set_xxx(), targeting the Windows console and an ANSI
 *       aware terminal (common ground in Unix, Linux and OSX).
 *       Those are already abstracted by con_color.h, so once
 *       again, see the function: _set_dark() for an example
 *       using conditional compilation directives.
 *
 * 3. Add the _set_xxx() function along with its _SKIN_ID_XXX
 *    enumerated id, to the switch-statement of the function:
 *    _set()
 *
 * That's it! From now on, the public function: tui_skin_cycle()
 * will be aware of the new xxx skin.
 *****************************************************************
 */

#define TUI_SKIN_C

#include <stdlib.h>       /* calloc(), free() */

#include "con_color.h"
#include "common.h"
#include "tui_skin.h"

/*
 * Given an enumerated skin id, return its next one 
 * (cycle over).
 */
#define _NEXT_SKIN_ID(id)                             \
(                                                     \
	(id) <= _SKIN_ID_NONE || (id) >= _MAXSKINS-1  \
		? 1                                   \
		: ++(id)                              \
)

/*
 * Enumerated skin id's.
 */
enum {
	_SKIN_ID_NONE  = 0,
	_SKIN_ID_DARK,
	_SKIN_ID_LIGHT,
	_MAXSKINS
};

/*
 * Publicly opaque skin data-type.
 */
struct _tuiskin {
	int id;

	ConColors screen;

	/* left half of the screen*/
	ConColors titlebar;
	ConColors tile0,    tile2,    tile4,    tile8,
	          tile16,   tile32,   tile64,
	          tile128,  tile256,  tile512,
	          tile1024, tile2048, tilebig;
	ConColors scoresbar;
	ConColors infobar;

	/* right half of the screen*/
	struct {
		ConColors box;
		ConColors header;
		ConColors footer;
		ConColors commands;
		ConColors commands_disabled;
	} help;
	ConColors iobar2;
	ConColors iobar;
};

#if CON_COLORMODE_WIN32 == CONSYS_GET_COLORMODE()

/* --------------------------------------------------------------
 * WIN32 DARK SKIN
 * --------------------------------------------------------------
 */
static void _set_dark( TuiSkin *skin )
{
	/* just for brevity later on (screen fg, bg) */
	const ConColors *s;

	skin->id = _SKIN_ID_DARK;

	/* console screen */
	CONOUT_CPYCLR( skin->screen.fg, FG_GRAY );
	CONOUT_CPYCLR( skin->screen.bg, BG_BLACK );

	s = &skin->screen;

	/*
	 * Left half of the screen.
	 */

	/* title-bar */
	CONOUT_CPYCLR( skin->titlebar.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->titlebar.bg, BG_WHITE );

	/* empty-tile */
	CONOUT_CPYCLR( skin->tile0.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->tile0.bg, s->bg );

	/* tile-2 */
	CONOUT_CPYCLR( skin->tile2.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile2.bg, BG_WHITE );

	/* tile-4 */
	CONOUT_CPYCLR( skin->tile4.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile4.bg, BG_GRAY );

	/* tile-8 */
	CONOUT_CPYCLR( skin->tile8.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile8.bg, BG_DARKMAGENTA );

	/* tile-16 */
	CONOUT_CPYCLR( skin->tile16.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile16.bg, BG_MAGENTA );

	/* tile-32 */
	CONOUT_CPYCLR( skin->tile32.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile32.bg, BG_DARKRED );

	/* tile-64 */
	CONOUT_CPYCLR( skin->tile64.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile64.bg, BG_RED );

	/* tile-128 */
	CONOUT_CPYCLR( skin->tile128.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile128.bg, BG_DARKGREEN );

	/* tile-256 */
	CONOUT_CPYCLR( skin->tile256.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile256.bg, BG_GREEN );

	/* tile-512 */
	CONOUT_CPYCLR( skin->tile512.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile512.bg, BG_DARKCYAN );

	/* tile-1024 */
	CONOUT_CPYCLR( skin->tile1024.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile1024.bg, BG_CYAN );

	/* tile-2048 */
	CONOUT_CPYCLR( skin->tile2048.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile2048.bg, BG_YELLOW );

	/* tile-big */
	CONOUT_CPYCLR( skin->tilebig.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tilebig.bg, BG_YELLOW );

	/* scores-bar */
	CONOUT_CPYCLR( skin->scoresbar.fg, s->fg );
	CONOUT_CPYCLR( skin->scoresbar.bg, s->bg );

	/* info-bar */
	CONOUT_CPYCLR( skin->infobar.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->infobar.bg, s->bg );

	/*
	 * Right half of the screen.
	 */

	/* help-box */
	CONOUT_CPYCLR( skin->help.box.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->help.box.bg, s->bg );

	/* help-box header */
	CONOUT_CPYCLR( skin->help.header.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->help.header.bg, BG_DARKGRAY );

	/* help-box commands */
	CONOUT_CPYCLR( skin->help.commands.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->help.commands.bg, s->bg );

	/* help-box disabled commands */
	CONOUT_CPYCLR( skin->help.commands_disabled.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->help.commands_disabled.bg, s->bg );

	/* help-box footer */
	CONOUT_CPYCLR( skin->help.footer.fg, skin->help.header.fg );
	CONOUT_CPYCLR( skin->help.footer.bg, skin->help.header.bg );

	/* iobar2 */
	CONOUT_CPYCLR( skin->iobar2.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->iobar2.bg, s->bg );

	/* io-bar */
	CONOUT_CPYCLR( skin->iobar.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->iobar.bg, s->bg );
}

/* --------------------------------------------------------------
 * WIN32 LIGHT SKIN
 * --------------------------------------------------------------
 */
static void _set_light( TuiSkin *skin )
{
	/* just for brevity later on (screen fg, bg) */
	const ConColors *s;

	skin->id = _SKIN_ID_LIGHT;

	/* console screen */
	CONOUT_CPYCLR( skin->screen.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->screen.bg, BG_GRAY );

	s = &skin->screen;

	/*
	 * Left half of the screen.
	 */

	/* title-bar */
	CONOUT_CPYCLR( skin->titlebar.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->titlebar.bg, BG_BLACK );

	/* empty-tile */
	CONOUT_CPYCLR( skin->tile0.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->tile0.bg, s->bg );

	/* tile-2 */
	CONOUT_CPYCLR( skin->tile2.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile2.bg, BG_WHITE );

	/* tile-4 */
	CONOUT_CPYCLR( skin->tile4.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile4.bg, BG_DARKGRAY );

	/* tile-8 */
	CONOUT_CPYCLR( skin->tile8.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile8.bg, BG_DARKMAGENTA );

	/* tile-16 */
	CONOUT_CPYCLR( skin->tile16.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile16.bg, BG_MAGENTA );

	/* tile-32 */
	CONOUT_CPYCLR( skin->tile32.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile32.bg, BG_DARKRED );

	/* tile-64 */
	CONOUT_CPYCLR( skin->tile64.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile64.bg, BG_RED );

	/* tile-128 */
	CONOUT_CPYCLR( skin->tile128.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile128.bg, BG_DARKGREEN );

	/* tile-256 */
	CONOUT_CPYCLR( skin->tile256.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile256.bg, BG_GREEN );

	/* tile-512 */
	CONOUT_CPYCLR( skin->tile512.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile512.bg, BG_DARKCYAN );

	/* tile-1024 */
	CONOUT_CPYCLR( skin->tile1024.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile1024.bg, BG_CYAN );

	/* tile-2048 */
	CONOUT_CPYCLR( skin->tile2048.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile2048.bg, BG_YELLOW );

	/* tile-big */
	CONOUT_CPYCLR( skin->tilebig.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tilebig.bg, BG_YELLOW );

	/* scores-bar */
	CONOUT_CPYCLR( skin->scoresbar.fg, s->fg );
	CONOUT_CPYCLR( skin->scoresbar.bg, s->bg );

	/* info-bar */
	CONOUT_CPYCLR( skin->infobar.fg, FG_BLUE );
	CONOUT_CPYCLR( skin->infobar.bg, s->bg );

	/*
	 * Right half of the screen.
	 */

	/* help-box */
	CONOUT_CPYCLR( skin->help.box.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->help.box.bg, s->bg );

	/* help-box header */
	CONOUT_CPYCLR( skin->help.header.fg, FG_GRAY );
	CONOUT_CPYCLR( skin->help.header.bg, BG_DARKGRAY );

	/* help-box commands */
	CONOUT_CPYCLR( skin->help.commands.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->help.commands.bg, s->bg );

	/* help-box disabled commands */
	CONOUT_CPYCLR( skin->help.commands_disabled.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->help.commands_disabled.bg, s->bg );

	/* help-box footer */
	CONOUT_CPYCLR( skin->help.footer.fg, skin->help.header.fg );
	CONOUT_CPYCLR( skin->help.footer.bg, skin->help.header.bg );

	/* iobar2 */
	CONOUT_CPYCLR( skin->iobar2.fg, FG_BLUE );
	CONOUT_CPYCLR( skin->iobar2.bg, s->bg );

	/* io-bar */
	CONOUT_CPYCLR( skin->iobar.fg, FG_BLUE );
	CONOUT_CPYCLR( skin->iobar.bg, s->bg );
}

#else  /* for ANSI aware terinals */

/* --------------------------------------------------------------
 * ANSI DARK SKIN
 * --------------------------------------------------------------
 */
static void _set_dark( TuiSkin *skin )
{
	/* just for brevity later on (screen fg, bg) */
	const ConColors *s;

	skin->id = _SKIN_ID_DARK;

	/* console screen */
	CONOUT_CPYCLR( skin->screen.fg, FG_GRAY );
	CONOUT_CPYCLR( skin->screen.bg, BG_BLACK );

	s = &skin->screen;

	/*
	 * Left half of the screen.
	 */

	/* title-bar */
	CONOUT_CPYCLR( skin->titlebar.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->titlebar.bg, BG_DARKBLUE );

	/* empty-tile */
	CONOUT_CPYCLR( skin->tile0.fg, FG_GRAY );
	CONOUT_CPYCLR( skin->tile0.bg, s->bg );

	/* tile-2 */
	CONOUT_CPYCLR( skin->tile2.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile2.bg, BG_GRAY );

	/* tile-4 */
	CONOUT_CPYCLR( skin->tile4.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile4.bg, BG_DARKCYAN );

	/* tile-8 */
	CONOUT_CPYCLR( skin->tile8.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile8.bg, BG_DARKGREEN );

	/* tile-16 */
	CONOUT_CPYCLR( skin->tile16.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile16.bg, BG_DARKMAGENTA );

	/* tile-32 */
	CONOUT_CPYCLR( skin->tile32.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile32.bg, BG_DARKRED );

	/* tile-64 */
	CONOUT_CPYCLR( skin->tile64.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile64.bg, BG_DARKRED );

	/* tile-128 */
	CONOUT_CPYCLR( skin->tile128.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->tile128.bg, BG_DARKBLUE );

	/* tile-256 */
	CONOUT_CPYCLR( skin->tile256.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile256.bg, BG_DARKBLUE );

	/* tile-512 */
	CONOUT_CPYCLR( skin->tile512.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile512.bg, BG_DARKCYAN );

	/* tile-1024 */
	CONOUT_CPYCLR( skin->tile1024.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile1024.bg, BG_DARKYELLOW );

	/* tile-2048 */
	CONOUT_CPYCLR( skin->tile2048.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile2048.bg, BG_DARKYELLOW );

	/* tile-big */
	CONOUT_CPYCLR( skin->tilebig.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tilebig.bg, BG_DARKMAGENTA );

	/* scores-bar */
	CONOUT_CPYCLR( skin->scoresbar.fg, s->fg );
	CONOUT_CPYCLR( skin->scoresbar.bg, s->bg );

	/* info-bar */
	CONOUT_CPYCLR( skin->infobar.fg, FG_GREEN );
	CONOUT_CPYCLR( skin->infobar.bg, s->bg );

	/*
	 * Right half of the screen.
	 */

	/* help-box */
	CONOUT_CPYCLR( skin->help.box.fg, s->fg );
	CONOUT_CPYCLR( skin->help.box.bg, s->bg );

	/* help-box header */
	CONOUT_CPYCLR( skin->help.header.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->help.header.bg, BG_GRAY );

	/* help-box commands */
	CONOUT_CPYCLR( skin->help.commands.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->help.commands.bg, s->bg );

	/* help-box disabled commands */
	CONOUT_CPYCLR( skin->help.commands_disabled.fg, s->fg );
	CONOUT_CPYCLR( skin->help.commands_disabled.bg, s->bg );

	/* help-box footer */
	CONOUT_CPYCLR( skin->help.footer.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->help.footer.bg, BG_GRAY );

	/* iobar2 */
	CONOUT_CPYCLR( skin->iobar2.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->iobar2.bg, s->bg );

	/* io-bar */
	CONOUT_CPYCLR( skin->iobar.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->iobar.bg, s->bg );
}

/* --------------------------------------------------------------
 * ANSI LIGHT SKIN
 * --------------------------------------------------------------
 */
static void _set_light( TuiSkin *skin )
{
	/* just for brevity later on (screen fg, bg) */
	const ConColors *s;

	skin->id = _SKIN_ID_LIGHT;

	/* console screen */
	CONOUT_CPYCLR( skin->screen.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->screen.bg, BG_GRAY );

	s = &skin->screen;

	/*
	 * Left half of the screen.
	 */

	/* title-bar */
	CONOUT_CPYCLR( skin->titlebar.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->titlebar.bg, BG_DARKBLUE );

	/* empty-tile */
	CONOUT_CPYCLR( skin->tile0.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile0.bg, s->bg );

	/* tile-2 */
	CONOUT_CPYCLR( skin->tile2.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->tile2.bg, BG_DARKCYAN );

	/* tile-4 */
	CONOUT_CPYCLR( skin->tile4.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile4.bg, BG_DARKCYAN );

	/* tile-8 */
	CONOUT_CPYCLR( skin->tile8.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile8.bg, BG_DARKGREEN );

	/* tile-16 */
	CONOUT_CPYCLR( skin->tile16.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile16.bg, BG_DARKMAGENTA );

	/* tile-32 */
	CONOUT_CPYCLR( skin->tile32.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile32.bg, BG_DARKRED );

	/* tile-64 */
	CONOUT_CPYCLR( skin->tile64.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile64.bg, BG_DARKRED );

	/* tile-128 */
	CONOUT_CPYCLR( skin->tile128.fg, FG_CYAN );
	CONOUT_CPYCLR( skin->tile128.bg, BG_DARKBLUE );

	/* tile-256 */
	CONOUT_CPYCLR( skin->tile256.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile256.bg, BG_DARKBLUE );

	/* tile-512 */
	CONOUT_CPYCLR( skin->tile512.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile512.bg, BG_DARKCYAN );

	/* tile-1024 */
	CONOUT_CPYCLR( skin->tile1024.fg, FG_BLACK );
	CONOUT_CPYCLR( skin->tile1024.bg, BG_DARKYELLOW );

	/* tile-2048 */
	CONOUT_CPYCLR( skin->tile2048.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tile2048.bg, BG_DARKYELLOW );

	/* tile-big */
	CONOUT_CPYCLR( skin->tilebig.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->tilebig.bg, BG_DARKMAGENTA );

	/* scores-bar */
	CONOUT_CPYCLR( skin->scoresbar.fg, s->fg );
	CONOUT_CPYCLR( skin->scoresbar.bg, s->bg );

	/* info-bar */
	CONOUT_CPYCLR( skin->infobar.fg, FG_BLUE );
	CONOUT_CPYCLR( skin->infobar.bg, s->bg );

	/*
	 * Right half of the screen.
	 */

	/* help-box */
	CONOUT_CPYCLR( skin->help.box.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->help.box.bg, s->bg );

	/* help-box header */
	CONOUT_CPYCLR( skin->help.header.fg, FG_GRAY );
	CONOUT_CPYCLR( skin->help.header.bg, BG_BLACK );

	/* help-box commands */
	CONOUT_CPYCLR( skin->help.commands.fg, FG_WHITE );
	CONOUT_CPYCLR( skin->help.commands.bg, s->bg );

	/* help-box disabled commands */
	CONOUT_CPYCLR( skin->help.commands_disabled.fg, FG_DARKGRAY );
	CONOUT_CPYCLR( skin->help.commands_disabled.bg, s->bg );

	/* help-box footer */
	CONOUT_CPYCLR( skin->help.footer.fg, FG_GRAY );
	CONOUT_CPYCLR( skin->help.footer.bg, BG_BLACK );

	/* iobar2 */
	CONOUT_CPYCLR( skin->iobar2.fg, FG_BLUE );
	CONOUT_CPYCLR( skin->iobar2.bg, s->bg );

	/* io-bar */
	CONOUT_CPYCLR( skin->iobar.fg, FG_BLUE );
	CONOUT_CPYCLR( skin->iobar.bg, s->bg );
}

#endif

/* --------------------------------------------------------------
 *
 * --------------------------------------------------------------
 */
static void _set( TuiSkin *skin, int id )
{
	switch( id )
	{
		case _SKIN_ID_DARK:
			_set_dark( skin );
			break;

		case _SKIN_ID_LIGHT:
			_set_light( skin );
			break;

		default:
			break;
	}
}

/* --------------------------------------------------------------
 *
 * --------------------------------------------------------------
 */
static void _init( TuiSkin *skin )
{
	_set_dark( skin );
}

/* --------------------------------------------------------------
 * TuiSkin *new_tui_skin():
 *
 * The tui_skin constructor creates a new TuiSkin object in memory,
 * initializes it to default values and returns a pointer to it.
 * On error, it returns NULL.
 * --------------------------------------------------------------
 */
TuiSkin *new_tui_skin( void )
{
	TuiSkin *skin = calloc(1, sizeof(*skin) );
	if ( NULL == skin ) {
		DBGF( "%s", "calloc() failed!" );
		return NULL;
	}

	_init( skin );

	return skin;
}

/* --------------------------------------------------------------
 * TuiSkin *tui_skin_release():
 *
 * The tui_skin destructor releases the memory reserved for
 * the TuiSkin object pointed to by the specified pointer,
 * and it returns NULL (so the caller may assign it back to
 * the pointer of the released object).
 * --------------------------------------------------------------
 */
TuiSkin *tui_skin_release( TuiSkin *skin )
{
	if ( skin ) {
		free( skin );
	}

	return NULL;
}

/* --------------------------------------------------------------
 * int tui_skin_cycle():
 *
 * Replace the specified skin with the next skin, and return the
 * replaced id of the skin, or _SKIN_ID_NONE on error.
 *
 * NOTE: First, the function passes the id of the specified skin
 *       to the macro _NEXT_SKIN_ID(), which in turn, returns the
 *       next id according to the SKIN_ID_XXX enumeration (see
 *       its definition at the top of this file). The macro NEVER
 *       returns an invalid id.
 *
 *       Then, the next-id along with the skin, are passed to the
 *       function _set(), which does the actual replacement. If
 *       the next-id is NOT handled in the switch-statement of the
 *       function _set(), then no replacement takes place.
 * --------------------------------------------------------------
 */
int tui_skin_cycle( TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return _SKIN_ID_NONE;
	}

	_set( skin, _NEXT_SKIN_ID(skin->id) );
	return skin->id;
}

/* --------------------------------------------------------------
 * (Getter)
 * const ConColors *tui_skin_get_colors_screen():
 *
 * Return a pointer to the screen colors of the specified skin,
 * or NULL on error.
 * --------------------------------------------------------------
 */
const const ConColors *tui_skin_get_colors_screen( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->screen;
}

/* --------------------------------------------------------------
 * (Getter)
 * --------------------------------------------------------------
 */
const const ConColors *tui_skin_get_colors_titlebar( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->titlebar;
}

/* --------------------------------------------------------------
 * (Getters)
 * --------------------------------------------------------------
 */
const const ConColors *tui_skin_get_colors_tile0( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile0;
}

const const ConColors *tui_skin_get_colors_tile2( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile2;
}

const const ConColors *tui_skin_get_colors_tile4( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile4;
}

const const ConColors *tui_skin_get_colors_tile8( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile8;
}

const const ConColors *tui_skin_get_colors_tile16( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile16;
}

const const ConColors *tui_skin_get_colors_tile32( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile32;
}

const const ConColors *tui_skin_get_colors_tile64( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile64;
}

const const ConColors *tui_skin_get_colors_tile128( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile128;
}

const const ConColors *tui_skin_get_colors_tile256( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile256;
}

const const ConColors *tui_skin_get_colors_tile512( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile512;
}

const ConColors *tui_skin_get_colors_tile1024( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile1024;
}

const ConColors *tui_skin_get_colors_tile2048( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tile2048;
}

const ConColors *tui_skin_get_colors_tilebig( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->tilebig;
}

/* --------------------------------------------------------------
 * (Getter)
 * --------------------------------------------------------------
 */
const ConColors *tui_skin_get_colors_scoresbar( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->scoresbar;
}

/* --------------------------------------------------------------
 * (Getter)
 * --------------------------------------------------------------
 */
const ConColors *tui_skin_get_colors_infobar( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->infobar;
}

/* --------------------------------------------------------------
 * (Getters)
 * --------------------------------------------------------------
 */
const ConColors *tui_skin_get_colors_help_box( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->help.box;
}

const ConColors *tui_skin_get_colors_help_header( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->help.header;
}

const ConColors *tui_skin_get_colors_help_footer( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->help.footer;
}

const ConColors *tui_skin_get_colors_help_commands( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->help.commands;
}

const ConColors *tui_skin_get_colors_help_commands_disabled( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->help.commands_disabled;
}

/* --------------------------------------------------------------
 * (Getter)
 * --------------------------------------------------------------
 */
const ConColors *tui_skin_get_colors_iobar2( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->iobar2;
}

/* --------------------------------------------------------------
 * (Getter)
 * --------------------------------------------------------------
 */
const ConColors *tui_skin_get_colors_iobar( const TuiSkin *skin )
{
	if ( NULL == skin ) {
		DBGF( "%s", "NULL pointer argument (skin)!" );
		return NULL;
	}
	return &skin->iobar;
}
