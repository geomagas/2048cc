/*****************************************************//**
 * @file	con_color_private.h
 * @version	1.04 (h3) alpha
 * @date	Jun 23, 2014
 * @author 	migf1 <mig_f1@hotmail.com>
 * @par Language:
 *		ISO C (C99)
 *
 * @brief	This file provides the private, low-level
 *		implementation of the CON_COLOR interface.
 *********************************************************
 */

#ifndef CON_COLOR_PRIVATE_H  /* START OF INCLUSION GUARD ============= */
#define CON_COLOR_PRIVATE_H


/* ======================================================================
 * Common On All Platforms (even unsupported ones)
 * ======================================================================
 */

#include <stdio.h>      /* printf(), ... */
#include <string.h>     /* memset(), strncpy(), strcmp(), ... */

/* Common constants */

#define CONOUT_LL_MAXLEN_ColorLABEL  (25+1)

#define CONOUT_LL_MAX_FGCOLORS        16  /* currently unused */
#define CONOUT_LL_MAX_BGCOLORS        16  /* currently unused */

/* */
#define CON_LL_DBG_INFO()                                         \
do {                                                              \
	fprintf(stderr, "*** File: %s | Line: %d | Func: %s()\n", \
		__FILE__, __LINE__, __func__);                    \
	fflush(stderr);                                           \
} while(0)

/*  Common Helper Macros */

#define errPUTS( msg )                                            \
do {                                                              \
	CON_LL_DBG_INFO();                                        \
	fputs( (const char *)msg, stderr);                        \
	fflush(stderr);                                           \
	putchar('\n');                                            \
	fflush(stdout);                                           \
} while(0)

/* */
#define errPRINTF( format, ... )                                  \
do {                                                              \
	CON_LL_DBG_INFO();                                        \
	fprintf(stderr, (const char *)(format), __VA_ARGS__);     \
	fflush(stderr);                                           \
} while(0)


/* ======================================================================
 * Detect Current Platform & Set Color Mode
 * ======================================================================
 */

#define CON_LL_PLATFORM_ISFORCED   0  /* false */
#define CON_LL_PLATFORM_UNKNOWN    CON_PLATFORM_UNKNOWN
#define CON_LL_PLATFORM_WINDOWS    CON_PLATFORM_WINDOWS
#define CON_LL_PLATFORM_LINUX      CON_PLATFORM_LINUX
#define CON_LL_PLATFORM_UNIX       CON_PLATFORM_UNIX
#define CON_LL_MAX_PLATFORMS       4

#define CON_LL_COLORMODE_ISFORCED  0  /* false */
#define CON_LL_COLORMODE_NOCOLORS  CON_COLORMODE_NOCOLORS
#define CON_LL_COLORMODE_WIN32     CON_COLORMODE_WIN32
#define CON_LL_COLORMODE_ANSI      CON_COLORMODE_ANSI
#define CON_LL_MAX_COLORMODES      3

/*
 * if CON_COLORMODE is externally defined, we force-set its defined value
 * (or default it to CON_LL_COLORMODE_NOCOLORS if it contains an invalid value)
 */
#ifdef CON_COLORMODE
	#undef CON_LL_COLORMODE_ISFORCED
	#define CON_LL_COLORMODE_ISFORCED        1  /* true */

	#if (CON_COLORMODE < CON_LL_COLORMODE_NOCOLORS)	\
	|| (CON_COLORMODE >= CON_LL_MAX_COLORMODES)
		#undef CON_COLORMODE
		#define CON_COLORMODE            CON_LL_COLORMODE_NOCOLORS
		#define CON_LL_STR_COLORMODE     "NoColors|DEFAULTED"

	#elif (CON_COLORMODE == CON_LL_COLORMODE_NOCOLORS)
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "NoColors|FORCED"

	#elif (CON_COLORMODE == CON_LL_COLORMODE_WIN32)
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "WIN32|FORCED"

	#elif (CON_COLORMODE == CON_LL_COLORMODE_ANSI)
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "ANSI|FORCED"

	#else
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "*** ERROR ***"

	#endif
#endif

/*
 * if CON_PLATFORM is externally defined, we force-set its defined value
 * (or default it to CON_LL_PLATFORM_UNKNOWN if it contains an invalid value)
 */
#ifdef CON_PLATFORM
	#undef CON_LL_PLATFORM_ISFORCED
	#define CON_LL_PLATFORM_ISFORCED         1  /* true */

	#if (CON_PLATFORM < CON_LL_PLATFORM_UNKNOWN) \
	|| (CON_PLATFORM >= CON_LL_MAX_PLATFORMS)
		#undef CON_PLATFORM
		#define CON_PLATFORM             CON_LL_PLATFORM_UNKNOWN
		#define CON_LL_STR_PLATFORM      "Unknown|DEFAULTED"

	#elif (CON_PLATFORM == CON_LL_PLATFORM_UNKNOWN)
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_LL_STR_PLATFORM      "Unknown|FORCED"

	#elif (CON_PLATFORM == CON_LL_PLATFORM_WINDOWS)
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_LL_STR_PLATFORM      "Windows|FORCED"

	#elif (CON_PLATFORM == CON_LL_PLATFORM_LINUX)
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_LL_STR_PLATFORM      "Linux|FORCED"

	#elif (CON_PLATFORM == CON_LL_PLATFORM_UNIX)
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_LL_STR_PLATFORM      "Unix|FORCED"

	#else
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_LL_STR_PLATFORM      "*** ERROR ***"

	#endif
#endif

/* Are we on a Windows platform ? */
#if defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__) \
|| defined(__TOS_WIN__)

	/* set CON_PLATFORM and CON_LL_STR_PLATFORM to Windows */
	#if ( !CON_LL_PLATFORM_ISFORCED )
		#ifdef CON_PLATFORM
			#undef CON_PLATFORM
		#endif
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_PLATFORM             CON_LL_PLATFORM_WINDOWS
		#define CON_LL_STR_PLATFORM      "Windows"
	#endif

	/* set CON_COLORMODE and CON_LL_STR_COLORMODE to WIN32 */
	#if ( !CON_LL_COLORMODE_ISFORCED )
		#undef CON_COLORMODE
		#define CON_COLORMODE            CON_LL_COLORMODE_WIN32
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "WIN32"
	#endif

/* Are we on a Linux platform ? */
#elif defined(__linux__) || defined(__linux) || defined(linux) \
|| defined(__gnu_linux__)

	/* set CON_PLATFORM and CON_LL_STR_PLATFORM to Linux */
	#if ( !CON_LL_PLATFORM_ISFORCED )
		#ifdef CON_PLATFORM
			#undef CON_PLATFORM
		#endif
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_PLATFORM             CON_LL_PLATFORM_LINUX
		#define CON_LL_STR_PLATFORM      "Linux"
	#endif

	/* set CON_COLORMODE and CON_LL_STR_COLORMODE to ANSI */
	#if ( !CON_LL_COLORMODE_ISFORCED )
		#undef CON_COLORMODE
		#define CON_COLORMODE            CON_LL_COLORMODE_ANSI
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "ANSI"
	#endif

/* Are we on a Unix platform? (CYGWIN and OSX count as Unix too) */
#elif defined(__unix__) || defined(__unix) || defined(unix) \
|| defined(__CYGWIN__)                                      \
|| ( defined(__APPLE__) && defined(__MACH__) )              \
|| ( defined(__APPLE__) && defined(__MACH) )

	/* set CON_PLATFORM and CON_LL_STR_PLATFORM to Unix */
	#if ( !CON_LL_PLATFORM_ISFORCED )
		#ifdef CON_PLATFORM
			#undef CON_PLATFORM
		#endif
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_PLATFORM             CON_LL_PLATFORM_UNIX
		#define CON_LL_STR_PLATFORM      "Unix"
	#endif

	/* set CON_COLORMODE and CON_LL_STR_COLORMODE to ANSI */
	#if ( !CON_LL_COLORMODE_ISFORCED )
		#undef CON_COLORMODE
		#define CON_COLORMODE            CON_LL_COLORMODE_ANSI
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "ANSI"
	#endif

