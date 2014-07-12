/*****************************************************//**
 * @file	con_color.h
 * @version	1.04 (h3) alpha
 * @date	Jun 23, 2014
 * @author 	migf1 <mig_f1@hotmail.com>
 * @par Language:
 *		ISO C (C99)
 *
 * @brief	This file provides the public, high-level
 *		implementation of the CON_COLOR interface.
 *********************************************************
 */

/* ***************************************************************** *
 * CONsole COLOR v1.04 - Jun 23, 2014
 * Cross-platform console/terminal text-color interface, for the C/C+
 * pre-processor.
 *
 * AUTHOR:
 *     migf1 <mig_f1@hotmail.com> | <http://x-karagiannis.gr/prg/>
 *
 * LICENSE:
 *    I'm not very fond of license formalities for free stuff.
 *    So, I'm only asking you for 3 simple things:
 *    a)  Mention my name (and email) and include the original file,
 *        if you decide to re-distribute it, modified or not.
 *    b)  Always keep it free when re-distributing this file or
 *        any derivatives of it.
 *    c)  Please let me know of any improvements you make, so I can
 *        include them, along with your name, on the website.
 *
 * PLATFORM:
 *    The interface should be run fine on the console of any Windows version
 *    (tested on XP, Vista/7) along with any ANSI aware terminal (tested on
 *    Cygwin, Ubuntu and FreeBSD).
 *
 * USAGE:
 *    1.  Call CONOUT_INIT() at the start of your program to initialize the
 *        interface.
 *    2.  Optionally define needed foreground & background colors using the
 *        custom data type: ConColors.
 *        Then call twice : CONOUT_CPYCLR( dst, src ) to set the colors.
 *        For example:
 *        ...
 *        ConColors textColors;
 *        CONOUT_CPYCLR( textColors.fg, FG_WHITE );
 *        CONOUT_CPYCLR( textColors.bg, BG_RED );
 *        ... 
 *    3a. Call CONOUT_PRINTF(fg, bg, format, ...) to print text in fg/bg colors.
 *        Avoid changing the line when printing in color-mode.
 *        For example:
 *        ...
 *        CONOUT_PRINTF( textColors.fg, textColors.bg, "%s", "white on red" );
 *        // or directly: CONOUT_PRINTF(FG_WHITE, BG_RED, "%s", "white on red");
 *        putchar( '\n' );    // change line in non-color mode
 *        ...
 *    3b. Use CONOUT_NPRINTF(n, fg, bg, format, ...) instead of CONOUT_PRINTF()
 *        if you want n to reflect the number of chars successfully printed.
 *    4.  Use CONOUT_RESTORE() at the end of your program to restore the colors
 *        your console/terminal had before your program.
 *
 * HELLO WORLD example:
 *        #include <stdio.h>
 *        #include "con_color.h"
 *        int main( void )
 *        {
 *             CONOUT_INIT();
 *             CONOUT_PRINTF( FG_RED, BG_WHITE, "%s", "Hello world!" );
 *             putchar( '\n' );
 *             CONOUT_RESTORE();
 *             return 0;
 *        }
 * ***************************************************************** *
 */

#ifndef CON_COLORMODE_H  /* START OF INCLUSION GUARD ================== */
#define CON_COLORMODE_H


/* =======================================================================
 * High-Level System Constants (PLEASE READ BELOW )
 * =======================================================================
 */

/**
 * @defgroup SYS_CONSTANTS SYSTEM CONSTANTS
 *
 * @brief
 *	The interface uses internally a rather primitive system detection
 *	mechanism, via two groups of System Constants: @ref SYS_CONSTS_PLAT
 *	and @ref SYS_CONSTS_COLORMODE. As their name suggest, the former
 *	identify platforms, while the latter identify color-modes....
 *
 * @details
 *	During system auto-detection at compile time, a hidden constant
 *	called \b CON_PLATFORM gets defined with its value being one of
 *	the Platform Constants. Likewise, a hidden constant called
 *	\b CON_COLORMODE gets defined, with its value being one of the
 *	Color Mode Constants.
 *	\n\n
 *	Under most circumstances the end programmer does \b not need to
 *	deal with any of those constants at all. There are however some
 *	exceptions, mostly regarding x-platform debugging.
 *	\n\n
 *	Please read the relative sections for more information
 *	and coding samples.
 * @{
 */

