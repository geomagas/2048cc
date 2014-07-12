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

#include "my.h"

/* Determine compilation OS */
#if defined(__linux__) || defined(__linux) || defined(linux)   \
|| defined(__gnu_linux__)
	#include "os-unix.c"

#elif defined(__unix__) || defined(__unix) || defined(unix)      \
|| defined(__CYGWIN__)                                           \
|| ( defined(__APPLE__) && defined(__MACH__) )                   \
|| ( defined(__APPLE__) && defined(__MACH) )
	#include "os-unix.c"

#elif defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__) \
|| defined(__TOS_WIN__)
	#include "os-win.c"

#else
	#include "os-unsupported.c"

#endif

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
