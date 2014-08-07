/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: 
 * --------------------------------------------------------------
 *
 * A small collection of misc constants, macros and functions
 * to be used used across the project.
 ****************************************************************
 */

#ifndef MISC_H

//#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>

#define SZMAX_DBGMSG   (1023+1)

extern bool global_debugOn;   /* defined in: main.c */

/**
 * Macro displaying an error-message with debugging info in the stderr stream.
 */
#define DBG_STDERR_MSG( errmsg )                                \
do {                                                            \
	if ( !global_debugOn )                                  \
		break;                                          \
	fputs( "*** ERROR:\n", stderr );                        \
	fprintf(stderr,                                         \
		"*** File: %s | Func: %s | Line: %d\n*** %s\n", \
		__FILE__, __func__,  __LINE__,                  \
		(const char *)(errmsg) ? errmsg : "\0"          \
		);                                              \
} while(0)


#ifndef MISC_C
extern void dbg_print_info( char *fmtxt, ...);
extern int int_count_digits( int num );
extern char *s_fgets( char *s, int n, FILE *fp );
extern char *s_new_shortfname( const char *s );

#endif

#endif