/*********************************************************************//**
 * @defgroup SYS_CONSTS_PLAT Platform Constants
 *
 * @brief
 *	In most cases, <b>Platform Constants</b> are \b not used at all
 *	by the end programmers. Their main purpose is to provide valid
 *	values for an otherwise  <b>hidden constant</b>, called
 *	\b CON_PLATFORM....
 *
 * @details 	
 *	This hidden constant gets defined with one of these values at
 *	compile time, when the interface tries to \b auto-detect the
 *	compilation platform.
 *	\n\n
 *	However, there are at least 2 cases where those constants
 *	(including the hidden one) may be useful.
 *	\n
 *
 *	<ol>
 *		<li>
 *		You may want to take advantage of CON_COLOR.H's
 *		auto-detected platform, if for example some code of yours
 *		need different implementations for different platforms.
 *		In that case, you may compare the value of \b CON_PLATFORM
 *		against any of the available Platform Constants, in order
 *		to determine the currently auto-detected platform.
 *		\n
 *
 *		The current value of CON_PLATFORM is also returned by the
 *		macro \b CONSYS_GET_PLATFORM(), which may make your code
 *		a bit more readable. Feel free to use either one in your
 *		comparisons.
 *		\n
 *
 *		The following code snippets demonstrate 2 possible ways
 *		of using the Platform Constants in your programs. Both
 *		snippets fetch the auto-detected platform indirectly,
 *		via the macro (instead of examining directly the hidden
 *		constant)...
 *
 * 		@code
 *		// Using CON_COLOR.H's Platform Constants in the pre-processor
 *
 *		#if CONSYS_GET_PLATFORM() == CON_PLATFORM_WINDOWS
 *			// implement windows code here
 *
 *		#elif CONSYS_GET_PLATFORM() == CON_PLATFORM_LINUX
 *			// implement linux code here
 *
 *		#elif CONSYS_GET_PLATFORM() == CON_PLATFORM_UNIX
 *			// implement unix code here
 *
 *		#else
 *			// general implementation goes here
 *
 *		#endif
 *		@endcode
 *
 *		@code
 *      	// Using CON_COLOR.H's Platform Constants in a function
 *
 *		void cls( void ) // clear the console screen
 *		{
 *			int platform = CONSYS_GET_PLATFORM();
 *
 *			if ( CON_PLATFORM_WINDOWS == platform )
 *				system( "cls" );
 *
 *			else if ( CON_PLATFORM_LINUX == platform )
 *				system( "clear" );
 *
 *			else {
 *				for (int i=0; i < 25; i++) {
 *					putchar( '\n' );
 *				}
 *				fflush( stdout );
 *			}
 *		}
 * 		@endcode
 *		</li>
 *
 *		<li>
 *		Another less frequent (and potentially messy) usage of
 *		the Platform Constants is when you want to debug your
 *		code for different configurations. You are given the
 *		option to \b forcefully define the platform, thus bypassing
 *		the default auto-detecting behavior.
 *		\n\n
 *		To do so, you need to \b manually define \b CON_PLATFORM
 *		with one of the pre-defined Platform Constans, \b before
 *		including the \b "con_color.h" header file in your code. 
 *		\n\n
 *		For example...
 *
 *		@code
 *		#include <stdio.h>
 *		...
 *		#define CON_PLATFORM	CON_PLATFORM_UNIX
 *		#include "con_color.h"
 *		...
 *		@endcode
 *		
 *		The above code will forcefully define UNIX as the
 *		active compilation platform. This will influence
 *		CON_COLOR.H's behavior too, and it will most probably...
 *		bite you, unless of course you are already working
 *		on a Unix platform.
 *		\n
 *
 *		@note Please be extremely cautious when forcefully by-passing the
 *		platform auto-detection procedure. This means that you should
 *		check, double-check and triple-check the results after such an
 *		action.
 *		\n\n
 *		Also note that, if you define CON_PLATFORM with a value other than
 *		one of the pre-defined Platform Constants, it will get auto-adjusted
 *		to CON_PLATFORM_UNKNOWN. 
 *		</li>
 *	</ol>
 * @{
 */

/* Platform Constants */
#define CON_PLATFORM_UNKNOWN  0
#define CON_PLATFORM_WINDOWS  1
#define CON_PLATFORM_LINUX    2
#define CON_PLATFORM_UNIX     3

/**@}*/	/* end of SYS_CONSTS_PLAT doxygen sub-group */

/*********************************************************************//**
 * @defgroup SYS_CONSTS_COLORMODE Color Mode Constants
 *
 * @brief
 *	Just like @ref SYS_CONSTS_PLAT may be used to forcefully define the
 *	active compilation platform, the <b>Color Mode Constants</b> may be
 *	used to forcefully define which color mode will be used....
 * @details
 *	Normally the color mode gets auto-detected at compile time, and the
 *	result is internally defined as an otherwise <b>hidden constant</b>,
 *	called \b CON_COLORMODE.
 * 	The value of this hidden constant is one of the pre-defined Color Mode
 *	Constants.
 *	\n
 *
 *	Generally it's not really a good idea to re-define CON_COLORMODE, nor
 *	is to by-pass the auto-detection routine of the interface. But if you
 *	really need to, you can \b manually define CON_COLORMODE  \b before
 *	including the \b "con_color.h" header in your code...
 *	\n
 *
 *	For example...
 *
 *	@code
 *	#include <stdio.h>
 *	...
 *	#define CON_COLORMODE	CON_COLORMODE_ANSI
 *	#include "con_color.h"
 *		...
 *	@endcode
 *
 *	The above example will force CON_COLOR.H to use ANSI escape
 *	sequences for outputting colors to the console.
 *	\n
 *
 *	Now that's wrong for Windows platforms, because their default console
 *	cannot  interpret ANSI escape sequences, unless some external app or
 *	service is run (like <a target="_blank" href="http://adoxa.3eeweb.com/ansicon/">ansicon.exe</a>
 *	for example... back in the DOS days, this was mostly achieved via the
 *	<a target="_blank" href="http://en.wikipedia.org/wiki/ANSI.SYS">ANSI.SYS</a>
 *	device driver).
 *	\n
 *
 *	Note that the ANSI color mode produces less background colors
 *	(8 instead of 16) compared to the \b CON_COLORMODE_WIN32 color
 *	mode. This is not a limitation of this interface, it's a limitation
 *	of the minimum set of standardized ANSI escape sequences (some
 *	terminals may use extra sequences for producing more bg colors, but
 *	they are not guaranteed to work on other terminals).
 *	So, even on Unix/Linux platforms that support the ANSI color mode
 *	by default, the interface will produce 8 bg colors.
 *	\n
 *
 *	It is often desirable to test your code against a color mode that
 *	differs from the one provided as default by your coding platform.
 *	To do so, your coding platform must give you a way to run a
 *	console/terminal which supports that other color mode.
 *	\n
 *
 *	So, let's assume we are coding on Windows and that we have prepared
 *	via a batch file an alternative console, capable of interpreting
 *	ANSI escape sequences (using ansicon.exe for example).
 *	\n
 *
 *	In this scenario it makes sense to temporarily define CON_COLORMODE
 *	with the value \b CON_COLORMODE_ANSI (as shown in the above example),
 *	and run the code on the alterantive console. It will give us a
 *	pretty good idea of how our program will look on an ANSI enabled
 *	terminal.
 *	\n
 *
 *	Another frequent scenario is when we want to check our program
 *	against platforms that use color modes not supported by CON_COLOR.H.
 *	In such cases, the auto-detection routine falls back to
 *	\b CON_COLORMODE_NOCOLORS, meaning that no colors are used at all.
 *	\n
 *
 *	<pre>
 *	Note that if you define CON_COLORMODE with a value other than one of
 *	the pre-defined @ref SYS_CONSTS_COLORMODE, it will get auto-adjusted
 *	to CON_COLORMODE_NOCOLORS.
 *	</pre>
 *
 *	So, in order to test our code against unsupported color modes, we may
 *	temporarily define CON_COLORMODE as following...
 *
 * 	@code
 *	#include <stdio.h>
 *	...
 *	#define CON_COLORMODE	CON_COLORMODE_NOCOLORS
 *	#include "con_color.h"
 *	...
 * 	@endcode
 * @{
 */

