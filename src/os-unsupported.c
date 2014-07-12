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

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_on( void )
{
	return 0;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_off( void )
{
	return 0;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_on( void )
{
	return 0;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_off( void )
{
	return 0;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
int my_getch( unsigned int *outKeyMask )
{
	int key;

	*outKeyMask = MY_KEYMASK_RESET;

	key = getchar();

	return key;
}

/* --------------------------------------------------------------
 * Cross-platform sleeping function (in milliseconds).
 * --------------------------------------------------------------
 */
int my_sleep_msecs( unsigned long int msecs )
{
	for (unsigned int i=0; i < msecs * 1000; i++)
		; /* void */
	return 1;
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
	for (int i=0; i < 24; i++)
		putchar( '\n' );

	return 1;
}

/* --------------------------------------------------------------
 * Return the count of console's visible columns, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_width( void )
{
	return 80;
}

/* --------------------------------------------------------------
 * Return the count of console's visible rows, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_height( void )
{
	return 24;
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
	return 0;
}

/* --------------------------------------------------------------
 * Get cursor's current x & y coords.
 * --------------------------------------------------------------
 */
int my_getxy( int *x, int *y )
{
	return 0;
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
	return 0;
}
