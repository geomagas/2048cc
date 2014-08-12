#ifndef TEXT_H
#define TEXT_H

#include <glib/gi18n.h>

/* GUI strings that are dynamically added/updated,
 * regardless their initial values specified in
 * the Glade file: FNAME_GLADE.
 * Only those enclosed in _() are translatable.
 */
#define TXT_APP_NAME              _("GTK2 Replayer")
#define TXT_APP_VERSION           "0.1a"
#define TXT_APP_DESCRIPTION       _("for the 2048cc game (v0.3a3 )")
#define TXT_APP_COPYRIGHT         "copyright (c) 2014 migf1"
#define TXT_APP_WEBSITE_LABEL     "x-karagiannis.gr/prg/"
#define TXT_APP_WEBSITE           "http://x-karagiannis.gr/prg/"
#define TXT_APP_AUTHOR            "migf1 <mig_f1@hotmail.com>"
#define TXT_APP_ARTIST            TXT_APP_AUTHOR

#define TXT_YES                   _("Yes")
#define TXT_NO                    _("No")

#define TXTF_MARKUP_SCORE                                               \
_(                                                                      \
	"<span font=\"8\" foreground=\"#eee4da\">SCORE</span>\n"        \
	"<span foreground=\"white\" font=\"11\" font_weight=\"heavy\">" \
		"%ld"                                                   \
	"</span>"                                                       \
)

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
#define TXTF_STATUSBAR_ONROLL     _(" Spin: %03d | Frame: %03d | Delay: %06ld")
#define TXTF_STATUSBAR_RESULT     _(" Result: %d")

#define TXTF_STATUSBAR                                   \
_(                                                       \
	" %s | T: %ld | B: %dx%d | ST: %ld |"            \
	" R: %d | D: %lu | Won: %s"                      \
)

#define TXT_ERR_INVALID_REPLAYFILE                       \
_(                                                       \
	"The requested file was not loaded! \n"          \
	"\n"                                             \
	"Perhaps it was corrupted or not a\n"            \
	"valid replay-file at all."                      \
)

#endif