/* Color Mode Constants */
#define CON_COLORMODE_NOCOLORS  0
#define CON_COLORMODE_WIN32     1
#define CON_COLORMODE_ANSI      2

/**@}*/		/* end of SYS_CONSTS_COLORMODE doxygen sub-group */

/**@}*/	/* end of SYSTEM CONSTANTS doxygen group */

#include "con_color_private.h" /* low-level implementation of the interface */

/** @cond start doxygen exclusion */

/* =======================================================================
 * Private Global Vars ( !! DO NOT USE DIRECTLY !! )
 * =======================================================================
 */
ConOut g_conout;

/** @endcond */	/* end of doxygen exclusion */

/* =======================================================================
 * High-Level Custom Data Types (optional)
 * =======================================================================
 */
/*********************************************************************//**
 * @defgroup HL_DATA_TYPES HL DATA TYPES
 * 
 * @brief
 *	The interface provides two high-level data-types, which can be
 *	used for defining \b abstractly any foreground and/or background
 *	color variable in your programs....
 *
 * @details
 *	You can always use brutally the pre-defined @ref COLOR_CONSTANTS,
 *	but you'll soon realize that it is getting more and more tedious
 *	to get hold of things, as the project gets bigger.
 *	\n
 *
 *	You need a way to define color variables, so you can easily pass
 *	them around, mostly via functions. The internal data types of the
 *	colors are \b not the same across color-modes (and/or platforms)
 *	so you really need an abstract data type (or two) in order to
 *	handle colors uniformly.
 *	\n
 *
 *	It is crucial to understand that <b>the only way</b> to assign a
 *	value  to a custom defined variable, is via the CONOUT_CPYCLR()
 *	macro. Likewise, <b>the only way</b> to compare a custom defined
 *	color variable against a color constant or another color variable,
 *	is via the CONOUT_SAMECLR() macro.
 *	\n
 *
 *	@note <b>NEVER ASSIGN A COLOR CONSTANT TO A COLOR VARIABLE USING
 *	THE = OPERATOR.\n ALWAYS USE CONOUT_CPYCLR() FOR SUCH TASKS!</b>
 *	\n
 *	\n
 *	<b>ALSO, NEVER COMPARE COLOR VARIABLES DIRECTLY WITH THE DEFAULT
 *	COMPARISON OPERATORS.\n
 *	ALWAYS USE CONOUT_SAMECLR() FOR SUCH TASKS!</b>
 *	\n
 *
 *	The reason is (one more time) that the internal data types of the
 *	colors are \b not the same across color-modes (and/or platforms).
 *	The assignment operator may work (and currently it will) in WIN32
 *	color mode, because colors are of type \a uint16_t in that mode.
 *	However it will <b>silently fail</b> in ANSI color mode, because
 *	colors are of type <em>char *</em> there. It will not only fail,
 *	but eventually it will seg-fault your program at run-time.
 *	Moreover, its not guaranteed that the internal representation of
 *	the colors will stay the same in future versions of the interface,
 *	but CONOUT_CPYCLR() will always work the same.
 *	\n
 *
 *	@note For exactly the same reason, you should <b>ALWAYS USE THE </b>
 *	<b>PREDEFINED @ref COLOR_CONSTANTS AS THE SECOND ARGUMENT IN THE </b>
 *	<b>CONOUT_CPYCLR() MACRO</b>.
 *	\n
 *
 *	Now that we have cleared that up, we are ready to use either
 *	\b ConSingleColor or \b \a struct ConColors to define custom color
 *	variables in our programs.
 *	The former is used for single color variables, while the latter is
 *	used for fg/bg color pairs, expressed as abstract fields inside a
 *	struct.
 *	\n\n
 * 	Please read the corresponding sections of those data types for
 *	more info, along with code-samples for their proper usage in your
 *	programs.
 * @{
 */

/**
 * @par ConSingleColor
 * @brief 	High level data-type to abstractly define fg and/or bg
 *		single variables.
 * @details	Example of defining & using single color variables\n
 * @include 	con_single_color.c
 */
typedef ConColor_T ConSingleColor;

/**
 * @par typedef struct ConColors {ConSingleColor fg; ConSingleColor bg;} ConColors;
 * @brief  	High level data-type to abstractly define variables
 *		of fg/bg pairs.
 * @details	Example of defining & using color variables in a
 *		structural manner\n
 * @include	con_struct_colors.c
 */
