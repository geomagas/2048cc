#define GUI_SLIDER_C

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

#include "misc.h"
#include "gui_slider.h"
#include "gui.h"

/* a bit more convenient GUI abstraction of an horizontal slider */
struct _GuiSlider {
	GtkWidget       *widget;
	GtkAdjustment   *adjustment;
	GtkPositionType valuePos;
	gboolean        drawValue;
	gdouble         minVal, maxVal, value;
	gdouble         stepInc, pageInc;
	gint            digits;
	/* non-critical fields, let them be as defined in Glade
	gdouble         pageSize;
	*/
};

/* ---------------------------------------------------
 * Callback function connected to the GTK+ "value-changed"
 * signal, for the playback-slider.
 * ---------------------------------------------------
 */
static void _on_value_changed_slider_playback( GtkRange *range, Gui *gui )
{
	gdouble val;

	/* sanity checks */
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "Invalid pointer arg (gui)" );
		return;
	}

	val = gtk_range_get_value(range);
	gui_set_slider_playback_value( gui, val );
	gui_set_imove( gui, val - 1 );
	gui_refresh( gui );
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
static gboolean _init_from_builder(
	GuiSlider   *slider,
	GtkBuilder  *builder,
	const gchar *sliderName,
	const gchar *adjustmentName,
	Gui         *gui
	)
{
	/* sanity checks */
//	if ( NULL == hslider ) {
//		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (slider)" );
//		return FALSE;
//	}
//	if ( NULL == slider->widget || NULL == slider->adjustment ) {
//		DBG_GUI_ERRMSG( NULL, _"hslider has not been properly created" );
//		return FALSE;
//	}

	slider->widget =
	GTK_WIDGET( gtk_builder_get_object(builder, sliderName) );

	if ( NULL == slider->widget ) {
		DBG_STDERR_MSG( "Failure to load slider->widget from builder");
		return FALSE;
	}

	slider->adjustment =
	GTK_ADJUSTMENT( gtk_builder_get_object(builder, adjustmentName) );

	if ( NULL == slider->widget ) {
		DBG_STDERR_MSG(
			"Failure to load slider->adjustment from builder" );
		return FALSE;
	}

	slider->valuePos  = gtk_scale_get_value_pos(GTK_SCALE(slider->widget));
	slider->drawValue = gtk_scale_get_draw_value(GTK_SCALE(slider->widget));
	slider->minVal    = gtk_adjustment_get_lower(
				GTK_ADJUSTMENT( slider->adjustment)
				);
	slider->maxVal    = gtk_adjustment_get_upper(
				GTK_ADJUSTMENT( slider->adjustment )
				);
	slider->value   = gtk_adjustment_get_value( slider->adjustment );
	slider->stepInc = gtk_adjustment_get_step_increment(
				GTK_ADJUSTMENT( slider->adjustment )
				);
	slider->pageInc = gtk_adjustment_get_page_increment(
				GTK_ADJUSTMENT( slider->adjustment )
				);
	slider->digits = gtk_scale_get_digits( GTK_SCALE(slider->widget) );

gtk_range_set_show_fill_level(
	GTK_RANGE( slider->widget ),
	TRUE
	);
gtk_range_set_fill_level(
	GTK_RANGE( slider->widget ),
	0.0
	);
	g_signal_connect(
		G_OBJECT( slider->widget ),
		"value-changed",
		G_CALLBACK( _on_value_changed_slider_playback ),
		gui
	);

	return TRUE;
}

/* ---------------------------------------------------
 * Change the value field of the specified slider
 * ---------------------------------------------------
 */