/* Are we on Unsupported platform? */
#else
	/* set CON_PLATFORM and CON_LL_STR_PLATFORM to Unsupported */
	#if ( !CON_LL_PLATFORM_ISFORCED )
		#ifdef CON_PLATFORM
			#undef CON_PLATFORM
		#endif
		#ifdef CON_LL_STR_PLATFORM
			#undef CON_LL_STR_PLATFORM
		#endif
		#define CON_PLATFORM             CON_LL_PLATFORM_UNKNOWN
		#define CON_LL_STR_PLATFORM      "Unknown"
	#endif

	/* set CON_COLORMODE and CON_LL_STR_COLORMODE to Unsupported */
	#if ( !CON_LL_COLORMODE_ISFORCED )
		#undef CON_COLORMODE
		#define CON_COLORMODE            CON_LL_COLORMODE_NOCOLORS
		#ifdef CON_LL_STR_COLORMODE
			#undef CON_LL_STR_COLORMODE
		#endif
		#define CON_LL_STR_COLORMODE     "NoColors"
	#endif

/*
|| defined(__GNU__)				\
|| defined(__MINGW32__) || defined(__MINGW64__)

	#define CON_COLORMODE_ANSI
*/
#endif

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONSYS_LL_GET_PLATFORM()        CON_PLATFORM

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONSYS_LL_GET_PLATFORM_LABEL()  (const char *)CON_LL_STR_PLATFORM

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONSYS_LL_GET_COLORMODE()       CON_COLORMODE

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONSYS_LL_GET_COLORMODE_LABEL() (const char *)CON_LL_STR_COLORMODE


/* =======================================================================
 * Ensure that UNSUPPORTED platforms can be used too, producing uncolored
 * output.
 *
 * NOTE:
 *   This is treated as a completely autonomous special case, within its
 *   own #if directive. I'm using commented @ signs to visually emphasize
 *   it (actually, this should be a separate header file... I may do so
 *   in future versions).
 * =======================================================================
 */
#if (CON_COLORMODE == CON_LL_COLORMODE_NOCOLORS) /* @@@@@@@@@@@@@@@@@@@ */

	typedef int ConOut;
	typedef int ConColor_T;

	#define CONOUT_MSG_NOCOLOR \
		"*** CON_COLOR says: colors are disabled (unsupported platform?) ***"

	/* Foreground color-constants for UNSUPPORTED systems ---------- */

	#define FGLL_DEFAULT     0
	#define FGLL_NOCHANGE    0

	#define FGLL_BLACK       0
	#define FGLL_DARKGRAY    0

	#define FGLL_RED         0
	#define FGLL_DARKRED     0

	#define FGLL_GREEN       0
	#define FGLL_DARKGREEN   0

	#define FGLL_YELLOW      0
	#define FGLL_DARKYELLOW  0

	#define FGLL_BLUE        0
	#define FGLL_DARKBLUE    0

	#define FGLL_MAGENTA     0
	#define FGLL_DARKMAGENTA 0

	#define FGLL_CYAN        0
	#define FGLL_DARKCYAN    0

	#define FGLL_WHITE       0
	#define FGLL_GRAY        0

	#define VALID_FGLL( fg ) 0

	/* Background color-constants for UNSUPPORTED systems ---------- */

	#define BGLL_DEFAULT     0
	#define BGLL_NOCHANGE    0

	#define BGLL_BLACK       0

	#define BGLL_RED         0
	#define BGLL_DARKRED     0

	#define BGLL_GREEN       0
	#define BGLL_DARKGREEN   0

	#define BGLL_YELLOW      0
	#define BGLL_DARKYELLOW  0

	#define BGLL_BLUE        0
	#define BGLL_DARKBLUE    0

	#define BGLL_MAGENTA     0
	#define BGLL_DARKMAGENTA 0

	#define BGLL_CYAN        0
	#define BGLL_DARKCYAN    0

	#define BGLL_WHITE       0
	#define BGLL_GRAY        0
	#define BGLL_DARKGRAY    0

	#define VALID_BGLL( bg ) 0

	/* Medi-Level Macros for UNSUPPORTED systems ------------------- */

	#define CONSYS_LL_GET_FG_LABEL()  (const char *)CONOUT_MSG_NOCOLOR
	#define CONSYS_LL_GET_BG_LABEL()  (const char *)CONOUT_MSG_NOCOLOR

	#define CONOUT_LL_PAINT(bg)                             \
		do {                                            \
			putchar( '-' );                         \
			fflush(stdout);                         \
		} while(0)

	#define CONOUT_LL_PAINT_NTIMES(bg, ntimes)              \
		do {                                            \
			int MyI = 0;                            \
			for (MyI=0; MyI < (ntimes); MyI++)      \
				putchar( '_' );                 \
			fflush(stdout);                         \
		}while(0)

	#define CONOUT_LL_PUTCHAR(fg, bg, c)                    \
		do{                                             \
			putchar(c);                             \
			fflush(stdout);                         \
		}while(0)

	/* wide */
	#define CONOUT_LL_PUTWCHAR(fg, bg, wc)                  \
		do{                                             \
			putwchar(wc);                           \
			fflush(stdout);                         \
		}while(0)

	#define CONOUT_LL_PUTCHAR_NTIMES(fg, bg, c, ntimes)     \
		do {                                            \
			int i = 0;                              \
			for (i=0; i < (ntimes); i++)            \
				putchar( (int)(c) );            \
			fflush(stdout);                         \
		}while(0)

	/* wide */
	#define CONOUT_LL_PUTWCHAR_NTIMES(fg, bg, wc, ntimes)   \
		do {                                            \
			int i = 0;                              \
			for (i=0; i < (ntimes); i++)            \
				putwchar( (wchar_t)(wc) );      \
			fflush(stdout);                         \
		}while(0)

	#define CONOUT_LL_PUTS(fg, bg, str)         puts( (str) )

	/* wide */
	#define CONOUT_LL_PUTWS(fg, bg, wstr)                   \
		do {                                            \
			fputws( (wstr), stdout );               \
			fflush(stdout);                         \
		}while(0)

	#define CONOUT_LL_PRINTF(fg, bg, ...)                   \
		do {                                            \
			printf(__VA_ARGS__);                    \
			fflush( stdout );                       \
		} while(0)

	/* wide */
	#define CONOUT_LL_WPRINTF(fg, bg, ...)                  \
		do {                                            \
			wprintf(__VA_ARGS__);                   \
			fflush( stdout );                       \
		} while(0)

	#define CONOUT_LL_NPRINTF(n, fg, bg, ...)               \
		do {                                            \
			(n) = printf(__VA_ARGS__);              \
			fflush(stdout);                         \
		} while(0)

	/* wide */
	#define CONOUT_LL_NWPRINTF(n, fg, bg, ...)              \
		do {                                            \
			(n) = wprintf(__VA_ARGS__);             \
			fflush(stdout);                         \
		} while(0)

	#define CONOUT_LL_INIT()
	#define CONOUT_LL_RESTORE()

	#define CONOUT_LL_SAMECLR(clr1, clr2)       (0)
	#define CONOUT_LL_CPYCLR( dst, src )


	/* Low-Level Macros for UNSUPPORTED systems -------------------- */

	#define CONOUT_LL_RESET()
	#define CONOUT_LL_SET_COLOR( fg )
	#define CONOUT_LL_ADD_COLOR( bg )


	/* !!! Experimental (START) !!! */

	#define CONOUT_LL_SET_FG_LABEL()
	#define CONOUT_LL_SET_BG_LABEL()

	/* !!! Experimental (END) !!! */