typedef struct ConColors {
	ConSingleColor fg;      /*!< foreground color */
	ConSingleColor bg;      /*!< background color */
}ConColors;

/**@}*/	/* end of HL_DATA_TYPES doxygen group */

/* =======================================================================
 * High-Level Color Constants
 * =======================================================================
 */
/*********************************************************************//**
 * @defgroup COLOR_CONSTANTS COLOR CONSTANTS
 * 
 * @brief
 *	Color Constants are abstract identifiers for the actual colors
 *	being outputted to the console by the interface. In their most
 *	naive usage they are just passed brutally as arguments to macros
 *	which are responsible for producing colored output to the console,
 *	such as CONOUT_PRINTF(), to name the most popular one....
 *
 * @details
 *	The prefix in their naming denotes which ones refer to foreground
 *	colors (\b FG_xxx) and which ones refer to background colors
 *	(\b BG_xxx).
 *	\n
 *
 *	The internal implementation of the actual colors differs across
 *	color modes and/or system platforms (see @ref SYS_CONSTANTS) so
 *	these constants should always & consistently be used by end programmers,
 *	whenever they need to refer to any particular color in their code.
 *	This will ensure that their code will compile successfully in any
 *	of the supported color modes.
 *	\n
 *
 *	There are a few more things you should know, though.
 *	\n
 *
 *	First, not all colors are available in every color-mode.
 *	In the \b ANSI color mode there are <b>only 8 background colors</b>
 *	available, instead of 16 which are available in the WIN32 color mode.
 *	This is a limitation in the minimum standardized set of ANSI escape
 *	sequences. The interface only implements the minimum standardized
 *	set in an effort to support as many platforms as possible (see also
 *	@ref SYS_CONSTS_COLORMODE).
 *	\n
 *
 *	Thus when in ANSI color-mode, the darker background colors are
 *	equivalent to their normal counterparts (e.g BG_RED is the same
 *	with BG_DARKRED).
 *	Moreover, BG_WHITE, BG_GRAY and BG_DARKGRAY are all mapped to
 *	the same color.
 *	\n
 *
 *	@note In order for your code to produce fairly similar results
 *	on all supported color modes, try to use normal colors for the
 *	foreground and dark colors for the background (for example,
 *	FG_YELLOW on BG_DARKBLUE).
 *
 *	Regardless of color-modes, some clarification is needed for the
 *	color constants FG_NOCHANGE, BG_NOCHANGE and FG_DEFAULT, BG_DEFAULT.
 *	\n
 *
 *	One of my initial goals when making the interface was to have it
 *	falling back to the original color-state of the console, after
 *	every high-level colored output it performs. Thus, there is no need
 *	to manually reset the colors every time we want to use any of the
 *	standard output functions (via stdio.h).
 *	\n
 *
 *	This means that after an output macro does its job, the color-state
 *	of the console is automatically reseted back to \b FG_DEFAULT
 *	foreground on \b BG_DEFAULT background.
 *	The end-programmer doesn't need to do it manually before calling
 *	any of the standard output functions.
 *	\n
 *
 *	But this also means that the last fg and bg colors used by the
 *	interface must be saved before resetting the console state back
 *	to its original colors. Those saved fg and bg colors are represented
 *	by the color constants \b FG_NOCHANGE and \b BG_NOCHANGE, respectively.
 *	\n
 *
 *	This proved to be quite tricky to implement, due to the completely
 *	different approaches used for the colors representation by the
 *	WIN32 API and the ANSI escape sequences mechanism.
 *	\n
 *
 *	<h2>Some more technical info</h2>
 *
 *	Contrary to interfaces that deal exclusively with ANSI colors,
 *	CON_COLOR.H does not stack up colors one on top of the other.
 *	This is a quite popular technique when dealing only with ANSI
 *	escape sequences, because they are just strings containing any
 *	combination of fg/bg sub-strings, but only the last one gets
 *	activated, automatically by the terminal).
 *	\n
 *
 *	The WIN32 API on the other hand, treats colors as nibbles in the
 *	low byte of a WORD (a WORD is a 2-byte, unsigned entity). The low
 *	nibble of that byte expresses the fg-color, while the high nibble
 *	expresses the bg-color. One of the nibble bits is used for the
 *	color's intensity (bright when the intensity bit is on, normal
 *	otherwise). The remaining 3 bits contain the RGB value of the color.
 *	So we need to binary OR a fg-color byte with a bg-color byte and
 *	store the result in the lower byte of the WORD used by the API when
 *	outputting colored text on the console.
 *	\n
 *
 *	Another big difference is that the ANSI mechanism does not provide
 *	a standard way to identify the currently active fg and bg colors
 *	of the terminal, not even the default ones. Thus, when running in
 *	ANSI mode, the interface does not know what are the default colors
 *	of the terminal.
 *	\n
 *
 *	Consequently, when calling the macros CONSYS_GET_FG_LABEL() and/or
 *	CONSYS_GET_BG_LABEL() right after having outputted in FG_DEFAULT
 *	and/or BG_DEFAULT colors, you get as a response the generic
 *	c-strings: "FG_DEFAULT" and "BG_DEFAULT", respectively.
 *	\n
 *
 *	Fortunately, the ANSI mechanism does provide a standard way to
 *	reset the terminal back to its defalut colors (but still without
 *	identifying their actual values).
 * @{
 */

/**
 * @name Foreground Color Constants
 * @{
 */
/* foreground color-constants */

#define FG_NOCHANGE    FGLL_NOCHANGE   /*!< @brief Keep the active fg color. */
#define FG_DEFAULT     FGLL_DEFAULT    /*!< @brief The console's default fg color.*/

