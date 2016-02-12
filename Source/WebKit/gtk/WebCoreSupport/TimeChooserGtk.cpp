#include "config.h"

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

#include "ChromeClientGtk.h"
#include "BaseDateTimeChooserGtk.h"
#include "DateTimeChooserClient.h"
#include "TimeChooserGtk.h"

namespace WebCore {

void TimeChooserCancleButtonPressedCallback(GtkWidget* widget, gpointer data)
{
    TimeChooserGtk* chooser = reinterpret_cast<TimeChooserGtk*>(data);
    
    if(!chooser || !chooser->combobox() || !chooser->spinbuttonHour() || !chooser->spinbuttonMinute())
        return;
    
    gtk_widget_hide(GTK_WIDGET(chooser->window()));

    if(chooser->getPastValues().sPastAmPm == AM)
        gtk_combo_box_set_active (GTK_COMBO_BOX(chooser->combobox()), 0);
    else
        gtk_combo_box_set_active (GTK_COMBO_BOX(chooser->combobox()), 1);
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(chooser->spinbuttonHour()), chooser->getPastValues().iPastHour);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(chooser->spinbuttonMinute()), chooser->getPastValues().iPastMin);
}

void TimeChooserOkButtonPressedCallback(GtkWidget* widget, gpointer data)
{
    TimeChooserGtk* chooser = reinterpret_cast<TimeChooserGtk*>(data);

    if( !chooser || !chooser->client() || !chooser->combobox() || !chooser->spinbuttonHour() || !chooser->spinbuttonMinute())
    	return;

    String AmPm(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(chooser->combobox())));
    gint iHour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(chooser->spinbuttonHour()));

    if(AmPm == PM && iHour != NOON) //afternoon ex) 12:32, 14:32, ...
        iHour += 12;
    if(AmPm == AM && iHour == MIDNIGHT) //morning ex) 00:13, 1:13, ...
        iHour = 0;    
    
    String sHour(String::number(iHour));
    String sMinute(gtk_entry_get_text(GTK_ENTRY(chooser->spinbuttonMinute())));
    
    if(sHour.length() < 2)
        sHour.insert("0", 0);
    if(sMinute.length() < 2)
        sMinute.insert("0", 0);
    
    chooser->client()->updateDateTimeChooser(String::format("%s:%s", sHour.utf8().data(), sMinute.utf8().data()));
    gtk_widget_hide(GTK_WIDGET(chooser->window()));
    
}

TimeChooserGtk::TimeChooserGtk(ChromeClient* chromeClient, String type) : BaseDateTimeChooserGtk(chromeClient, type)
    , m_comboboxAmPm(NULL)
    , m_spinbuttonHour(NULL)
    , m_spinbuttonMinute(NULL)
    , m_buttonOk(NULL)
    , m_buttonCancle(NULL)
    , m_labelCurrentTime(NULL)
{
    initialize();
}

TimeChooserGtk::~TimeChooserGtk()
{   
    m_comboboxAmPm = NULL;
    m_spinbuttonHour = NULL;
    m_spinbuttonMinute = NULL;
    m_buttonOk = NULL;
    m_buttonCancle = NULL;
    m_labelCurrentTime = NULL;
}   