#else	/* We have a SUPPORTED ColorMode  @@@@@@**@@@@@@@@@@@@@@@@@@@@@@ */

/* ======================================================================
 * Shared code on all SUPPORTED platforms, but ONLY AFTER the platform
 * has been detected.
 * ======================================================================
 */

#if (CON_COLORMODE == CON_LL_COLORMODE_ANSI)

	#define CONOUT_LL_DBG_DUMP( prompt )                            \
	do {                                                            \
		if ( NULL != (prompt) ) {                               \
			printf( "%s", (char *)(prompt) );               \
		}                                                       \
		printf( "cur.fg = ESC[%s (%s)\n",                       \
			&g_conout.curFg[2],                             \
			CONOUT_GET_FG_LABEL()                           \
			);                                              \
		printf( "cur.bg = ESC[%s (%s)\n",                       \
			&g_conout.curBg[2],                             \
			CONOUT_GET_BG_LABEL()                           \
			);                                              \
		printf( "cur.fg_label = '%s'\n", g_conout.fg_label );   \
		printf( "cur.bg_label = '%s'\n", g_conout.bg_label );   \
	} while(0)

#elif (CON_COLORMODE == CON_LL_COLORMODE_WIN32)

	#define CONOUT_LL_DBG_DUMP( prompt )                            \
	do {                                                            \
		if ( NULL != (char *)(prompt) ) {                       \
			printf( "%s", (char *)(prompt) );               \
		}                                                       \
		printf( "cur.fg = %04X (%s)\n",                         \
			g_conout.curFg,                                 \
			CONOUT_GET_FG_LABEL()                           \
			);                                              \
		printf( "cur.bg = %04X (%s)\n",                         \
			g_conout.curBg,                                 \
			CONOUT_GET_BG_LABEL()                           \
			);                                              \
		printf( "cur.fg_label = '%s'\n", g_conout.fg_label );   \
		printf( "cur.bg_label = '%s'\n", g_conout.bg_label );   \
	} while(0)
#endif

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONSYS_LL_GET_FG_LABEL()      (const char *)(g_conout.fg_label)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONSYS_LL_GET_BG_LABEL()      (const char *)(g_conout.bg_label)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PAINT(bg)                                             \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PAINT() failed: The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	if ( !VALID_BGLL((bg)) ) {                                      \
		errPUTS( "*** CONOUT_PAINT() failed: Invalid background color! ***"); \
		break;                                                  \
	}                                                               \
	CONOUT_LL_PUTCHAR(FG_NOCHANGE, (bg), ' ');                      \
}while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PAINT_NTIMES(bg, ntimes)                              \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PAINT_NTIMES(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	if ( !VALID_BGLL((bg)) ) {                                      \
		errPUTS( "*** CONOUT_PAINT_NTIMES() failed: Invalid background color! ***"); \
		break;                                                  \
	}                                                               \
	CONOUT_LL_PUTCHAR_NTIMES(FG_NOCHANGE, (bg), ' ', (ntimes));     \
}while(0)


/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PUTCHAR(fg, bg, c)                                    \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PUTCHAR(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR(fg);                                        \
	CONOUT_LL_ADD_COLOR(bg);                                        \
	putchar( (int)(c) );                                            \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
} while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PUTCHAR_NTIMES(fg, bg, c, ntimes)                     \
do {                                                                    \
	int ii = 0;                                                     \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PUTCHAR_NTIMES(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	for (ii=0; ii < (int)(ntimes); ii++) {                          \
		CONOUT_LL_SET_COLOR(fg);                                \
		CONOUT_LL_ADD_COLOR(bg);                                \
		putchar( (int)(c) );                                    \
		fflush(stdout);                                         \
		CONOUT_LL_RESET();                                      \
	}                                                               \
}while(0)

/*******************************//**
 * @brief works like puts(), but with 2 extra arguments at the front:
 * @param fg 	the desired foreground color for the output
 * @param bg 	the desired background color for the output
 * @note  make sure fg comes always BEFORE bg
 */
#define CONOUT_LL_PUTS(fg, bg, str)                                     \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PUTS(): The interface is NOT inited! ***");\
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR( (fg) );                                    \
	CONOUT_LL_ADD_COLOR( (bg) );                                    \
	printf( "%s", (char *)(str) );                                  \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
	putchar('\n');                                                  \
	fflush(stdout);                                                 \
} while(0)

/*******************************//**
 * @brief works like printf(), but with 2 extra arguments at the front:
 * @param fg 	the desired foreground color for the output
 * @param bg 	the desired background color for the output
 * @note  make sure fg comes always BEFORE bg
 */
#define CONOUT_LL_PRINTF(fg, bg, ...)                                   \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PRINTF(): The interface is NOT inited! ***");\
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR( (fg) );                                    \
	CONOUT_LL_ADD_COLOR( (bg) );                                    \
	printf( __VA_ARGS__ );                                          \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
} while(0)

/*******************************//**
 * @brief works like printf(), but with 3 extra arguments at the front:
 * @param n 	gets the return value of the printf() call
 * @param fg 	the desired foreground color for the output
 * @param bg 	the desired background color for the output
 * @note make sure fg comes always BEFORE bg
 */
#define CONOUT_LL_NPRINTF(n, fg, bg, ...)                               \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_NPRINTF(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR( (fg) );                                    \
	CONOUT_LL_ADD_COLOR( (bg) );                                    \
	(n) = printf( __VA_ARGS__ );                                    \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
} while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PUTWCHAR(fg, bg, wc)                                  \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PUTWCHAR(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR(fg);                                        \
	CONOUT_LL_ADD_COLOR(bg);                                        \
	putwchar( (wchar_t)(wc) );                                      \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
} while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PUTWCHAR_NTIMES(fg, bg, wc, ntimes)                   \
do {                                                                    \
	int i = 0;                                                      \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PUTWCHAR_NTIMES(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	for (i=0; i < (ntimes); i++) {                                  \
		CONOUT_LL_SET_COLOR(fg);                                \
		CONOUT_LL_ADD_COLOR(bg);                                \
		putwchar( (wchar_t)(wc) );                              \
		fflush(stdout);                                         \
		CONOUT_LL_RESET();                                      \
	}                                                               \
}while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_PUTWS(fg, bg, wstr)                                   \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_PUTWS(): The interface is NOT inited! ***");\
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR( (fg) );                                    \
	CONOUT_LL_ADD_COLOR( (bg) );                                    \
	wprintf( L"%ls", (wchar_t *)(wstr) );                           \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
	putchar('\n');                                                  \
	fflush(stdout);                                                 \
} while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_WPRINTF(fg, bg, ...)                                  \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_WPRINTF(): The interface is NOT inited! ***");\
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR( (fg) );                                    \
	CONOUT_LL_ADD_COLOR( (bg) );                                    \
	wprintf( __VA_ARGS__ );                                         \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
} while(0)

/********************************************************************//**
 * @par    -------
 * @brief
 * @note
 */
#define CONOUT_LL_NWPRINTF(n, fg, bg, ...)                              \
do {                                                                    \
	if ( !g_conout.isInited ) {                                     \
		errPUTS( "*** CONOUT_NWPRINTF(): The interface is NOT inited! ***"); \
		break;                                                  \
	}                                                               \
	CONOUT_LL_SET_COLOR( (fg) );                                    \
	CONOUT_LL_ADD_COLOR( (bg) );                                    \
	(n) = wprintf( __VA_ARGS__ );                                   \
	fflush(stdout);                                                 \
	CONOUT_LL_RESET();                                              \
} while(0)

/* ======================================================================
 * WIN32 SPECIFIC
 * ======================================================================
 */