#define FG_BLACK       FGLL_BLACK      /*!< @brief Black fg color */
#define FG_WHITE       FGLL_WHITE      /*!< @brief White fg color */

#define FG_GRAY        FGLL_GRAY       /*!< @brief Gray fg color */
#define FG_DARKGRAY    FGLL_DARKGRAY   /*!< @brief Dark Gray fg color */

#define FG_RED         FGLL_RED        /*!< @brief Red fg color */
#define FG_DARKRED     FGLL_DARKRED    /*!< @brief Dark Red fg color */

#define FG_GREEN       FGLL_GREEN      /*!< @brief Green fg color */
#define FG_DARKGREEN   FGLL_DARKGREEN  /*!< @brief Dark Green fg color */

#define FG_BLUE        FGLL_BLUE       /*!< @brief Blue fg color */
#define FG_DARKBLUE    FGLL_DARKBLUE   /*!< @brief Dark Blue fg color */

#define FG_YELLOW      FGLL_YELLOW     /*!< @brief Yellow fg color */
#define FG_DARKYELLOW  FGLL_DARKYELLOW /*!< @brief Dark Yellow fg color */

#define FG_MAGENTA     FGLL_MAGENTA    /*!< @brief Magenta fg color */
#define FG_DARKMAGENTA FGLL_DARKMAGENTA/*!< @brief Dark Magenta fg color */

#define FG_CYAN        FGLL_CYAN       /*!< @brief Cyan fg color */
#define FG_DARKCYAN    FGLL_DARKCYAN   /*!< @brief Dark Cyan fg color */

/**@}*/	/* end of Foreground Color Constants doxygen sub-group */

/**
 * @name Background Color Constants
 * @{
 */
/* background color-constants */

#define BG_NOCHANGE    BGLL_NOCHANGE   /*!< @brief Keep the active bg color. */
#define BG_DEFAULT     BGLL_DEFAULT    /*!< @brief The console's default bg color. */

#define BG_BLACK       BGLL_BLACK      /*!< @brief Black bg color */
#define BG_WHITE       BGLL_WHITE      /*!< @brief White bg color (BG_GRAY in ANSI)*/

#define BG_GRAY        BGLL_GRAY       /*!< @brief Gray bg color (BG_DARKGRAYin ANSI) */
#define BG_DARKGRAY    BGLL_DARKGRAY   /*!< @brief Dark Gray bg color */

#define BG_RED         BGLL_RED        /*!< @brief Red bg color (BG_DARKRED in ANSI) */
#define BG_DARKRED     BGLL_DARKRED    /*!< @brief Dark Red bg color */

#define BG_GREEN       BGLL_GREEN      /*!< @brief Green bg color (BG_DARKGREEN in ANSI)*/
#define BG_DARKGREEN   BGLL_DARKGREEN  /*!< @brief Dark Green bg color */

#define BG_BLUE        BGLL_BLUE       /*!< @brief Blue bg color (BG_DARKBLUE in ANSI)*/
#define BG_DARKBLUE    BGLL_DARKBLUE   /*!< @brief Dark Blue bg color */

#define BG_YELLOW      BGLL_YELLOW     /*!< @brief Yellow bg color (BG_DARKYELLOW in ANSI)*/
#define BG_DARKYELLOW  BGLL_DARKYELLOW /*!< @brief Dark Yellow bg color */

#define BG_MAGENTA     BGLL_MAGENTA    /*!< @brief Magenta bg color (BG_DARKMAGENTA in ANSI)*/
#define BG_DARKMAGENTA BGLL_DARKMAGENTA/*!< @brief Dark Magenta bg color */

#define BG_CYAN        BGLL_CYAN       /*!< @brief Cyan bg color (BG_DARKCYAN in ANSI)*/
#define BG_DARKCYAN    BGLL_DARKCYAN   /*!< @brief Dark Cyan bg color */

/**@}*/	/* end of Background Color Constants doxygen sub-group */

/**
 * @name Linguistics Overhead
 * @{
 */
/* some linguistics overhead */

#define FG_GREY         FG_GRAY        /*!< @brief FG_GRAY alias */
#define FG_DARKGREY     FG_DARKGRAY    /*!< @brief FG_DARKGRAY alias */
#define BG_GREY         BG_GRAY        /*!< @brief BG_GRAY alias */
#define BG_DARKGREY     BG_DARKGRAY    /*!< @brief BG_GRAY alias */

/**@}*/	/* end of Linguistics Overhead doxygen sub-group */

/** @}*/ /* end of COLOR_CONSTANTS doxygen group */

/**
 * @defgroup PUBLIC_MACROS MACROS
 *
 * @brief
 *	The public macros is a collection of abstract operations on
 *	internally stored objects, data types, constants, directives,
 *	and other low level implementations, which are privately kept
 *	in a separate header file, called: con_color_private.h....
 *
 * @details
 *	End programmers do not need to worry about any of those private
 *	stuff. The publicly exposed constants, data-types and macros are
 *	all they need in order to focus on productivity, instead of
 *	house-keeping.
 *	\n
 *
 *	This section provides documentation for the public macros, which
 *	are grouped in small categories, depending on their type, on their
 *	actions, or on any other common characteristic they may share.
 *	\n
 *
 *	Regardless of their presentation grouping, the prefixes in the
 *	names of the macros give a hint about their primary purpose.
 *	Most of them have the \b CONOUT_ prefix in their names because
 *	their primary task is to output something to the console.
 *	\n
 *
 *	Macros that deal directly with the internal system have
 *	a \b CONSYS_ prefix in their names. For example the macro
 *	CONSYS_GET_FG_LABEL() returns the currently active foreground
 *	color as a c-string, so a CONOUT_ prefix wouldn't make
 *	much sense for this macro. CONSYS_ is much more descriptive.
 * @{
 */

