/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 11, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: my.h
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "my.h"

static int _my_raw_on( void )
{
	struct termios term;

	if ( 0 != tcgetattr(STDOUT_FILENO, &term) ) {
		return 0;
	}

	term.c_lflag &= ~(IXOFF);
	term.c_lflag &= ~(ICANON);

/*	if ( 0 != tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term) ) {*/
	if ( 0 != tcsetattr(STDOUT_FILENO, TCSANOW, &term) ) {
		return 0;
	}

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_off( void )
{
	struct termios term;

	if ( 0 != tcgetattr(STDOUT_FILENO, &term) ) {
		return 0;
	}

	term.c_lflag |= IXOFF;
	term.c_lflag |= ICANON;

/*	if ( 0 != tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term) ) {*/
	if ( 0 != tcsetattr(STDOUT_FILENO, TCSANOW, &term) ) {
		return 0;
	}

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_on( void )
{
	struct termios term;

	if ( 0 != tcgetattr(STDOUT_FILENO, &term) ) {
		return 0;
	}

	term.c_lflag |= ECHO;

	if ( 0 != tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term) ) {
/*	if ( 0 != tcsetattr(STDOUT_FILENO, TCSANOW, &term) ) {*/
		return 0;
	}

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_off( void )
{
	struct termios term;

	if ( 0 != tcgetattr(STDOUT_FILENO, &term) ) {
		return 0;
	}

	term.c_lflag &= ~ECHO;

	if ( 0 != tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term) ) {
/*	if ( 0 != tcsetattr(STDOUT_FILENO, TCSANOW, &term) ) {*/
		return 0;
	}

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
int my_getch( unsigned int *outKeyMask )
{
	int key;

	*outKeyMask = MY_KEYMASK_RESET;

	/* on Linux & Unix platforms */

	#define MY_MAGIC_MAX_CHARS 18

	struct termios	oldt, newt;
	unsigned char	keycodes[ MY_MAGIC_MAX_CHARS ] = {'\0'};
	int		count;

	/* get the terminal settings for stdin */
	tcgetattr( STDIN_FILENO, &oldt );

	/* we want to keep the old setting to restore them at the end */
	newt = oldt;

	/* Input must have at least this number of chars for read to return */
	newt.c_cc[ VMIN  ] = MY_MAGIC_MAX_CHARS;
	/* Time in 0.1 seconds after first char to wait before returning */
	newt.c_cc[ VTIME ] = 1;
	newt.c_iflag &= ~(IXOFF);

	/* disable canonical mode (buffered i/o) and local echo */
	newt.c_lflag &= ~(ECHO | ICANON);

	/* set the new settings */
/*	tcsetattr( STDIN_FILENO, TCSAFLUSH, &newt );*/
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );

	count = read( STDIN_FILENO, keycodes, MY_MAGIC_MAX_CHARS );
	if ( 1 == count )
	{
		key = keycodes[0];
		if ( '\r' == key || '\n' == key ) {
			key = MY_KEY_ENTER;
		}
		else if ( '\b' == key || 127 == key ) {
			key = MY_KEY_BACKSPACE;
		}
	}
	else
	{
		if ( '\r' == keycodes[0] || '\n' == keycodes[0] ) {
			key = MY_KEY_ENTER;
		}

		/* arrow keys */
		else if (0 == memcmp(&keycodes[1], "[A", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_UP;
		}
		else if (0 == memcmp(&keycodes[1], "[B", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_DOWN;
		}
		else if (0 == memcmp(&keycodes[1], "[C", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_RIGHT;
		}
		else if (0 == memcmp(&keycodes[1], "[D", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_LEFT;
		}

		/* insert & delete keys */
		else if (0 == memcmp(&keycodes[1], "[2~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_INSERT;
		}
		else if (0 == memcmp(&keycodes[1], "[3~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_DELETE;
		}

		/* home & end keys */
		else if (0 == memcmp(&keycodes[1], "OH", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_HOME;
		}
		else if (0 == memcmp(&keycodes[1], "OF", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_END;
		}

		/* page-up & page-down keys */
		else if (0 == memcmp(&keycodes[1], "[5~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_PAGE_UP;
		}
		else if (0 == memcmp(&keycodes[1], "[6~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_ARROW;
			key = MY_KEY_PAGE_DOWN;
		}

		/* FKEYS */
		else if (0 == memcmp(&keycodes[1], "OP", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F1;
		}
		else if (0 == memcmp(&keycodes[1], "OQ", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F2;
		}
		else if (0 == memcmp(&keycodes[1], "OR", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F3;
		}
		else if (0 == memcmp(&keycodes[1], "OS", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F4;
		}
		else if (0 == memcmp(&keycodes[1], "[15~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F5;
		}
		else if (0 == memcmp(&keycodes[1], "[17~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F6;
		}
		else if (0 == memcmp(&keycodes[1], "[18~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F7;
		}
		else if (0 == memcmp(&keycodes[1], "[19~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F8;
		}
		else if (0 == memcmp(&keycodes[1], "[20~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F9;
		}
		else if (0 == memcmp(&keycodes[1], "[24~", count-1) ) {
			*outKeyMask |= MY_KEYMASK_FKEY;
			key = MY_KEY_F12;
		}

		/* other multi-character ANSI escape sequences */
		else {
			*outKeyMask |= MY_KEYMASK_UNKNOWN;
			key = -(int)keycodes[count-1];
		}
	}

	/* restore the former settings */
	tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

	return key;
}

/* --------------------------------------------------------------
 * Cross-platform sleeping function (in milliseconds).
 * --------------------------------------------------------------
 */
int my_sleep_msecs( unsigned long int msecs )
{
	unsigned long int usecs = msecs * 1000;
	const unsigned long int maxusecs = 1000000;
	unsigned long int ntimes = usecs / maxusecs;
	while ( ntimes-- > 0 ) {
		if ( -1 == usleep(usecs) ) {
			return 0;  /* false */
		}
	}
	usecs %= maxusecs;
	return usecs && 0 == usleep(usecs);
}

/* -----------------------------------------------------
 * Cross-platform function to clear the standard output.
 *
 * It works both on the Windows console and on terminals that
 * support ANSI escape sequences (that is on most Unix/Linux
 * terminals).
 * --------------------------------------------------------------
 */
int my_cls( void )
{
	printf( "%s%s", "\033[2J", "\033[H" );
	fflush( stdout );

	return 1;
}

/* --------------------------------------------------------------
 * Return the count of console's visible columns, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_width( void )
{
	struct winsize ws;
	(void) ioctl(0, TIOCGWINSZ, &ws);

	return (int) ws.ws_col;
}

/* --------------------------------------------------------------
 * Return the count of console's visible rows, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_height( void )
{
	struct winsize ws;
	(void) ioctl(0, TIOCGWINSZ, &ws);

	return (int) ws.ws_row;
}

/* --------------------------------------------------------------
 * Cross-platform function to show or hide the console cursor.
 *
 * It works both on the Windows console and on terminals that
 * support ANSI escape sequences (that is on most Unix/Linux
 * terminals).
 * --------------------------------------------------------------
 */
int my_cursor_onoff( int onoff )
{
	if ( onoff ) {
		printf( "%s", "\033[?25h" );
		fflush( stdout );
	}
	else {
		printf( "%s", "\033[?25l" );
		fflush( stdout );
	}

	return 1;
}

/* --------------------------------------------------------------
 * Get cursor's current x & y coords.
 * --------------------------------------------------------------
 */
int my_getxy( int *x, int *y )
{
	char keycodes[18+1] = {'\0'};
	int  tempX, tempY;

	_my_echo_off();
	_my_raw_on();

	/* query cursor coords */
	printf( "%s", "\033[6n" );
	fflush( stdout );

	read( STDIN_FILENO, keycodes, 18 );

	keycodes[18] = '\0';
	if ( 2 != sscanf(keycodes, "\033[%d;%dR", &tempY, &tempX) ) {
		return 0;
	}
	*x = tempX-1;
	*y = tempY-1;

	_my_raw_off();
	_my_echo_on();

	return 1;
}

/* --------------------------------------------------------------
 * Cross-platform function to move arbitrarily the console cursor.
 *
 * It works both on the Windows console and on terminals that
 * support ANSI escape sequences (that is on most Unix/Linux
 * terminals).
 * --------------------------------------------------------------
 */
int my_gotoxy( int x, int y )
{
	printf( "%s%d;%dH", "\033[", y+1, x+1 );
	fflush( stdout );

	return 1;
}