bool TimeChooserGtk::createDateTimeChooserWidget()
{
    struct timeval timeVal;
    gettimeofday(&timeVal, 0);
    struct tm* currentTime = localtime( &timeVal.tv_sec);
    String stringBuffer;

    GtkWidget* label = NULL; 
    
    GtkWidget* containerBox = NULL; //main container box
    GtkWidget* containerSubBox = NULL; //sub container box for date chooser and time chooser
    GtkWidget* containerButtonBox = NULL;
    
    GtkAdjustment* adjHour = NULL;
    GtkAdjustment* adjMinute = NULL;
    
    m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if(!m_window) 
		return false;
   
    gtk_window_set_title(GTK_WINDOW(m_window), "TimeChooser");
    gtk_window_set_default_size(GTK_WINDOW(m_window), WIDGET_DEFAULT_HEIGHT, WIDGET_DEFAULT_WIDTH);
    gtk_window_set_resizable (GTK_WINDOW(m_window), false);
    gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);
  

    containerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(m_window), containerBox);
    if(!containerBox) 
		return false;

    adjHour = gtk_adjustment_new(1, 1, 13, 1, 1, 1);  //set spinbuttonHour value range
    adjMinute = gtk_adjustment_new (1, 0, 60, 1, 1, 1 ); //set spinbuttonMinute value range
    m_comboboxAmPm = gtk_combo_box_text_new();
    if(!adjHour || !adjMinute || !m_comboboxAmPm) 
		return false;

    m_spinbuttonHour = gtk_spin_button_new(adjHour, 1, 0);
    m_spinbuttonMinute = gtk_spin_button_new(adjMinute,1, 0);
    if(!m_spinbuttonHour || !m_spinbuttonMinute) 
		return false;
    
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(m_spinbuttonHour), TRUE);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(m_spinbuttonMinute), TRUE);
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(m_comboboxAmPm), AM);
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(m_comboboxAmPm), PM);

    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonMinute), currentTime->tm_min);
    
    if(currentTime->tm_hour > NOON) // afternoon
    {
        stringBuffer = String::format("%s %s %d : %d", "Current Time : ", PM, currentTime->tm_hour-12, currentTime->tm_min);
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonHour), currentTime->tm_hour-12);
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxAmPm), 1);
        setPastValues(0, 0, 0, currentTime->tm_hour-12, currentTime->tm_min, PM);
  
    }  
    else //morning
    {
        stringBuffer = String::format("%s %s %d : %d", "Current Time : ", AM, currentTime->tm_hour, currentTime->tm_min);
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonHour), currentTime->tm_hour);
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxAmPm), 0);
        setPastValues(0, 0, 0, currentTime->tm_hour, currentTime->tm_min, AM);
    }
    
    m_labelCurrentTime = gtk_label_new(stringBuffer.utf8().data());
    if(!m_labelCurrentTime) 
		return false;
    
    gtk_box_pack_start(GTK_BOX(containerBox), m_labelCurrentTime, FALSE, FALSE, 0);
    
    containerSubBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    if(!containerSubBox) 
		return false;
    
    gtk_box_set_spacing(GTK_BOX(containerSubBox), 1); 
    label = gtk_label_new(":");
    if(!label) 
		return false;
    
    gtk_box_pack_start(GTK_BOX(containerBox), containerSubBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(containerSubBox), m_comboboxAmPm, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(containerSubBox), m_spinbuttonHour, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(containerSubBox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(containerSubBox), m_spinbuttonMinute, TRUE, TRUE, 0);
    
    //buttons
    containerButtonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    if(!containerButtonBox) 
		return false;    
    
    gtk_box_pack_start(GTK_BOX(containerBox), containerButtonBox, TRUE, TRUE, 0);

    m_buttonOk = gtk_button_new_with_label("Ok");
    m_buttonCancle = gtk_button_new_with_label("Cancle");

    if(!m_buttonOk || !m_buttonCancle) 
		return false;
    
    gtk_box_pack_start(GTK_BOX(containerButtonBox), m_buttonOk, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(containerButtonBox), m_buttonCancle, TRUE, TRUE, 0);
    gtk_box_set_homogeneous(GTK_BOX(containerButtonBox), TRUE);

    //button's listener
    g_signal_connect(GTK_WIDGET(m_buttonOk), "clicked", G_CALLBACK(TimeChooserOkButtonPressedCallback), (gpointer)this);
    g_signal_connect(GTK_WIDGET(m_buttonCancle), "clicked", G_CALLBACK(TimeChooserCancleButtonPressedCallback), (gpointer)this);

    //calender's listener
    g_signal_connect(GTK_WIDGET(m_window), "delete_event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);

    gtk_widget_show_all(m_window);

    return TRUE;
}
	
void TimeChooserGtk::reattachDateTimeChooser()
{
    gint hour = 0; 
    gint min = 0;
    gchar* AmPm = NULL;
    if(!m_window || !m_spinbuttonHour || !m_spinbuttonMinute || !m_labelCurrentTime)
        return;

    AmPm = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(m_comboboxAmPm));
    hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonHour));
    min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonMinute));
    
    String stringBuffer = String::format("%s %s %d : %d", "Current Time : ", AmPm, hour, min);
    gtk_label_set_text (GTK_LABEL(m_labelCurrentTime), stringBuffer.utf8().data());   

    setPastValues(0, 0, 0, hour, min, AmPm);

    gtk_window_present(GTK_WINDOW(m_window));
}

    
}//namespace webcore

#endif
