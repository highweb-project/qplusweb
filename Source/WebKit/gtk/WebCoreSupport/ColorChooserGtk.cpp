#include "config.h"

#if ENABLE(INPUT_TYPE_COLOR)

#include "Color.h"

#include "ColorChooserGtk.h"

#include "ChromeClientGtk.h"
#include "ColorChooserClient.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define COLOR_8_TO_16(x) (x << 8) + x
#define COLOR_16_TO_8(x) (x >> 8)

namespace WebCore {

void colorChooserOkResponseCallback(GtkWidget* widget, GdkEventButton* gevent, gpointer data)
{	
	ColorChooserGtk* chooser = reinterpret_cast<ColorChooserGtk*>(data);
	if(chooser && chooser->client())
	{
		GdkColor gColor;
		GtkWidget* colorSelection;
		colorSelection = gtk_color_selection_dialog_get_color_selection(GTK_COLOR_SELECTION_DIALOG(chooser->dialog()));
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION (colorSelection),&gColor);
		Color color(gColor);

		chooser->client()->updateColorChooser(color);

		gtk_widget_hide(GTK_WIDGET(chooser->dialog()));		
	}
}

void colorChooserCancleResponseCallback(GtkWidget* widget, GdkEventButton* gevent, gpointer data)
{	
	ColorChooserGtk* chooser = reinterpret_cast<ColorChooserGtk*>(data);
	if(!chooser)
		return ;

	gtk_widget_hide(GTK_WIDGET(chooser->dialog()));
}

ColorChooserGtk::ColorChooserGtk(ChromeClient* chromeClient, const Color& color)
	: m_chromeClient(chromeClient)	
{	
	m_colorSelectionDialog = gtk_color_selection_dialog_new("color selection");
	gtk_window_set_position(GTK_WINDOW(m_colorSelectionDialog), GTK_WIN_POS_CENTER);
	GtkWidget* colorSelection;
	colorSelection = gtk_color_selection_dialog_get_color_selection(GTK_COLOR_SELECTION_DIALOG(m_colorSelectionDialog));

	GdkColor gColor;
	gColor.red = COLOR_8_TO_16(color.red());
	gColor.blue = COLOR_8_TO_16(color.green());
	gColor.green = COLOR_8_TO_16(color.blue());

	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(colorSelection),&gColor);
	
	GtkWidget* okResponseButton = gtk_dialog_get_widget_for_response(GTK_DIALOG(m_colorSelectionDialog), GTK_RESPONSE_OK);
	g_signal_connect(GTK_WIDGET(okResponseButton), "button_release_event", G_CALLBACK(colorChooserOkResponseCallback), (gpointer)this);

	GtkWidget* abbrechenResponseButton = gtk_dialog_get_widget_for_response(GTK_DIALOG(m_colorSelectionDialog), GTK_RESPONSE_CANCEL);
	g_signal_connect(GTK_WIDGET(abbrechenResponseButton), "button_release_event", G_CALLBACK(colorChooserCancleResponseCallback), (gpointer)this);
	//g_signal_connect(GTK_DIALOG(m_colorSelectionDialog), "destroy",  G_CALLBACK(gtk_widget_destroyed), &m_colorSelectionDialog);
	g_signal_connect(GTK_WIDGET(m_colorSelectionDialog), "delete_event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

	gtk_widget_show(GTK_WIDGET(m_colorSelectionDialog));
	
}

ColorChooserGtk::~ColorChooserGtk()
{
	

	m_colorSelectionDialog = NULL;
}

void ColorChooserGtk::setSelectedColor(const Color& color)
{	
	if(!m_colorSelectionDialog)
		return ;
	
	GtkWidget* colorSelection;
	colorSelection = gtk_color_selection_dialog_get_color_selection(GTK_COLOR_SELECTION_DIALOG(m_colorSelectionDialog));	

	GdkColor gColor;
	gColor.red = COLOR_8_TO_16(color.red());
	gColor.blue = COLOR_8_TO_16(color.green());
	gColor.green = COLOR_8_TO_16(color.blue());

	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(colorSelection),&gColor);
}

void ColorChooserGtk::endChooser()
{
	
	if(m_colorSelectionDialog){
		gtk_widget_destroy(GTK_WIDGET(m_colorSelectionDialog));
	}
	m_chromeClient->removeColorChooser();	
}

//void ColorChooserGtk::showColorChooser()
void ColorChooserGtk::reattachColorChooser(const Color& color)
{
	setSelectedColor(color);	
	if(m_colorSelectionDialog)
		gtk_widget_show(GTK_WIDGET(m_colorSelectionDialog));		
}

}

#endif // ENABLE(INPUT_TYPE_COLOR)