#if ( CON_COLORMODE == CON_LL_COLORMODE_WIN32 )

	#include <windows.h>

	/* Win32 internal data types (NOT intended for end-users) */
	typedef struct ConOut {        /* */
		int 	isInited;
		HANDLE	handle;
		WORD	curFg, curBg;
		CONSOLE_SCREEN_BUFFER_INFO csbiOriginal;
		CONSOLE_SCREEN_BUFFER_INFO csbiCurrent;
		char 	fg_label[ CONOUT_LL_MAXLEN_ColorLABEL ];
		char 	bg_label[ CONOUT_LL_MAXLEN_ColorLABEL ];
	}ConOut;
	typedef WORD ConColor_T;       /* Win32 colors are 16bit WORDs */

	/* ================================
	 * the Win32 API defines console colors
	 * as 16bit WORDs in <wincon.h>
	 *
	#define FOREGROUND_BLUE         0x0001
	#define FOREGROUND_GREEN        0x0002
	#define FOREGROUND_RED          0x0004
	#define FOREGROUND_INTENSITY    0x0008

	#define BACKGROUND_BLUE         0x0010
	#define BACKGROUND_GREEN        0x0020
	#define BACKGROUND_RED          0x0040
	#define BACKGROUND_INTENSITY    0x0080
	 */

	/* Foreground color-constants for Win32 ---------------------- */

	#define FGLL_NOCHANGE      0xFFFF

	#define FGLL_DEFAULT       CONOUT_LL_XTRACT_FG_FROM(             \
	                               g_conout.csbiOriginal.wAttributes \
                                   )

	#define FGLL_BLACK         0x0000

	#define FGLL_WHITE        (FOREGROUND_RED|FOREGROUND_GREEN       \
                                  |FOREGROUND_BLUE|FOREGROUND_INTENSITY)

	#define FGLL_GRAY         (FOREGROUND_RED|FOREGROUND_GREEN       \
	                          |FOREGROUND_BLUE)

	#define FGLL_DARKGRAY      (FOREGROUND_INTENSITY)

	#define FGLL_RED           (FOREGROUND_RED|FOREGROUND_INTENSITY)
	#define FGLL_DARKRED       (FOREGROUND_RED)

	#define FGLL_GREEN         (FOREGROUND_GREEN|FOREGROUND_INTENSITY)
	#define FGLL_DARKGREEN     (FOREGROUND_GREEN)

	#define FGLL_BLUE          (FOREGROUND_BLUE|FOREGROUND_INTENSITY)
	#define FGLL_DARKBLUE      (FOREGROUND_BLUE)

	#define FGLL_YELLOW        (FOREGROUND_RED|FOREGROUND_GREEN      \
	                           |FOREGROUND_INTENSITY)

	#define FGLL_DARKYELLOW    (FOREGROUND_RED|FOREGROUND_GREEN)

	#define FGLL_MAGENTA       (FOREGROUND_RED|FOREGROUND_BLUE       \
	                           |FOREGROUND_INTENSITY)

	#define FGLL_DARKMAGENTA   (FOREGROUND_RED|FOREGROUND_BLUE)

	#define FGLL_CYAN          (FOREGROUND_GREEN|FOREGROUND_BLUE     \
	                           |FOREGROUND_INTENSITY)

	#define FGLL_DARKCYAN      (FOREGROUND_GREEN|FOREGROUND_BLUE)

	#define VALID_FGLL( fg )                \
	(                                       \
		FGLL_WHITE == (fg)              \
		|| FGLL_GRAY == (fg)            \
		|| FGLL_RED == (fg)             \
		|| FGLL_DARKRED == (fg)         \
		|| FGLL_GREEN == (fg)           \
		|| FGLL_DARKGREEN == (fg)       \
		|| FGLL_BLUE == (fg)            \
		|| FGLL_DARKBLUE == (fg)        \
		|| FGLL_YELLOW == (fg)          \
		|| FGLL_DARKYELLOW == (fg)      \
		|| FGLL_MAGENTA == (fg)         \
		|| FGLL_DARKMAGENTA == (fg)     \
		|| FGLL_CYAN == (fg)            \
		|| FGLL_DARKCYAN == (fg)        \
		|| FGLL_DARKGRAY == (fg)        \
		|| FGLL_BLACK == (fg)           \
	)

	/* Background color-constants for Win32 ---------------------- */

	#define BGLL_NOCHANGE      0xFFFF

	#define BGLL_DEFAULT       CONOUT_LL_XTRACT_BG_FROM(             \
	                               g_conout.csbiOriginal.wAttributes \
	                            )

	#define BGLL_BLACK         0x0000

	#define BGLL_WHITE         (BACKGROUND_RED|BACKGROUND_GREEN      \
	                           |BACKGROUND_BLUE|BACKGROUND_INTENSITY)

	#define BGLL_GRAY          (BACKGROUND_RED|BACKGROUND_GREEN      \
	                           |BACKGROUND_BLUE)

	#define BGLL_DARKGRAY      (BACKGROUND_INTENSITY)

	#define BGLL_RED           (BACKGROUND_RED|BACKGROUND_INTENSITY)
	#define BGLL_DARKRED       (BACKGROUND_RED)

	#define BGLL_GREEN         (BACKGROUND_GREEN|BACKGROUND_INTENSITY)
	#define BGLL_DARKGREEN     (BACKGROUND_GREEN)

	#define BGLL_BLUE          (BACKGROUND_BLUE|BACKGROUND_INTENSITY)
	#define BGLL_DARKBLUE      (BACKGROUND_BLUE)

	#define BGLL_YELLOW        (BACKGROUND_RED|BACKGROUND_GREEN      \
	                           |BACKGROUND_INTENSITY)

	#define BGLL_DARKYELLOW    (BACKGROUND_RED|BACKGROUND_GREEN)

	#define BGLL_MAGENTA       (BACKGROUND_RED|BACKGROUND_BLUE       \
	                           |BACKGROUND_INTENSITY)

	#define BGLL_DARKMAGENTA   (BACKGROUND_RED|BACKGROUND_BLUE)

	#define BGLL_CYAN          (BACKGROUND_GREEN|BACKGROUND_BLUE     \
	                           |BACKGROUND_INTENSITY)

	#define BGLL_DARKCYAN      (BACKGROUND_GREEN|BACKGROUND_BLUE)

	#define VALID_BGLL( bg )                \
	(                                       \
		BGLL_WHITE == (bg)              \
		|| BGLL_GRAY == (bg)            \
		|| BGLL_RED == (bg)             \
		|| BGLL_DARKRED == (bg)         \
		|| BGLL_GREEN == (bg)           \
		|| BGLL_DARKGREEN == (bg)       \
		|| BGLL_BLUE == (bg)            \
		|| BGLL_DARKBLUE == (bg)        \
		|| BGLL_YELLOW == (bg)          \
		|| BGLL_DARKYELLOW == (bg)      \
		|| BGLL_MAGENTA == (bg)         \
		|| BGLL_DARKMAGENTA == (bg)     \
		|| BGLL_CYAN == (bg)            \
		|| BGLL_DARKCYAN == (bg)        \
		|| BGLL_DARKGRAY == (bg)        \
		|| BGLL_BLACK == (bg)           \
	)

	/* Win32 HiLevel Macros (NOT intended for end-users) --------- */

	/*******************************//**
	 * WIN32 - Medi Level
	 * @brief Save console's original color-state & Init the interface
	 */
	#define CONOUT_LL_INIT()                                         \
	do {                                                             \
		if ( g_conout.isInited ) {                               \
			errPUTS( "*** CONOUT_INIT(): The interface is ALREADY inited! ***" );\
			break;                                           \
		}                                                        \
		                                                         \
		memset( &g_conout, 0, sizeof(ConOut) );                  \
		                                                         \
		g_conout.handle = GetStdHandle( STD_OUTPUT_HANDLE );     \
		if ( INVALID_HANDLE_VALUE == g_conout.handle ) {         \
			errPRINTF(                                       \
				"*** CONOUT_LL_INIT() failed: %d! ***\n",\
				(int)GetLastError()                      \
				);                                       \
			break;                                           \
		}                                                        \
		                                                         \
		if ( !GetConsoleScreenBufferInfo(g_conout.handle,        \
		                                &g_conout.csbiCurrent)   \
		){                                                       \
			errPRINTF(                                       \
				"*** CONOUT_LL_INIT() failed: %d! ***\n",\
				(int)GetLastError()                      \
				);                                       \
			break;                                           \
		}                                                        \
		                                                         \
		g_conout.isInited = 1;                                   \
		memcpy(                                                  \
			&g_conout.csbiOriginal,                          \
			&g_conout.csbiCurrent,                           \
			sizeof(CONSOLE_SCREEN_BUFFER_INFO)               \
			);                                               \
		g_conout.curFg = CONOUT_LL_XTRACT_FG_FROM(               \
					g_conout.csbiOriginal.wAttributes\
					);                               \
		g_conout.curBg = CONOUT_LL_XTRACT_BG_FROM(               \
					g_conout.csbiOriginal.wAttributes\
					);                               \
		                                                         \
		CONOUT_LL_SET_FG_LABEL();                                \
		CONOUT_LL_SET_BG_LABEL();                                \
	} while(0)

	/*******************************//**
	 * WIN32 - Medi Level
	 * @brief restore console's original color-state & update the interface
	 */
	#define CONOUT_LL_RESTORE()                                      \
	do {                                                             \
		if ( !g_conout.isInited ) {                              \
			errPUTS("*** CONOUT_RESTORE(): Interface is NOT inited! ***"); \
			break;                                           \
		}                                                        \
		if ( !SetConsoleTextAttribute(                           \
			g_conout.handle,                                 \
			g_conout.csbiOriginal.wAttributes                \
			)                                                \
		){                                                       \
			errPRINTF(                                       \
				"*** CONOUT_RESTORE() failed: %d! ***\n",\
				(int)GetLastError()                      \
				);                                       \
			break;                                           \
		}                                                        \
		memcpy(                                                  \
			&g_conout.csbiCurrent,                           \
			&g_conout.csbiOriginal,                          \
			sizeof(CONSOLE_SCREEN_BUFFER_INFO)               \
			);                                               \
		g_conout.curFg = CONOUT_LL_XTRACT_FG_FROM(               \
					g_conout.csbiCurrent.wAttributes \
					);                               \
		g_conout.curBg = CONOUT_LL_XTRACT_BG_FROM(               \
					g_conout.csbiCurrent.wAttributes \
					);                               \
		CONOUT_LL_SET_FG_LABEL();                                \
		CONOUT_LL_SET_BG_LABEL();                                \
	} while(0)

	/*******************************//**
	 * WIN32 - Medi Level
	 * @brief compare 2 colors for equality
	 */
	#define CONOUT_LL_SAMECLR(clr1, clr2) \
		( (WORD)(clr1) == (WORD)(clr2) )

	/*******************************//**
	 * WIN32 - Medi Level
	 * @brief copy source color (src) to destination color (dst)
	 * @note  both colors should be of same type (e.g. both FGLL_
	 * or both BGLL_ colors)
	 */
	#define CONOUT_LL_CPYCLR( dst, src )                             \
	do {                                                             \
		if ( !g_conout.isInited ) {                              \
			errPUTS("*** CONOUT_CPYCLR(): The interface is NOT inited! ***"); \
			break;                                           \
		}                                                        \
		(dst) = (WORD)(src);                                     \
	} while(0)


	/* Win32 LowLevel Macros (NOT intended for end-users) ------- */

	/*******************************//**
	 * WIN32 - Low Level
	 * @brief reset console's original color-state but do NOT update
	 *        the interface
	 */
	#define CONOUT_LL_RESET()                                        \
	do {                                                             \
		if ( !g_conout.isInited ) {                              \
			errPUTS( "*** CONOUT_RESET(): The interface is NOT inited! ***"); \
			break;                                           \
		}                                                        \
		if ( !SetConsoleTextAttribute(                           \
			g_conout.handle,                                 \
			g_conout.csbiOriginal.wAttributes                \
			)                                                \
		){                                                       \
			errPRINTF(                                       \
				"*** CONOUT_RESET() failed: %d! ***\n",  \
				(int)GetLastError()                      \
				);                                       \
			break;                                           \
		}                                                        \
	} while(0)

	/*******************************//**
	 * WIN32 - Low Level (DO NOT USE DIRECTLY)
	 * @brief extract the fg attribute from a Win32 APIs console
	 *        text-attributes WORD
	 * @note  FGLL_DARKGRAY and FGLL_BLACK must be checked last
	 */
	#define CONOUT_LL_XTRACT_FG_FROM( from )                              \
	(                                                                     \
	FGLL_WHITE == ((from) & FGLL_WHITE)                                   \
	? FGLL_WHITE                                                          \
	: FGLL_GRAY == ((from) & FGLL_GRAY)                                   \
	  ? FGLL_GRAY                                                         \
	  : FGLL_RED == ((from) & FGLL_RED)                                   \
	    ? FGLL_RED                                                        \
	    : FGLL_DARKRED == ((from) & FGLL_DARKRED)                         \
	      ? FGLL_DARKRED                                                  \
	      : FGLL_GREEN == ((from) & FGLL_GREEN)                           \
		? FGLL_GREEN                                                  \
		: FGLL_DARKGREEN == ((from) & FGLL_DARKGREEN)                 \
		  ? FGLL_DARKGREEN                                            \
		  : FGLL_BLUE == ((from) & FGLL_BLUE)                         \
		    ? FGLL_BLUE                                               \
		    : FGLL_DARKBLUE == ((from) & FGLL_DARKBLUE)               \
		      ? FGLL_DARKBLUE                                         \
		      : FGLL_YELLOW == ((from) & FGLL_YELLOW)                 \
			? FGLL_YELLOW                                         \
			: FGLL_DARKYELLOW == ((from) & FGLL_DARKYELLOW)       \
			  ? FGLL_DARKYELLOW                                   \
			  : FGLL_MAGENTA == ((from) & FGLL_MAGENTA)           \
			    ? FGLL_MAGENTA                                    \
			    : FGLL_DARKMAGENTA == ((from) & FGLL_DARKMAGENTA) \
			      ? FGLL_DARKMAGENTA                              \
			      : FGLL_CYAN == ((from) & FGLL_CYAN)             \
				? FGLL_CYAN                                   \
				: FGLL_DARKCYAN == ((from) & FGLL_DARKCYAN)   \
				  ? FGLL_DARKCYAN                             \
				  : FGLL_DARKGRAY == ((from) & FGLL_DARKGRAY) \
				    ? FGLL_DARKGRAY                           \
				    : FGLL_BLACK == ((from) & FGLL_BLACK)     \
				      ? FGLL_BLACK                            \
				      : FGLL_NOCHANGE                         \
	)

	/*******************************//**
	 * WIN32 - Low Level (DO NOT USE DIRECTLY)
	 * @brief extract the bg attribute from a Win32 APIs console
	 *        text-attributes WORD
	 * @note  BGLL_DARKGRAY and BGLL_BLACK must be checked last
	 */
	#define CONOUT_LL_XTRACT_BG_FROM( from )                              \
	(                                                                     \
	BGLL_WHITE == ((from) & BGLL_WHITE)                                   \
	? BGLL_WHITE                                                          \
	: BGLL_GRAY == ((from) & BGLL_GRAY)                                   \
	  ? BGLL_GRAY                                                         \
	  : BGLL_RED == ((from) & BGLL_RED)                                   \
	    ? BGLL_RED                                                        \
	    : BGLL_DARKRED == ((from) & BGLL_DARKRED)                         \
	      ? BGLL_DARKRED                                                  \
	      : BGLL_GREEN == ((from) & BGLL_GREEN)                           \
		? BGLL_GREEN                                                  \
		: BGLL_DARKGREEN == ((from) & BGLL_DARKGREEN)                 \
		  ? BGLL_DARKGREEN                                            \
		  : BGLL_BLUE == ((from) & BGLL_BLUE)                         \
		    ? BGLL_BLUE                                               \
		    : BGLL_DARKBLUE == ((from) & BGLL_DARKBLUE)               \
		      ? BGLL_DARKBLUE                                         \
		      : BGLL_YELLOW == ((from) & BGLL_YELLOW)                 \
			? BGLL_YELLOW                                         \
			: BGLL_DARKYELLOW == ((from) & BGLL_DARKYELLOW)       \
			  ? BGLL_DARKYELLOW                                   \
			  : BGLL_MAGENTA == ((from) & BGLL_MAGENTA)           \
			    ? BGLL_MAGENTA                                    \
			    : BGLL_DARKMAGENTA == ((from) & BGLL_DARKMAGENTA) \
			      ? BGLL_DARKMAGENTA                              \
			      : BGLL_CYAN == ((from) & BGLL_CYAN)             \
				? BGLL_CYAN                                   \
				: BGLL_DARKCYAN == ((from) & BGLL_DARKCYAN)   \
				  ? BGLL_DARKCYAN                             \
				  : BGLL_DARKGRAY == ((from) & BGLL_DARKGRAY) \
				    ? BGLL_DARKGRAY                           \
				    : BGLL_BLACK == ((from) & BGLL_BLACK)     \
				      ? BGLL_BLACK                            \
				      : BGLL_NOCHANGE                         \
	)

	/*******************************//**
	 * WIN32 - Low Level (DO NOT USE DIRECTLY)
	 * @brief set the foreground color (do some housekeeping too)
	 */
	#define CONOUT_LL_SET_COLOR( fg )                                \
	do {                                                             \
		WORD tryFg, tryFgBg;                                     \
		                                                         \
		if ( !g_conout.isInited ) {                              \
			errPUTS("*** CONOUT_SET_FG(): The interface is NOT inited! ***"); \
			break;                                           \
		}                                                        \
		                                                         \
		tryFg = CONOUT_LL_SAMECLR((fg), FGLL_NOCHANGE)           \
			? g_conout.curFg                                 \
			: (fg);                                          \
		                                                         \
		if ( !VALID_FGLL(tryFg) ) {                              \
			errPRINTF(                                       \
				"*** CONOUT_SET_FG() failed: Invalid fg color 0x%04x ***\n",\
				tryFg                                    \
				);                                       \
			break;                                           \
		}                                                        \
		                                                         \
		tryFgBg = tryFg | g_conout.curBg;                        \
		if ( !SetConsoleTextAttribute(g_conout.handle, tryFgBg) )\
		{                                                        \
			errPRINTF(                                       \
				"*** CONOUT_SET_FG() failed, with Win32 API exit code %d!\n",\
				(int)GetLastError()                      \
				);                                       \
			break;                                           \
		}                                                        \
		g_conout.csbiCurrent.wAttributes = tryFgBg;              \
		g_conout.curFg = tryFg;                                  \
		CONOUT_LL_SET_FG_LABEL();                                \
	} while(0)

	/*******************************//**
	 * WIN32 - Low Level (DO NOT USE DIRECTLY)
	 * @brief add the background color (do some housekeeping too)
	 */
	#define CONOUT_LL_ADD_COLOR( bg )                                \
	do {                                                             \
		WORD tryBg, tryFgBg;                                     \
		                                                         \
		if ( !g_conout.isInited ) {                              \
			errPUTS( "*** CONOUT_SET_BG(): The interface is NOT inited! ***");\
			break;                                           \
		}                                                        \
		                                                         \
		tryBg = CONOUT_LL_SAMECLR((bg), BGLL_NOCHANGE)           \
			? g_conout.curBg                                 \
			: (bg);                                          \
		                                                         \
		if ( !VALID_BGLL(tryBg) ) {                              \
			errPRINTF(                                       \
				"*** CONOUT_SET_BG() failed: Invalid bg color 0x%04x ***\n",\
				  tryBg                                  \
				);                                       \
			break;                                           \
		}                                                        \
		tryFgBg = g_conout.curFg | tryBg;                        \
		if ( !SetConsoleTextAttribute(g_conout.handle, tryFgBg) )\
		{                                                        \
			errPRINTF(                                       \
				"*** CONOUT_SET_BG() failed, with Win32 API exit code %d!\n",\
				(int)GetLastError()                      \
				);					 \
			break;                                           \
		}                                                        \
		g_conout.csbiCurrent.wAttributes = tryFgBg;              \
		g_conout.curBg = tryBg;                                  \
		CONOUT_LL_SET_BG_LABEL();                                \
	} while(0)



	/* !!! Experimental (START) !!! */

	/*******************************//**
	 * WIN32 * Low Level (DO NOT USE DIRECTLY)
	 * @brief
	 * 	convert current fg to c-string & keep it
	 * @note
	 * 	- FG_NOCHANGE must be check first
	 *	- FG_DEFAULT need not be checked (it is already a valid color)
	 * 	- FG_DARKCYAN must be checked just before last
	 *	- FG_BLACK must be checked last
	 */
	#define CONOUT_LL_SET_FG_LABEL()                                     \
	do {                                                                 \
		/* nochange */                                               \
		if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_NOCHANGE) )	{    \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"FG_NOCHANGE",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* white */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_WHITE) ) {  \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"white",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* gray, darkgray */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_GRAY) ) {   \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"grey",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKGRAY) ){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark grey",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1 );             \
		}                                                            \
		/* red, darkred */                                           \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_RED) ) {    \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"red", CONOUT_LL_MAXLEN_ColorLABEL-1         \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKRED) ){ \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark red",                                  \
				CONOUT_LL_MAXLEN_ColorLABEL-1 );             \
		}                                                            \
		/* green, darkgreen */                                       \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_GREEN) ) {  \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"green",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKGREEN)){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark green",                                \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* blue, darkblue */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_BLUE) ) {   \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"blue",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKBLUE) ){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark blue",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* yellow, darkyellow */                                     \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_YELLOW) ) { \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"yellow",                                    \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKYELLOW)){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark yellow",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* magenta, darkmagenta */                                   \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_MAGENTA) ){ \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"magenta",                                   \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKMAGENTA)){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark magenta",                              \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* cyan, darkcyan */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_CYAN) ) {   \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"cyan",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKCYAN) ){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark cyan",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* black */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_BLACK) ) {  \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"black",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
	} while(0)

	/*******************************//**
	 * WIN32 * Low Level (DO NOT USE DIRECTLY)
	 * @brief convert current bg to c-string & keep it
	 * @note
	 * 	  - BG_NOCHANGE must be check first
	 * 	  - BG_DEFAULT need not be checked (it is already a valid color)
	 * 	  - BG_DARKCYAN must be checked just before last
	 * 	  - BG_BLACK must be checked last
	 */
	#define CONOUT_LL_SET_BG_LABEL()                                     \
	do {                                                                 \
		/* nochange */                                               \
		if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_NOCHANGE) ) {    \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"BG_NOCHANGE",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* white */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_WHITE) ) {  \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"white",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* black */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_BLACK) ) {  \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"black",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* gray, darkgray */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_GRAY) ) {   \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"grey",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKGRAY)) { \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark grey",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1 );             \
		}                                                            \
		/* red, darkred */                                           \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_RED) ) {    \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"red",                                       \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKRED) ){ \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark red",                                  \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* green, darkgreen */                                       \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_GREEN) ) {  \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"green",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKGREEN)) {\
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark green",                                \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* blue, darkblue */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_BLUE) ) {   \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"blue",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKBLUE)){ \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark blue",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* yellow, darkyellow */                                     \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_YELLOW) ) { \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"yellow",                                    \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKYELLOW)){\
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark yellow",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* magenta, darkmagenta */                                   \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_MAGENTA) ) {\
			strncpy(                                             \
				g_conout.bg_label,                           \
				"magenta",                                   \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKMAGENTA)){\
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark magenta",                              \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* cyan, darkcyan */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_CYAN) ) {   \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"cyan",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKCYAN)) {\
			strncpy(                                             \
				g_conout.bg_label,                           \
				"dark cyan",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
	} while(0)

/* !!! WIN32 Experimental (END) !!! */



/* ======================================================================
 * ANSI SPECIFIC
 * ======================================================================
 */
#elif (CON_COLORMODE == CON_LL_COLORMODE_ANSI)

	#define CONOUT_LL_CLRSZ    (31+2) /* max size of any color-string */

	/* ANSI internal data types (NOT intended for end-users) */

	typedef struct ConOut {
		int  isInited;
		char curFg[ CONOUT_LL_CLRSZ ];
		char curBg[ CONOUT_LL_CLRSZ ];
		char fg_label[ CONOUT_LL_MAXLEN_ColorLABEL ];
		char bg_label[ CONOUT_LL_MAXLEN_ColorLABEL ];
	}ConOut;

	/* ANSI COLORS are c-strings */
	typedef char ConColor_T[ CONOUT_LL_CLRSZ ];

	#define FGLL_NOCHANGE      "fg-nochange"
	#define BGLL_NOCHANGE      "bg-nochange"

	/* Foreground color-constants for ANSI systems --------------- */

	#define FGLL_DEFAULT       "\033[39m"

	#define FGLL_BLACK         "\033[30m"
	#define FGLL_DARKGRAY      "\033[1;30m"

	#define FGLL_RED           "\033[1;31m"
	#define FGLL_DARKRED       "\033[31m"

	#define FGLL_GREEN         "\033[1;32m"
	#define FGLL_DARKGREEN     "\033[32m"

	#define FGLL_YELLOW        "\033[1;33m"
	#define FGLL_DARKYELLOW    "\033[33m"

	#define FGLL_BLUE          "\033[1;34m"
	#define FGLL_DARKBLUE      "\033[34m"

	#define FGLL_MAGENTA       "\033[1;35m"
	#define FGLL_DARKMAGENTA   "\033[35m"

	#define FGLL_CYAN          "\033[1;36m"
	#define FGLL_DARKCYAN      "\033[36m"

	#define FGLL_WHITE         "\033[1;37m"
	#define FGLL_GRAY          "\033[37m"

	#define VALID_FGLL( fg )                        \
	(                                               \
		0 == strcmp( FGLL_DEFAULT, (fg) )       \
		|| 0 == strcmp( FGLL_WHITE, (fg) )      \
		|| 0 == strcmp( FGLL_GRAY, (fg) )       \
		|| 0 == strcmp( FGLL_RED, (fg) )        \
		|| 0 == strcmp( FGLL_DARKRED, (fg) )    \
		|| 0 == strcmp( FGLL_GREEN, (fg) )      \
		|| 0 == strcmp( FGLL_DARKGREEN, (fg) )  \
		|| 0 == strcmp( FGLL_BLUE, (fg) )       \
		|| 0 == strcmp( FGLL_DARKBLUE, (fg) )   \
		|| 0 == strcmp( FGLL_YELLOW, (fg) )     \
		|| 0 == strcmp( FGLL_DARKYELLOW, (fg) ) \
		|| 0 == strcmp( FGLL_MAGENTA, (fg) )    \
		|| 0 == strcmp( FGLL_DARKMAGENTA, (fg) )\
		|| 0 == strcmp( FGLL_CYAN, (fg) )       \
		|| 0 == strcmp( FGLL_DARKCYAN, (fg) )   \
		|| 0 == strcmp( FGLL_DARKGRAY, (fg) )   \
		|| 0 == strcmp( FGLL_BLACK, (fg) )      \
	)


	/* Background color-constants for ANSI systems --------------- */

	#define BGLL_DEFAULT       "\033[49m"

	#define BGLL_BLACK         "\033[40m"

	#define BGLL_RED           "\033[41m"
	#define BGLL_DARKRED       BGLL_RED

	#define BGLL_GREEN		"\033[42m"
	#define BGLL_DARKGREEN		BGLL_GREEN

	#define BGLL_YELLOW		"\033[43m"
	#define BGLL_DARKYELLOW		BGLL_YELLOW

	#define BGLL_BLUE		"\033[44m"
	#define BGLL_DARKBLUE		BGLL_BLUE

	#define BGLL_MAGENTA		"\033[45m"
	#define BGLL_DARKMAGENTA        BGLL_MAGENTA

	#define BGLL_CYAN               "\033[46m"
	#define BGLL_DARKCYAN           BGLL_CYAN

	#define BGLL_WHITE              "\033[47m"
	#define BGLL_GRAY               "\033[47m"
	#define BGLL_DARKGRAY           BGLL_GRAY

	#define VALID_BGLL( bg )                        \
	(                                               \
		0 == strcmp( BGLL_DEFAULT, (bg) )       \
		|| 0 == strcmp( BGLL_WHITE, (bg) )      \
		|| 0 == strcmp( BGLL_GRAY, (bg) )       \
		|| 0 == strcmp( BGLL_RED, (bg) )        \
		|| 0 == strcmp( BGLL_DARKRED, (bg) )    \
		|| 0 == strcmp( BGLL_GREEN, (bg) )      \
		|| 0 == strcmp( BGLL_DARKGREEN, (bg) )  \
		|| 0 == strcmp( BGLL_BLUE, (bg) )       \
		|| 0 == strcmp( BGLL_DARKBLUE, (bg) )   \
		|| 0 == strcmp( BGLL_YELLOW, (bg) )     \
		|| 0 == strcmp( BGLL_DARKYELLOW, (bg) ) \
		|| 0 == strcmp( BGLL_MAGENTA, (bg) )    \
		|| 0 == strcmp( BGLL_DARKMAGENTA, (bg) )\
		|| 0 == strcmp( BGLL_CYAN, (bg) )       \
		|| 0 == strcmp( BGLL_DARKCYAN, (bg) )   \
		|| 0 == strcmp( BGLL_DARKGRAY, (bg) )   \
		|| 0 == strcmp( BGLL_BLACK, (bg) )      \
	)

	/* ANSI HighLevel Macros (intended for end-users) ------------ */


	/*******************************//**
	 * ANSI - Medi Level
	 * @brief save console's original color-state & init the interface
	 */
	#define CONOUT_LL_INIT()                                        \
	do {                                                            \
		if ( g_conout.isInited ) {                              \
			errPUTS( "*** CONOUT_INIT(): The interface is ALREADY inited! ***" );\
			break;                                          \
		}                                                       \
		                                                        \
		memset( &g_conout, 0, sizeof(ConOut) );	                \
		                                                        \
		printf( "%s", "\033[0m" );                              \
		fflush( stdout );                                       \
		                                                        \
		g_conout.isInited = 1;                                  \
		CONOUT_LL_CPYCLR(g_conout.curFg, FGLL_DEFAULT);         \
		CONOUT_LL_CPYCLR(g_conout.curBg, BGLL_DEFAULT);         \
		CONOUT_LL_SET_FG_LABEL();                               \
		CONOUT_LL_SET_BG_LABEL();                               \
	} while( 0 )

	/*******************************//**
	 * ANSI - Medi Level
	 * @brief restore console's original color-state & update the interface
	 */
	#define CONOUT_LL_RESTORE()                                     \
	do {                                                            \
		if ( !g_conout.isInited ) {                             \
			errPUTS( "*** CONOUT_RESTORE(): The interface is NOT inited! ***");\
			break;                                          \
		}                                                       \
		                                                        \
		printf( "%s", "\033[0m" );                              \
		fflush(stdout);                                         \
		                                                        \
		CONOUT_LL_CPYCLR(g_conout.curFg, FGLL_DEFAULT);         \
		CONOUT_LL_CPYCLR(g_conout.curBg, BGLL_DEFAULT);         \
		CONOUT_LL_SET_FG_LABEL();                               \
		CONOUT_LL_SET_BG_LABEL();                               \
	} while(0)

	/*******************************//**
	 * ANSI - Medi Level
	 * @brief compare 2 colors for equality
	 */
	#define CONOUT_LL_SAMECLR(clr1, clr2)   \
		( 0 == strcmp((const char *)(clr1), (const char *)(clr2)) )

	/*******************************//**
	 * ANSI - Medi Level
	 * @brief copy source color (src) to destination color (dst)
	 * @note  both colors should be of same type (e.g. both FGLL_
	 *        or both BGLL_ colors)
	 */
	#define CONOUT_LL_CPYCLR( dst, src )	\
		strncpy( (dst), (src), CONOUT_LL_CLRSZ-1 )


	/* ANSI LowLevel Macros (NOT intended for end-users) ---------- */

	#define CONOUT_LL_RESET()                                       \
	do {                                                            \
		if ( !g_conout.isInited ) {                             \
			errPUTS( "*** CONOUT_RESET(): The interface is NOT inited! ***"); \
			break;                                          \
		}                                                       \
		printf( "%s", "\033[0m" );                              \
		fflush( stdout );                                       \
	} while(0)


	#define CONOUT_LL_SET_COLOR( fg )                               \
	do {                                                            \
		ConColor_T tryFg;                                       \
		                                                        \
		if ( !g_conout.isInited ) {                             \
			errPUTS( "*** CONOUT_SET_FG(): The interface is NOT inited! ***"); \
			break;                                          \
		}                                                       \
		                                                        \
		if ( CONOUT_LL_SAMECLR((fg), FGLL_NOCHANGE) ) {         \
			CONOUT_LL_CPYCLR(tryFg, g_conout.curFg);        \
		}                                                       \
		else {                                                  \
			CONOUT_LL_CPYCLR(tryFg, (fg));                  \
		}                                                       \
		                                                        \
		if ( !VALID_FGLL(tryFg) ) {                             \
			errPRINTF(                                      \
				"*** CONOUT_SET_FG() failed: Invalid fg color '%s' ***\n",\
				tryFg                                   \
				);                                      \
			break;                                          \
		}                                                       \
		                                                        \
		printf( "%s", tryFg );                                  \
		fflush( stdout );                                       \
		CONOUT_LL_CPYCLR(g_conout.curFg, tryFg);                \
		CONOUT_LL_SET_FG_LABEL();                               \
	} while(0)


	#define CONOUT_LL_ADD_COLOR( bg )                               \
	do {                                                            \
		ConColor_T tryBg;                                       \
		                                                        \
		if ( !g_conout.isInited ) {                             \
			errPUTS( "*** CONOUT_SET_BG(): The interface is NOT inited! ***");\
			break;                                          \
		}                                                       \
		                                                        \
		if ( CONOUT_LL_SAMECLR(bg, BGLL_NOCHANGE) ) {           \
			CONOUT_LL_CPYCLR(tryBg, g_conout.curBg);        \
		}                                                       \
		else {                                                  \
			CONOUT_LL_CPYCLR(tryBg, (bg));                  \
		}                                                       \
		                                                        \
		if ( !VALID_BGLL(tryBg) ) {                             \
			errPRINTF(                                      \
				"*** CONOUT_SET_BG() failed: Invalid bg color '%s' ***\n", \
				tryBg                                   \
				);                                      \
			break;                                          \
		}                                                       \
		                                                        \
		printf( "%s", tryBg );                                  \
		fflush( stdout );                                       \
		CONOUT_LL_CPYCLR(g_conout.curBg, tryBg);                \
		CONOUT_LL_SET_BG_LABEL();                               \
	} while(0)


	/* !!! Experimental (START) !!! */

	#define CONOUT_LL_SET_FG_LABEL()                                     \
	do {                                                                 \
		/* nochange */                                               \
		if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_NOCHANGE) ) {    \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"FG_NOCHANGE",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* default */                                                \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DEFAULT) ){ \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"FG_DEFAULT",                                \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* white */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_WHITE) ) {  \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"white",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* gray, darkgray */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_GRAY) ) {   \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"grey",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKGRAY) ){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark grey",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* red, darkred */                                           \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_RED) ) {    \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"red",                                       \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKRED) ){ \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark red",                                  \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* green, darkgreen */                                       \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_GREEN) ) {  \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"green",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKGREEN)){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark green",                                \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* blue, darkblue */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_BLUE) ) {   \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"blue",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKBLUE) ){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark blue",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* yellow, darkyellow */                                     \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_YELLOW) ) { \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"yellow",                                    \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKYELLOW)){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark yellow",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* magenta, darkmagenta */                                   \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_MAGENTA) ) {\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"magenta",                                   \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if (CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKMAGENTA)){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark magenta",                              \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* cyan, darkcyan */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_CYAN) ) {   \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"cyan",                                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_DARKCYAN) ){\
			strncpy(                                             \
				g_conout.fg_label,                           \
				"dark cyan",                                 \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* black */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curFg, FGLL_BLACK) ) {  \
			strncpy(                                             \
				g_conout.fg_label,                           \
				"black",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
	} while(0)

	#define CONOUT_LL_SET_BG_LABEL()                                     \
	do {                                                                 \
		/* nochange */                                               \
		if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_NOCHANGE) ) {    \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"BG_NOCHANGE",                               \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* default */                                                \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DEFAULT) ){ \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"BG_DEFAULT",                                \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* white, gray, darkgray */                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_WHITE) ) {  \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"white/grey/dark grey",                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_GRAY)       \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKGRAY)          \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"white/grey/dark grey",                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* black */                                                  \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_BLACK) ) {  \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"black",                                     \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* red, darkred */                                           \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_RED)        \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKRED)           \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"red/dark red",                              \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* green, darkgreen */                                       \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_GREEN)      \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKGREEN)         \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"green/dark green",                          \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* blue, darkblue */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_BLUE)       \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKBLUE)          \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"blue/dark blue",                            \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* yellow, darkyellow */                                     \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_YELLOW)     \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKYELLOW)        \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"yellow/dark yellow",                        \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* magenta, darkmagenta */                                   \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_MAGENTA)    \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKMAGENTA)       \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"magenta/dark magenta",                      \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
		/* cyan, darkcyan */                                         \
		else if ( CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_CYAN)       \
		|| CONOUT_LL_SAMECLR(g_conout.curBg, BGLL_DARKCYAN)          \
		){                                                           \
			strncpy(                                             \
				g_conout.bg_label,                           \
				"cyan/dark cyan",                            \
				CONOUT_LL_MAXLEN_ColorLABEL-1                \
				);                                           \
		}                                                            \
	} while(0)

	/* !!! Experimental (END) !!! */



#endif /* if (CON_COLORMODE == WIN32) elif (CON_COLORMODE == ANSI) */

#endif /* if (CON_COLORMODE == UNSUPPORTED) @@@@@@@@@@@@@@@@@@@@@@@@@@ */


#endif /* END OF INCLUSION GUARD ===================================== */
