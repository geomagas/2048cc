#define GUI_LOCALE_C

/* gettext related constants */
#define GETTEXT_PACKAGE  "2048cc_gtk2_viewer" /* our translation text-domain */
#define LOCALEDIR        "lang"    /* directory to host translations */
#define OUTCODESET       "UTF-8"   /* desired output-codeset for translations */

/* */
#include <glib/gi18n.h>
#include <glib.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include "gui_locale.h"
#include "misc.h"

#define MAXSIZ_LOCALE             (511+1)

#define STR_EN_LOCALE             "en_US.utf8"
#define STR_EN_LOCALE_LANG        "en_US.UTF-8"
#define STR_EN_LOCALE_LANGUAGE    "en"

#define STR_EL_LOCALE             "el_GR.utf8"
#define STR_EL_LOCALE_LANG        "el_GR.UTF-8"
#define STR_EL_LOCALE_LANGUAGE    "el"

struct _GuiLocale
{
	/* i18n info needed by gettext */
	struct {
		const char  *domain;
		const char  *dir;
		const char  *outcodeset;
	} i18n;

	/* os inherited locale environment */
	struct {
		gchar  localeIn[MAXSIZ_LOCALE];
		gchar  *varLang;
		gchar  *varLanguage;  /* GTK+ does not check this on Win32 */
	} os;
};

/* ---------------------------------------------------
 * Print info about GUI's inherited locale environment,
 * only if 'global_debugOn' is TRUE.
 * ---------------------------------------------------
 */
void dbg_gui_locale_print_inherited( const GuiLocale *gl, const gchar *title )
{
	/* sanity check */
	if ( !gl ) {
		return;
	}

	if ( title ) {
		dbg_print_info( "%s\n", title );
	}
	dbg_print_info( "locale:\t\t%s\n", gl->os.localeIn );
	dbg_print_info( "LANG:\t\t%s\n", gl->os.varLang );
	dbg_print_info( "LANGUAGE:\t%s\n", gl->os.varLanguage );
	dbg_print_info( "%c", '\n' );

}

/* ---------------------------------------------------
 * Print info about the current locale environment,
 * only if 'global_debugOn' is TRUE.
 * ---------------------------------------------------
 */
void dbg_gui_locale_print_current( const GuiLocale *gl, const gchar *title )
{
	/* sanity check */
	if ( !gl ) {
		return;
	}

	if ( title ) {
		dbg_print_info( "%s\n", title );
	}

	dbg_print_info( "locale:\t\t%s\n", setlocale(LC_ALL, NULL) );
	dbg_print_info( "LANG:\t\t%s\n", g_getenv("LANG") );
	dbg_print_info( "LANGUAGE:\t%s\n", g_getenv("LANGUAGE") );
	dbg_print_info( "%c", '\n' );

}

/* ---------------------------------------------------
 * Initialize internationalization via the GNU-gettext library.
 * ---------------------------------------------------
 */
