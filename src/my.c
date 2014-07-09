/****************************************************************
 * This file is part of the "2048 Console Clone" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a2
 * Date:         July 9, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: my.h
 * --------------------------------------------------------------
 *
 ****************************************************************
 */

#define MY_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "my.h"

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_on( void )
{
#if defined( MY_OS_WINDOWS )
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	if ( INVALID_HANDLE_VALUE == hConsole) {
		return 0;
	}
	if ( !GetConsoleMode(hConsole, &mode) ) {
		return 0;
	}
	if ( !SetConsoleMode(hConsole, mode & ~(ENABLE_LINE_INPUT)) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
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

#else	/* on Unsupported Platforms */
	return 0;
#endif

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_off( void )
{
#if defined( MY_OS_WINDOWS )
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	if ( INVALID_HANDLE_VALUE == hConsole) {
		return 0;
	}
	if ( !GetConsoleMode(hConsole, &mode) ) {
		return 0;
	}
	if ( !SetConsoleMode(hConsole, mode | ENABLE_LINE_INPUT) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
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

#else	/* on Unsupported Platforms */
	return 0;
#endif

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_on( void )
{
#if defined( MY_OS_WINDOWS )
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	if ( INVALID_HANDLE_VALUE == hConsole) {
		return 0;
	}
	if ( !GetConsoleMode(hConsole, &mode) ) {
		return 0;
	}
	if ( !SetConsoleMode(hConsole, mode | ENABLE_ECHO_INPUT) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	struct termios term;

	if ( 0 != tcgetattr(STDOUT_FILENO, &term) ) {
		return 0;
	}

	term.c_lflag |= ECHO;

	if ( 0 != tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term) ) {
/*	if ( 0 != tcsetattr(STDOUT_FILENO, TCSANOW, &term) ) {*/
		return 0;
	}

#else	/* on Unsupported Platforms */
	return 0;
#endif

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_off( void )
{
#if defined( MY_OS_WINDOWS )
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	if ( INVALID_HANDLE_VALUE == hStdin) {
		return 0;
	}
	if ( !GetConsoleMode(hStdin, &mode) ) {
		return 0;
	}
	if ( !SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT)) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	struct termios term;

	if ( 0 != tcgetattr(STDOUT_FILENO, &term) ) {
		return 0;
	}

	term.c_lflag &= ~ECHO;

	if ( 0 != tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term) ) {
/*	if ( 0 != tcsetattr(STDOUT_FILENO, TCSANOW, &term) ) {*/
		return 0;
	}

#else	/* on Unsupported Platforms */
	return 0;
#endif

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

#if defined( MY_OS_WINDOWS )
	/* on Windows platforms */

	key = getch();
	if ( 0 == key ) /* FunctionKey*/
	{
		*outKeyMask |= MY_KEYMASK_FKEY;
		key = getch();
	}
	else if ( 224 == key /* ArrowKey */
	){
		*outKeyMask |= MY_KEYMASK_ARROW;
		key = getch();
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	/* on Linux & Unix platforms */

	#define MY_MAGIC_MAX_CHARS 18

	struct termios	oldt, newt;
	unsigned char	keycodes[ MY_MAGIC_MAX_CHARS ] = {'\0'};
	int		count;

	/* get the terminal settings for stdin */
	tcgetattr( STDIN_FILENO, &oldt );

	/* we want to keep the old setting to restore them at the end */
	newt = oldt;

	/* ? */
	newt.c_cc[ VMIN  ] = MY_MAGIC_MAX_CHARS;
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

#else 	/* On Unsupported Platforms, fall-back to bufferd getchar() */
	key = getchar();
#endif

	return key;
}

/* --------------------------------------------------------------
 * Cross-platform sleeping function (in milliseconds).
 * --------------------------------------------------------------
 */
int my_sleep_msecs( unsigned long int msecs )
{
#if defined( MY_OS_WINDOWS )
	SleepEx( msecs, FALSE );
	return 1;

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
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

#else	/* on Unsupported Platforms */
	for (unsigned int i=0; i < msecs * 1000; i++)
		; /* void */
	return 1;

#endif
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
#if defined( MY_OS_WINDOWS )

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = {0, 0};	/* home for the cursor */
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	if ( INVALID_HANDLE_VALUE == hConsole) {
		return 0;
	}

	/* get the number of character cells in the current buffer */
	if ( !GetConsoleScreenBufferInfo(hConsole, &csbi) ) {
		return 0;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	/* fill the entire screen with blanks */
	if ( !FillConsoleOutputCharacter(
		hConsole,		/* Handle to console screen buffer */
		(TCHAR) ' ',		/* Character to write to the buffer */
		dwConSize,		/* Number of cells to write */
		coordScreen,		/* Coordinates of first cell */
		&cCharsWritten )	/* Receive number of characters written */
	){
		return 0;
	}

	/* get the current text attribute */
	if ( !GetConsoleScreenBufferInfo(hConsole, &csbi) ) {
		return 0;
	}

	/* set the buffer's attributes accordingly */
	if ( !FillConsoleOutputAttribute(
		hConsole,		/* Handle to console screen buffer */
		csbi.wAttributes,	/* Character attributes to use */
		dwConSize,		/* Number of cells to set attribute */
		coordScreen,		/* Coordinates of first cell */
		&cCharsWritten )	/* Receive number of characters written */
	){
		return 0;
	}

	/* put the cursor at its home coordinates */
	if ( !SetConsoleCursorPosition(hConsole, coordScreen) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	printf( "%s%s", "\033[2J", "\033[H" );
	fflush( stdout );

#else	/* On Unsupported Platforms */
	for (int i=0; i < 24; i++)
		putchar( '\n' );
#endif

	return 1;
}

/* --------------------------------------------------------------
 * Return the count of console's visible columns, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_width( void )
{
#if defined( MY_OS_WINDOWS )
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	if ( INVALID_HANDLE_VALUE == hConsole) {
		return 0;
	}
	if ( !GetConsoleScreenBufferInfo( hConsole, &csbiInfo ) ) {
		return 0;
	}
	return (int) (csbiInfo.srWindow.Right - csbiInfo.srWindow.Left + 1);
/*	return (int) csbiInfo.dwSize.X;*/

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	struct winsize ws;
	(void) ioctl(0, TIOCGWINSZ, &ws);

	return (int) ws.ws_col;

#else	/* on Unsupported Platforms */
	return 80;

#endif
}

/* --------------------------------------------------------------
 * Return the count of console's visible rows, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_height( void )
{
#if defined( MY_OS_WINDOWS )
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	if ( INVALID_HANDLE_VALUE == hConsole) {
		return 0;
	}
	if ( !GetConsoleScreenBufferInfo( hConsole, &csbiInfo ) ) {
		return 0;
	}
	return (int) (csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top + 1);
/*	return (int) csbiInfo.dwSize.Y;*/

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	struct winsize ws;
	(void) ioctl(0, TIOCGWINSZ, &ws);

	return (int) ws.ws_row;

#else	/* on Unsupported Platforms */
	return 24;

#endif
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
#if defined( MY_OS_WINDOWS )
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO consoleCursorInfo;

	if ( INVALID_HANDLE_VALUE == hConsole ) {
		return 0;
	}
	if ( !GetConsoleCursorInfo(hConsole, &consoleCursorInfo) ) {
		return 0;
	}
	consoleCursorInfo.bVisible = onoff;
	if ( !SetConsoleCursorInfo(hConsole, &consoleCursorInfo) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	if ( onoff ) {
		printf( "%s", "\033[?25h" );
		fflush( stdout );
	}
	else {
		printf( "%s", "\033[?25l" );
		fflush( stdout );
	}

#else	/* On UnSupported Platforms */
	return 0;
#endif

	return 1;
}

/* --------------------------------------------------------------
 * Get cursor's current x & y coords.
 * --------------------------------------------------------------
 */
int my_getxy( int *x, int *y )
{
#if defined( MY_OS_WINDOWS )
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( INVALID_HANDLE_VALUE == hConsole ) {
		return 0;
	}
	if ( !GetConsoleScreenBufferInfo(hConsole, &csbiInfo) ) {
		return 0;
	}

	*x = csbiInfo.dwCursorPosition.X;
	*y = csbiInfo.dwCursorPosition.Y;

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
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

#else	/* On UnSupported Platforms */

	return 0;
#endif

	return 1;
}

/* --------------------------------------------------------------
 * Get cursor's current x coord.
 * --------------------------------------------------------------
 */
int my_getx( void )
{
	int x,y;
	return my_getxy(&x,&y) ? x : -1;
}

/* --------------------------------------------------------------
 * Get cursor's current y coord.
 * --------------------------------------------------------------
 */
int my_gety( void )
{
	int x,y;
	return	my_getxy(&x,&y) ? y : -1;
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
#if defined( MY_OS_WINDOWS )
	COORD temp;
	temp.X = x;
	temp.Y = y;
	if ( !SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), temp) ) {
		return 0;
	}

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	printf( "%s%d;%dH", "\033[", y+1, x+1 );
	fflush( stdout );

#else	/* On UnSupported Platforms */
	return 0;
#endif

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
int my_printfxy( int x, int y, const char *fmt, ... )
{
	int ret = 0;
	va_list args;

	my_gotoxy(x,y);
	va_start( args, fmt );
	ret = vprintf( fmt, args );
	fflush( stdout );
	va_end( args );

	return ret;
}
