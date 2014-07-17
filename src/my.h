/****************************************************************
 * This file is part of the "2048cc" game.
 *
 * Author:       migf1 <mig_f1@hotmail.com>
 * Version:      0.3a3
 * Date:         July 18, 2014
 * License:      Free Software (see comments in main.c for limitations)
 * --------------------------------------------------------------
 *
 * Header file exposing a small collection of cross-platform,
 * utility function and constants, aiming to help implementing
 * rather primitive text user interfaces on the console/terminal.
 ****************************************************************
 */

#ifndef MY_H
#define MY_H

/* Determine compilation OS */
#if defined(__linux__) || defined(__linux) || defined(linux)     \
|| defined(__gnu_linux__)
	#define MY_OS_LINUX

#elif defined(__unix__) || defined(__unix) || defined(unix)      \
|| defined(__CYGWIN__)                                           \
|| ( defined(__APPLE__) && defined(__MACH__) )                   \
|| ( defined(__APPLE__) && defined(__MACH) )
	#define MY_OS_UNIX

#elif defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__) \
|| defined(__TOS_WIN__)
	#define MY_OS_WINDOWS

#else
	#define MY_OS_UNKNOWN

#endif

/* -------------
 * Includes
 * -------------
 */

#if defined( MY_OS_WINDOWS )
	#include <conio.h>
	#include <windows.h>
	#ifdef __POCC__
		#define getch  _getch
	#endif

#elif defined( MY_OS_UNIX ) || defined( MY_OS_LINUX )
	#include <sys/ioctl.h>
	#include <termios.h>
	#include <unistd.h>
#endif

/* ------------------
 * Constants & Macros
 * ------------------
 */

enum {	/* unbuffered i/o related constants & masks */

	MY_KEYMASK_RESET   = 0x00,
	MY_KEYMASK_ARROW   = (1 << 0),	// 0x01
	MY_KEYMASK_FKEY    = (1 << 1),	// 0x02
	MY_KEYMASK_UNKNOWN = (1 << 2),	// 0x04

	MY_KEY_NUL	 = 0,
	MY_KEY_ESCAPE    = 27,
	MY_KEY_ENTER     = 13,
	MY_KEY_SPACE     = 32,

	MY_KEY_UP        = 72,
	MY_KEY_DOWN      = 80,
	MY_KEY_LEFT      = 75,
	MY_KEY_RIGHT     = 77,
	MY_KEY_INSERT    = 82,
	MY_KEY_DELETE    = 83,
	MY_KEY_HOME      = 71,
	MY_KEY_END	 = 79,
	MY_KEY_PAGE_UP   = 73,
	MY_KEY_PAGE_DOWN = 81,

	MY_KEY_BACKSPACE = 8,

	MY_KEY_F1        = 59,
	MY_KEY_F2        = 60,
	MY_KEY_F3        = 61,
	MY_KEY_F4        = 62,
	MY_KEY_F5        = 63,
	MY_KEY_F6        = 64,
	MY_KEY_F7        = 65,
	MY_KEY_F8        = 66,
	MY_KEY_F9        = 67,
	MY_KEY_F10       = 68,
	MY_KEY_F11       = 133,
	MY_KEY_F12       = 134
};

#ifndef MY_C
extern int my_cursor_onoff( int onoff );
extern int my_getch( unsigned int *outKeyMask );
extern int my_sleep_msecs( unsigned long int msecs );
extern int my_cls( void );
extern int my_console_width( void );
extern int my_console_height( void );
extern int my_getxy( int *x, int *y );
extern int my_getx( void );
extern int my_gety( void );
extern int my_gotoxy( int x, int y );
extern int my_printfxy( int x, int y, const char *fmt, ... );
#endif

#endif
