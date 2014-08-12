#ifndef GUI_LOCALE_ENV_H
#define GUI_LOCALE_ENV_H

#include <glib.h>

typedef struct _GuiLocale GuiLocale;

#ifndef GUI_LOCALE_C
extern void dbg_gui_locale_print_inherited(
		const GuiLocale *gl,
		const gchar *title
		);
extern void dbg_gui_locale_print_current(
		const GuiLocale *gl,
		const gchar *title
		);

extern GuiLocale *new_gui_locale( void );
extern GuiLocale *gui_locale_free( GuiLocale *gl );
extern gboolean  gui_locale_apply_en( const GuiLocale *gl );
extern gboolean  gui_locale_apply_el( const GuiLocale *gl );
extern gboolean  gui_locale_apply_osenv( GuiLocale *gl );
#endif

#endif
