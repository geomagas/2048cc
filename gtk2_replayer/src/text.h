/****************************************************************
 * This file is part of the "2048cc GTK+2 Replayer".
 *
 * Copyright:    2014 (c) migf1 <mig_f1@hotmail.com>
 * License:      Free Software (see comments in main.c for limitations)
 * Dependencies: glib/gi18n.h
 * --------------------------------------------------------------
 *
 * This header file contains all the translatable strings of the project.
 * Initially it was meant to host translatable strings of both gui and
 * non-gui related files, but it turned out that the latter do not output
 * any text at all. Most probably, the file-name will be changed to
 * something like gui_text.h or gui_strings.h in the future.
 *
 * The TXT_ prefix indicates plain text.
 * The TXTF_ prefix indicates text containing variable place-holders
 * ( a-la printf() ) to be filled-in with snprintf() or g_snprintf()
 * before outputting.
 ****************************************************************
 */

#ifndef TEXT_H
#define TEXT_H

#include <glib/gi18n.h>

/* GUI strings that are dynamically added/updated,
 * regardless their initial values specified in
 * the Glade file.
 * Only those enclosed in _() are translatable.
 */
#define TXT_APP_NAME              _("GTK2 Replayer")
#define TXT_APP_VERSION           "0.2a"
#define TXT_APP_DESCRIPTION       _("for the 2048cc game (v0.3a3 )")
#define TXT_APP_COPYRIGHT         "copyright (c) 2014 migf1"
#define TXT_APP_WEBSITE_LABEL     "x-karagiannis.gr/prg/"
#define TXT_APP_WEBSITE           "http://x-karagiannis.gr/prg/"
#define TXT_APP_AUTHOR            "migf1 <mig_f1@hotmail.com>"
#define TXT_APP_ARTIST            TXT_APP_AUTHOR

#define TXT_ALERT_BOX             _("Alert Box")
#define TXT_YES                   _("Yes")
#define TXT_NO                    _("No")

/* used in gtk_label_set_markup() for displaying the current score */
#define TXTF_MARKUP_SCORE                                               \
_(                                                                      \
	"<span font=\"8\" foreground=\"#eee4da\">SCORE</span>\n"        \
	"<span foreground=\"white\" font=\"11\" font_weight=\"heavy\">" \
		"%ld"                                                   \
	"</span>"                                                       \
)

/* used in gtk_label_set_markup() for displaying the current best-score */
#define TXTF_MARKUP_BSCORE                                              \
_(                                                                      \
	"<span font=\"8\" foreground=\"#eee4da\">BEST</span>\n"         \
	"<span foreground=\"white\" font=\"11\" font_weight=\"heavy\">" \
		"%ld"                                                   \
	"</span>"                                                       \
)

#define TXT_APP_LICENSE                                                  \
_(                                                                       \
	"Open-source, free software with the following limitations:\n"   \
	"\n"                                                             \
	"a.\tKeep it free and open-source.\n"                            \
	"\n"                                                             \
	"b.\tDo not attempt to make any kind of profit\n"                \
	"\tfrom it or from any derivatives of it.\n"                     \
	"\n"                                                             \
	"c.\tAlways re-distribute the original package,\n"               \
	"\talong with any software you distribute\n"                     \
	"\tbased on this one.\n"                                         \
)

#define TXT_MENUITEM_PLAYBACK_JUMPTO _("Jump to...")

#define TXTF_DLG_JUMPTO_RANGE     "%ld - %ld"
#define TXTF_DLG_JUMPTO_CURRENT   "%ld"
#define TXTF_DLG_JUMPTO_SUGGEST   "%ld"

/* all the info displayed in the status-bar */
#define TXTF_STATUSBAR                                   \
_(                                                       \
	" %s | T: %ld | B: %dx%d | ST: %ld |"            \
	" R: %d | D: %lu | Won: %s"                      \
)

#define TXT_ERR_INVALID_JUMPTO                           \
_(                                                       \
	"The requested move was invalid! \n"             \
	"\n"                                             \
	"Perhaps it was out of range,\n"                 \
	"or not a numeric value at all."                 \
)

#define TXT_ERR_INVALID_REPLAYFILE                       \
_(                                                       \
	"The requested file was not loaded! \n"          \
	"\n"                                             \
	"Perhaps it was corrupted or not a\n"            \
	"valid replay-file at all."                      \
)

#define TXT_ERR_NO_PROPER_LOCALE                         \
_(                                                       \
	"No proper locale environment found.  \n"        \
	"ENGLISH is used as fallback language."          \
)

#endif
