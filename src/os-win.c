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

#include <conio.h>
#include <windows.h>
#ifdef __POCC__
	#define getch  _getch
#endif

#include "my.h"

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_on( void )
{
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

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_raw_off( void )
{
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

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_on( void )
{
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

	return 1;
}

/* --------------------------------------------------------------
 * 
 * --------------------------------------------------------------
 */
static int _my_echo_off( void )
{
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

	return key;
}

/* --------------------------------------------------------------
 * Cross-platform sleeping function (in milliseconds).
 * --------------------------------------------------------------
 */
int my_sleep_msecs( unsigned long int msecs )
{
	SleepEx( msecs, FALSE );
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

	return 1;
}

/* --------------------------------------------------------------
 * Return the count of console's visible columns, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_width( void )
{
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
}

/* --------------------------------------------------------------
 * Return the count of console's visible rows, or 0 on error.
 * --------------------------------------------------------------
 */
int my_console_height( void )
{
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

	return 1;
}

/* --------------------------------------------------------------
 * Get cursor's current x & y coords.
 * --------------------------------------------------------------
 */
int my_getxy( int *x, int *y )
{
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
	COORD temp;
	temp.X = x;
	temp.Y = y;
	if ( !SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), temp) ) {
		return 0;
	}

	return 1;
}