/* =======================================================================
 * Backwards Compatible High-Level Macros
 * =======================================================================
 */
/*********************************************************************//**
 * @defgroup ORIGINAL_MACROS Original Macros
 *
 * @brief
 *	As the group name suggests, these are the macros that consisted
 *	the initial (original) version of the CON_COLOR.H interface....
 *
 * @details
 *	They are common across all versions (new and old) keeping
 *	exactly the same signatures and behaviours. There are only
 *	5 of them, and they come down to just 3 if we take out
 *	CONOUT_INIT() and CONOUT_RESTORE() which are called just
 *	once in every program.
 *	\n
 *
 *	You may be surprised with how much you can accomplish by using
 *	just three little macros. For example, have a look at
 * 	<a target="_blank" href="http://x-karagiannis.gr/prg/c-prog/c-misc/hexview/">CC HexView</a>.
 *	It is an open-source, freeware, Cross-platform Console Hex Viewer
 *	that among other things features skins and syntax highlighting. 
 *	It uses exclusively the original con_color.h v1.00 header file,
 *	for all its colored output (the file can be found in the sources
 *	of CC HexView).
 * @{
 */

/*********************************************************************//**
 * @par CONOUT_INIT()
 * @brief	Initialize the interface.
 * @details	This macro must be called at the start of the program,
 *		just once.
 *		Among other things, it saves the original color-state
 *		of the console, which should be restored with CONOUT_RESTORE()
 *		before program termination.
 * @note	If this macro has not been called at least once, no other
 *		macro will work (they will output error messages complaining
 *		that the interface has not been initialized).
 */
#define CONOUT_INIT() \
	CONOUT_LL_INIT()

/*********************************************************************//**
 * @par CONOUT_RESTORE()
 * @brief	Restore console's original color-state & Update the interface.
 * @details	You should call this macro just before exiting your program,
 *		in order to restore the original state of the console (it
 *		gets saved with CONOUT_INIT() at the start of your program).
 */
#define CONOUT_RESTORE() \
	CONOUT_LL_RESTORE()

/*********************************************************************//**
 * @par CONOUT_PRINTF(fg, bg, ...)
 * @brief 	This is a colored wrapper of the standard function printf(),
 *		taking 2 extra arguments at the front.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param ...	The remaining arguments are the same as in the standard
 *		function printf().
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_NPRINTF()
 */
#define CONOUT_PRINTF(fg, bg, ...) \
	CONOUT_LL_PRINTF((fg), (bg), __VA_ARGS__)

/*********************************************************************//**
 * @par CONOUT_NPRINTF(n, fg, bg, ...)
 * @brief 	This is a colored wrapper of the standard function printf(),
 *		taking 3 extra arguments at the front.
 * @param	(int)(n)             The return value of printf().
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param ...	The remaining arguments are the same as in the standard
 *		function printf().
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_PRINTF()
 */
#define CONOUT_NPRINTF(n, fg, bg, ...) \
	CONOUT_LL_NPRINTF((n), (fg), (bg), __VA_ARGS__)


/*********************************************************************//**
 * @par CONOUT_CPYCLR(dst, src)
 * @brief 	Copy a source color into a destination color.
 * @param 	(ConSingleColor)(dst)  The color to copy into (destination).
 * @param 	(ConSingleColor)(src)  The color to copy from (source).
 * @note	Both colors should be of the same type, that is
 *		both foreground (FG_xxx) or both background (BG_xxx) colors.
 * @sa		CONOUT_SAMECLR()
 */
#define CONOUT_CPYCLR(dst, src) \
	CONOUT_LL_CPYCLR((dst), (src))

/** @}*/ /* end of ORIGINAL_MACROS doxygen sub-group */

/* =======================================================================
 * New High-Level Macros (since version 1.04)
 * =======================================================================
 */
/*********************************************************************//**
 * @defgroup NEW_MACROS New Macros (since v1.04)
 *
 * @brief	These are the macros that were added in version 1.04....
 *
 * @details	
 * @{
 */

/*********************************************************************//**
 * @par CONOUT_SAMECLR(cl1, clr1)
 * @brief  	Compare two colors.
 * @param 	(ConSingleColor)(clr1)	The first color to be compared.
 * @param 	(ConSingleColor)(clr2)	The second color to be compared.
 * @return 	1 (true) if colors are the same, 0 (false) otherwise.
 * @note   	Both colors should be of same type (e.g. both FG_xxx
 *		or both BG_xxx colors).
 */
#define CONOUT_SAMECLR(clr1, clr2) \
	CONOUT_LL_SAMECLR( (clr1), (clr2) )


/*********************************************************************//**
 * @par CONOUT_PUTCHAR(fg, bg, c)
 * @brief	This is a colored wrapper of the standard function putchar(),
 *		taking 2 extra arguments at the front.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param	(int)(c)             The character to be ouputted.
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 */
#define CONOUT_PUTCHAR(fg, bg, c) \
	CONOUT_LL_PUTCHAR((fg), (bg), (c))

/*********************************************************************//**
 * @par CONOUT_PUTCHAR_NTIMES(fg, bg, c, ntimes)
 * @brief  	Repetitive, colored output of a character to the stdin.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param	(int)(c)             The character to be ouputted.
 * @param	(int)(ntimes)        The count of desired repetitions.
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 */
#define CONOUT_PUTCHAR_NTIMES(fg, bg, c, ntimes) \
	CONOUT_LL_PUTCHAR_NTIMES( (fg), (bg), (c), (ntimes) )

