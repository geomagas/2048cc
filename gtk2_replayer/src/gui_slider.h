#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include <gtk/gtk.h>

typedef struct _GuiSlider GuiSlider;

#ifndef GUI_SLIDER_C
extern GuiSlider   *make_gui_slider( void );
extern GuiSlider   *gui_slider_free( GuiSlider *slider );
extern gboolean    gui_slider_set_value( GuiSlider *slider, gdouble value );
extern gboolean    gui_slider_set_value_limits(
                         GuiSlider  *slider,
                         gdouble    minVal,
                         gdouble    maxVal
	                 );
extern gboolean    gui_slider_set_page_increment_10percent(
                         GuiSlider *slider,
                         gdouble    minVal,
                         gdouble    maxVal
                         );

extern gboolean    gui_slider_playback_init_from_builder(
                         GuiSlider  *slider,
                         GtkBuilder *builder,
                         void       *gui
                         );
extern gboolean    gui_slider_playback_refresh(
                         GuiSlider *slider,
                         void *gui
                         );
#endif

#endif