gboolean gui_slider_set_value(
	GuiSlider  *slider,
	gdouble    value
	)
{
	/* sanity checks */
	if ( NULL == slider ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (slider)" );
		return FALSE;
	}
	if ( NULL == slider->widget || NULL == slider->adjustment ) {
		DBG_GUI_ERRMSG( NULL, "slider has not been properly created" );
		return FALSE;
	}

	gtk_adjustment_set_value( slider->adjustment, value );
	slider->value = value;

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_slider_set_value_limits(
	GuiSlider  *slider,
	gdouble    minVal,
	gdouble    maxVal
	)
{
	/* sanity checks */
	if ( NULL == slider ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (slider)" );
		return FALSE;
	}
	if ( NULL == slider->widget || NULL == slider->adjustment ) {
		DBG_GUI_ERRMSG( NULL, "slider has not been properly created" );
		return FALSE;
	}

	if ( minVal > maxVal ) {
		gdouble temp = minVal;
		minVal = temp;
		maxVal = minVal;
	}

//	gtk_range_set_range(
//		GTK_RANGE( slider->widget ),
//		slider->minVal,
//		slider->maxVal
//		);
	gtk_adjustment_set_lower(
		GTK_ADJUSTMENT( slider->adjustment ),
		minVal
		);
	gtk_adjustment_set_upper(
		GTK_ADJUSTMENT( slider->adjustment ),
		maxVal
		);

	slider->minVal = minVal;
	slider->maxVal = maxVal;

	return TRUE;
}

/* ---------------------------------------------------
 * 
 * ---------------------------------------------------
 */
gboolean gui_slider_set_page_increment_10percent(
	GuiSlider *slider,
	gdouble    minval,
	gdouble    maxval
	)
{
	gdouble pageInc, diff;

	/* sanity checks */
	if ( NULL == slider ) {
		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (slider)" );
		return FALSE;
	}
	if ( NULL == slider->widget || NULL == slider->adjustment ) {
		DBG_GUI_ERRMSG( NULL, "slider has not been properly created" );
		return FALSE;
	}

	diff = ABS( maxval - minval );

	pageInc = diff < 10.1
			? (long int)diff
			: (long int)diff / 10
			;

	gtk_adjustment_set_page_increment(
		GTK_ADJUSTMENT( slider->adjustment ),
		pageInc
		);
	slider->pageInc = pageInc;

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
static gboolean _slider_set(
	GuiSlider  *slider,
	gdouble    value,     /* current value */
	gdouble    minVal,    /* lower bound */
	gdouble    maxVal,    /* upper bound */
	gdouble    stepInc,   /* step increment */
	gdouble    pageInc,   /* page increment */
	gint       digits     /* # of decimal digits (for increments) */
	)
{
	/* sanity checks */
//	if ( NULL == slider ) {
//		DBG_GUI_ERRMSG( NULL, "Invalid pointer arg (slider)" );
//		return FALSE;
//	}
//	if ( NULL == slider->widget || NULL == slider->adjustment ) {
//		DBG_GUI_ERRMSG( NULL, _"slider has not been properly created");
//		return FALSE;
//	}

	slider->value   = value;
	slider->minVal  = minVal;
	slider->maxVal  = maxVal;
	slider->stepInc = stepInc;
	slider->pageInc = pageInc;
	slider->digits	 = digits;

	gtk_scale_set_digits( GTK_SCALE(slider->widget), digits );

//	gtk_range_set_range(
//		GTK_RANGE( slider->widget ),
//		slider->minVal,
//		slider->maxVal
//		);
	gtk_adjustment_set_lower(
		GTK_ADJUSTMENT( slider->adjustment ),
		minVal
		);
	gtk_adjustment_set_upper(
		GTK_ADJUSTMENT( slider->adjustment ),
		maxVal
		);

//	gtk_range_set_increments(
//		GTK_RANGE( slider->widget ),
//		slider->stepInc,
//		slider->pageInc
//		);
	gtk_adjustment_set_step_increment(
		GTK_ADJUSTMENT( slider->adjustment ),
		stepInc
		);
	gtk_adjustment_set_page_increment(
		GTK_ADJUSTMENT( slider->adjustment ),
		pageInc
		);

	gtk_adjustment_set_value( slider->adjustment, value );

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GuiSlider *gui_slider_free( GuiSlider *slider )
{
	if ( slider ) {
		free( slider );
	}
	return NULL;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
GuiSlider *make_gui_slider( void )
{
	GuiSlider *slider = calloc( 1, sizeof(*slider) );
	if ( NULL == slider ) {
		DBG_GUI_ERRMSG( NULL, "calloc() failed!");
		return NULL;
	}
	return slider;
}

/* ---------------------------------------------------
 * Initialize the playback slider.
 *
 * Copies the GUI playback-slider from the specified GTK+ builder
 * into my GUI slider abstraction, it connects callback functions
 * to it, and initializes its visual appearance.
 *
 * NOTE:
 *    The GTK+ callback function: gtk_widget_destroyed()
 *    ensures that the widget pointer will be set to NULL
 *    after the widget gets destroyed.
 * ---------------------------------------------------
 */
gboolean gui_slider_playback_init_from_builder(
	GuiSlider  *slider,
	GtkBuilder *builder,
	Gui        *gui
	)
{
	/* sanity checks */
	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gui)" );
		return FALSE;
	}
	if ( NULL == slider || NULL == builder ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (slider OR builder)"
			);
		return FALSE;
	}

	if ( !_init_from_builder(
			slider,
			builder,
			"hsPlaybackSlider",
			"adjPlaybackSlider",
			gui
			)
	){
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"_init_from_builder() failed!"
			);
		return FALSE;
	}

	return TRUE;
}

/* ---------------------------------------------------
 *
 * ---------------------------------------------------
 */
gboolean gui_slider_playback_refresh( GuiSlider *slider, Gui *gui )
{
	gdouble val;

	if ( NULL == gui ) {
		DBG_STDERR_MSG( "NULL pointer argument (gm)" );
		return FALSE;
	}
	if ( NULL == slider ) {
		DBG_GUI_ERRMSG(
			gui_get_appWindow( gui ),
			"NULL pointer argument (gm)"
		);
		return FALSE;
	}

	/* when no replay-file is loaded */
	if ( !gui_get_hasloadedfile(gui) ) {
		gtk_widget_set_sensitive( slider->widget, FALSE );
		return TRUE;
	}

	/* when replay-file is loaded */
	if ( gui_get_isPlayPressed(gui) ) {
		gtk_widget_set_sensitive( slider->widget, FALSE );
	}
	else {
		gtk_widget_set_sensitive( slider->widget, TRUE );
	}

	val = (gdouble)gui_get_imove(gui) + 1;
	gui_set_slider_playback_value( gui, val );
	gtk_range_set_fill_level( GTK_RANGE(slider->widget), val );

	return TRUE;
}