static inline gboolean _init_i18n( GuiLocale *gl )
{
	gl->i18n.domain     = GETTEXT_PACKAGE;
	gl->i18n.dir        = LOCALEDIR;
	gl->i18n.outcodeset = OUTCODESET;

	/* set the current locale to system default & save it */
	setlocale( LC_ALL, "" );

	/* ensure that the translated messages of the specified text-domain
	 * will be searched in the specified locale-directory rather than in
	 * the system locale database
	 */
	if ( !bindtextdomain(gl->i18n.domain, gl->i18n.dir) ) {
		return FALSE;
	}

	/* ensure the specified output-character-set will be used
	 * for the message-catalogs of the specified text-domain
	 */
	if ( !bind_textdomain_codeset(gl->i18n.domain, gl->i18n.outcodeset) ){
		return FALSE;
	}

	/* set the current default text-domain to the specified one */
	if ( !textdomain(gl->i18n.domain) ) {
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
static inline void _save_os( GuiLocale *gl )
{
	gchar *locale = NULL;

	/* save locally the current locale environment from os */

	locale = setlocale( LC_ALL, NULL );
	strncpy( gl->os.localeIn, locale, MAXSIZ_LOCALE-1 );

	gl->os.varLang     = (gchar *) g_getenv("LANG");
	gl->os.varLanguage = (gchar *) g_getenv("LANGUAGE");
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
static inline void _restore_os( const GuiLocale *gl )
{
	setlocale( LC_ALL, gl->os.localeIn );

	if ( g_getenv("LANG") ) {
		g_unsetenv("LANG");
	}
	if ( gl->os.varLang ) {
		g_setenv( "LANG", gl->os.varLang, TRUE );
	}

	if ( g_getenv("LANGUAGE") ) {
		g_unsetenv("LANGUAGE");
	}
	if ( gl->os.varLanguage ) {
		g_setenv( "LANGUAGE", gl->os.varLanguage, TRUE );
	}
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
static inline gboolean _apply(
	const gchar *locale,
	const gchar *varLang,
	const gchar *varLanguage
	)
{
	if ( NULL == setlocale(LC_ALL, locale) ) {
		DBG_GUI_ERRMSG( NULL, "setlocale() failed!" );
		return FALSE;
	}

	if ( g_getenv("LANG") ) {
		g_unsetenv("LANG");
	}
	if ( varLang && !g_setenv("LANG", varLang, TRUE) ) {
		DBG_GUI_ERRMSG( NULL, "g_setenv(\"LANG\") failed!" );
		return FALSE;
	}

	if ( g_getenv("LANGUAGE") ) {
		g_unsetenv("LANGUAGE");
	}
	if ( varLanguage && !g_setenv("LANGUAGE", varLanguage, TRUE) ) {
		DBG_GUI_ERRMSG( NULL, "g_setenv(\"LANGUAGE\") failed!" );
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_locale_apply_en( const GuiLocale *gl )
{
	if ( NULL == gl ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument!");
		return FALSE;
	}

	return _apply(
			(gchar *) STR_EN_LOCALE,
			(gchar *) STR_EN_LOCALE_LANG,
			(gchar *) STR_EN_LOCALE_LANGUAGE
		);
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_locale_apply_el( const GuiLocale *gl )
{
	if ( NULL == gl ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument!");
		return FALSE;
	}

	return _apply(
			(gchar *) STR_EL_LOCALE,
			(gchar *) STR_EL_LOCALE_LANG,
			(gchar *) STR_EL_LOCALE_LANGUAGE
		);
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_locale_apply_osenv( const GuiLocale *gl )
{
	if ( NULL == gl ) {
		DBG_GUI_ERRMSG( NULL, "NULL pointer argument!");
		return FALSE;
	}

	if ( NULL == gl->os.localeIn
	|| NULL == gl->os.varLang
	|| NULL == gl->os.varLang
	){
		DBG_GUI_ERRMSG(
			NULL,
			_("No proper locale environment found.\n\
ENGLISH is used as fallback language.")
		);
		return gui_locale_apply_en( gl );
	}

	return _apply(gl->os.localeIn, gl->os.varLang, gl->os.varLanguage);
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiLocale *gui_locale_free( GuiLocale *gl )
{
	if ( gl ) {
		_restore_os( gl );
		free( gl );
	}
	return NULL;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
GuiLocale *new_gui_locale( void )
{
	GuiLocale *gl = calloc( 1, sizeof(*gl) );
	if ( NULL == gl ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}

	/* Initialize gettext i18n related fields of gl, then
	 * init and apply the gettetx internationalization.
	 */
	if ( ! _init_i18n(gl) ) {
		DBG_GUI_ERRMSG( NULL, "_init_gettext_library() failed!");
		free( gl );
		return NULL;
	}

	/* Save the current locale environment of the OS... */
	_save_os( gl );
	
	/* Apply the os locale (if invalid it falls-back to English).
	 */
	if ( !gui_locale_apply_osenv(gl) ) {
		DBG_GUI_ERRMSG( NULL, "gui_locale_apply_osenv() failed!");
		gui_locale_free( gl );
		return NULL;
	}

	return gl;
}