/*********************************************************************//**
 * @par    CONOUT_PUTS( fg, bg, s)
 * @brief	This is a colored wrapper of the standard function puts(),
 *		taking 2 extra arguments at the front.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param	(const char *)(s)    The c-string to be outputted.
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 */
#define CONOUT_PUTS(fg, bg, s) \
	CONOUT_LL_PUTS((fg), (bg), (s))

/*********************************************************************//**
 * @par CONOUT_WPRINTF(fg, bg, ...)
 * @brief 	This is a colored wrapper of the standard function wprintf(),
 *		taking 2 extra arguments at the front.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param ...	The remaining arguments are the same as in the standard
 *		function wprintf().
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_NWPRINTF(), CONOUT_PRINTF(), CONOUT_NPRINTF()
 */
#define CONOUT_WPRINTF(fg, bg, ...) \
	CONOUT_LL_WPRINTF((fg), (bg), __VA_ARGS__)

/*********************************************************************//**
 * @par CONOUT_NWPRINTF(n, fg, bg, ...)
 * @brief 	This is a colored wrapper of the standard function wprintf(),
 *		taking 3 extra arguments at the front.
 * @param	(int)(n)             The return value of wprintf().
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param ...	The remaining arguments are the same as in the standard
 *		function wprintf().
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_WPRINTF(), CONOUT_PRINTF(), CONOUT_NPRINTF()
 */
#define CONOUT_NWPRINTF(n, fg, bg, ...) \
	CONOUT_LL_NWPRINTF((n), (fg), (bg), __VA_ARGS__)

/*********************************************************************//**
 * @par CONOUT_PUTWCHAR(fg, bg, wc)
 * @brief	This is a colored wrapper of the standard function putwchar(),
 *		taking 2 extra arguments at the front.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param	(wchar_t)(wc)        The wide character to be ouputted.
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_PUTWCHAR_NTIMES(), CONOUT_PUTCHAR(), CONOUT_PUTCHAR_NTIMES()
 */
#define CONOUT_PUTWCHAR(fg, bg, wc) \
	CONOUT_LL_PUTWCHAR((fg), (bg), (wc))

/*********************************************************************//**
 * @par CONOUT_PUTWCHAR_NTIMES(fg, bg, wc, ntimes)
 * @brief  	Repetitive, colored output of a wide character to the stdin.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param	(wchar_t)(wc)        The wide character to be ouputted.
 * @param	(int)(ntimes)        The count of desired repetitions.
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_PUTWCHAR(), CONOUT_PUTCHAR(), CONOUT_PUTCHAR_NTIMES()
 */
#define CONOUT_PUTWCHAR_NTIMES(fg, bg, wc, ntimes) 	\
		CONOUT_LL_PUTWCHAR_NTIMES( (fg), (bg), (wc), (ntimes) )

/*********************************************************************//**
 * @par    CONOUT_PUTWS(fg, bg, ws)
 * @brief	Output a wide c-string to the stdin, with the specified
 *		foreground and background colors.
 * @param	(ConSingleColor)(fg) The desired foreground color for the output.
 * @param	(ConSingleColor)(bg) The desired background color for the output.
 * @param	(const wchar_t*)(ws) The wide c-string to be outputted.
 * @note   	Make sure (fg) comes always \a before (bg), otherwise the
 *		macro will produce an error message, complaining about
 *		invalid fg and/or bg color.
 * @sa		CONOUT_PUTS()
 */
#define CONOUT_PUTWS(fg, bg, ws) \
	CONOUT_LL_PUTWS((fg), (bg), (ws))

/** @}*/ /* end of NEW_MACROS doxygen sub-group */

/* =======================================================================
 * Release Candidate High-Level Macros (they may change)
 * =======================================================================
 */
/*********************************************************************//**
 * @defgroup RC_MACROS Release Candidate Macros
 *
 * @brief	These are macros which are still under testing, but they
 *		are getting close to be officially added to the interface....
 *
 * @details	Most of them are fairly tested, but use them with a pinch
 *		of salt. It is not guranteed that they will finally make it
 *		to get released. They may even released under different names
 *		and/or behavior.
 *
 * @{
 */

/*********************************************************************//**
 * @par CONSYS_GET_COLORMODE()
 * @brief	Get the currently active color mode.
 * @details	This macro is provided mostly for debugging.
 *		Please follow the link provided below for more details.
 * @return 	The active color-mode, expressed as one of
 *		the pre-defined @ref SYS_CONSTS_COLORMODE.
 * @note   	RC104 (still experimental, but fairly tested).
 */
#define CONSYS_GET_COLORMODE() \
	CONSYS_LL_GET_COLORMODE()

/*********************************************************************//**
 * @par CONSYS_GET_PLATFORM()
 * @brief	Get the currently active compilation platform.
 * @details	This macro is provided mostly for debugging. You may also
 *		use it on any project if you need to know what platform
 *		your code was compiled on. Please follow the link provided
 *		below, for more details and coding samples.
 * @return 	The active compilation platform, expressed
 *		as one of the pre-defined @ref SYS_CONSTS_PLAT.
 * @note   	RC104 (still experimental, but fairly tested).
 */
#define CONSYS_GET_PLATFORM() \
	CONSYS_LL_GET_PLATFORM()

/*********************************************************************//**
 * @par CONSYS_GET_COLORMODE_LABEL()
 * @brief	Get a readable label for the active color-mode.
 * @details	Use this macro when you need a readable c-string describing
 *		the active color-mode.
 * @return 	The currently active color-mode as an immutable c-string:
 *		(const char *)
 * @remarks	Do not attempt to directly alter the contents of the
 *		returned c-string, or your program will seg-fault at run-time.
 * @note   	Still experimental, but fairly tested.
 */
#define CONSYS_GET_COLORMODE_LABEL() \
	CONSYS_LL_GET_COLORMODE_LABEL()

/*********************************************************************//**
 * @par CONSYS_GET_PLATFORM_LABEL()
 * @brief	Get a readable label for the active compilation platform.
 * @details	Use this macro when you need a readable c-string describing
 *		the active platform.
 * @return 	The currently active platform as an immutable c-string:
 *		(const char *)
 * @remarks	Do not attempt to directly alter the contents of the
 *		returned c-string, or your program will seg-fault at run-time.
 * @note   	RC104 (still experimental, but fairly tested).
 */
#define CONSYS_GET_PLATFORM_LABEL() \
	CONSYS_LL_GET_PLATFORM_LABEL()

/*********************************************************************//**
 * @par CONSYS_GET_FG_LABEL()
 * @brief	Get a readable label for the currently active foreground color.
 * @details	Use this macro when you need a readable c-string describing
 *		the active fg-color. "Active" means the color that will be
 *		applied to any colored output that specifies the fg-color
 *		as FG_NOCHANGE. Please read the @ref COLOR_CONSTANTS section
 *		for details about how this interface "computes" the active
 *		fg-color.
 * @return 	The currently active fg-color as an immutable c-string:
 *		(const char *)
 * @remarks	Do not attempt to directly alter the contents of the
 *		returned c-string, or your program will seg-fault at run-time.
 * @note   	RC104 (still experimental, but fairly tested).
 */
#define CONSYS_GET_FG_LABEL() \
	CONSYS_LL_GET_FG_LABEL()

/*********************************************************************//**
 * @par CONSYS_GET_BG_LABEL()
 * @brief	Get a readable label for the currently active background color.
 * @details	Use this macro when you need a readable c-string describing
 *		the active bg-color. "Active" means the color that will be
 *		applied to any colored output that specifies the bg-color
 *		as BG_NOCHANGE. Please read the @ref COLOR_CONSTANTS section
 *		for details about how this interface "computes" the active
 *		bg-color.
 * @return 	The currently active bg-color as an immutable c-string:
 *		(const char *)
 * @remarks	Do not attempt to directly alter the contents of the
 *		returned c-string, or your program will seg-fault at run-time.
 * @note   	RC104 (still experimental, but fairly tested).
 */
#define CONSYS_GET_BG_LABEL() \
	CONSYS_LL_GET_BG_LABEL()

/*********************************************************************//**
 * @par CONOUT_PAINT(bg)
 * @brief 	Print an empty space char in the specified background color.
 * @param	(ConSingleColor)(bg)	The background color to be used.
 * @details	Outputting an empty char on a colored background, gives
 *		a paint-like effect at the position of the cursor.
 * @remarks	In a no-colors environment (see @ref SYS_CONSTS_COLORMODE)
 *		the macro actually outputs the '-' character, thus still
 *		producing a paint-like effect.
 * @note   	RC104 (still experimental, but fairly tested).
 *		\n
 *		Make sure that (bg) corresponds to a valid bg-color,
 *		otherwise the macro will produce an error message.
 * @sa		CONOUT_PAINT_NTIMES()
 */
#define CONOUT_PAINT(bg) \
	CONOUT_LL_PAINT( (bg) )

/*********************************************************************//**
 * @par    CONOUT_PAINT_NTIMES(bg, n)
 * @brief 	Print repetitively empty space chars in the specified
 *		background color.
 * @param	(ConSingleColor)(bg) The background color to be used.
 * @param	(int)(n)             The count of desired repetitions.
 * @details	By outputting repetitively empty chars on a colored
 *		background, you get a paint-like effect starting at
 *		the position of the cursor.
 *		\n
 *		This macro actually uses the CONOUT_PAINT() macro.
 * @remarks	In a no-colors environment (see @ref SYS_CONSTS_COLORMODE)
 *		the macro actually outputs '-' characters, thus still
 *		producing a paint-like effect.
 * @note   	RC104 (still experimental, but fairly tested).
 *		\n
 *		Make sure that (bg) corresponds to a valid bg-color,
 *		otherwise the macro will produce an error message.
 * @sa		CONOUT_PAINT()
 */
#define CONOUT_PAINT_NTIMES(bg, n) \
	CONOUT_LL_PAINT_NTIMES( (bg), (n) )

/** @}*/ /* end of RC104_MACROS doxygen sub-group */


/** @cond start doxygen exclusion */

/* =======================================================================
 * Experimental High-Level Macros
 * -----------------------------------------------------------------------
 * DO NOT USE THEM, most probably they'll bite you big time.
 * (they are here mostly to make my life a bit easier when debugging the interface)
 * =======================================================================
 */

#define CONOUT_RESET()               CONOUT_LL_RESET()
#define CONOUT_SET_FG(fg)            CONOUT_LL_SET_COLOR( (fg) )
#define CONOUT_SET_BG(bg)            CONOUT_LL_ADD_COLOR( (bg) )

#define CONOUT_SET_FGBG(fg, bg)                          \
	do {                                             \
		CONOUT_SET_FG(fg);                       \
		CONOUT_SET_BG(bg);                       \
	}while(0)

#define CONOUT_GET_FG()              (ConSingleColor)(g_conout.curFg)
#define CONOUT_GET_BG()              (ConSingleColor)(g_conout.curBg)

#define CONOUT_GET_FGBG(getFg, getBg)                    \
	do {                                             \
		CONOUT_CPYCLR( getFg, CONOUT_GET_FG() ); \
		CONOUT_CPYCLR( getBg, CONOUT_GET_BG() ); \
	} while(0)

/**@endcond*/

/** @}*/ /* end of PUBLIC_MACROS doxygen group */


#endif    /* END OF INCLUSION GUARD =================================== */
