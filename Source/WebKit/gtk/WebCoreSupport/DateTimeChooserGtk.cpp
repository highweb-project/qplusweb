#include "config.h"

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

#include "DateTimeChooserGtk.h"

namespace WebCore {

void DateTimeChooserDaySelectedCallback(GtkWidget* widget, gpointer data)
{
    if(!widget)
        return;
    
    guint year = 0;
    guint month = 0;
    guint day = 0;

    gtk_calendar_clear_marks(GTK_CALENDAR(widget));
    gtk_calendar_get_date(GTK_CALENDAR(widget), &year, &month, &day);
    gtk_calendar_mark_day(GTK_CALENDAR(widget), day);    
}

void DateTimeChooserCancleButtonPressedCallback(GtkWidget* widget, gpointer data)
{
    DateTimeChooserGtk* chooser = reinterpret_cast<DateTimeChooserGtk*>(data);

    if(!chooser || !chooser->window() || !chooser->combobox() || !chooser->spinbuttonHour() || !chooser->spinbuttonMinute())
        return;
    
    gtk_widget_hide(GTK_WIDGET(chooser->window()));

    if(chooser->getPastValues().sPastAmPm == AM)
        gtk_combo_box_set_active (GTK_COMBO_BOX(chooser->combobox()), 0);
    else
        gtk_combo_box_set_active (GTK_COMBO_BOX(chooser->combobox()), 1);
    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(chooser->spinbuttonHour()), chooser->getPastValues().iPastHour);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(chooser->spinbuttonMinute()), chooser->getPastValues().iPastMin);

    gtk_calendar_select_month (GTK_CALENDAR(chooser->calendar()),chooser->getPastValues().uiPastMonth, chooser->getPastValues().uiPastYear);
    gtk_calendar_select_day (GTK_CALENDAR(chooser->calendar()), chooser->getPastValues().uiPastDay );    
}

void DateTimeChooserOkButtonPressedCallback(GtkWidget* widget, gpointer data)
{
    DateTimeChooserGtk* chooser = reinterpret_cast<DateTimeChooserGtk*>(data);

    if(!chooser || !chooser->client() || !chooser->calendar() || !chooser->combobox() || !chooser->spinbuttonHour() || !chooser->spinbuttonMinute() || !chooser->window())
        return;
    
    String type(chooser->inputType());
    guint year = 0;
    guint month = 0;
    guint day = 0;

    gtk_calendar_get_date(GTK_CALENDAR(chooser->calendar()), &year, &month, &day);

    String sYear(String::number(year));
    String sMonth(String::number(CURRENT_MONTH(month))); 
    String sDay(String::number(day));
    String AmPm(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(chooser->combobox())));

    gint iHour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(chooser->spinbuttonHour()));

    if(AmPm == PM && iHour != NOON) //afternoon ex) 12:32, 14:32, ...
        iHour += 12;
    if(AmPm == AM && iHour == MIDNIGHT) //morning ex) 00:13, 1:13, ...
        iHour = 0;    

    String sHour(String::number(iHour));
    String sMinute(gtk_entry_get_text(GTK_ENTRY(chooser->spinbuttonMinute())));

    if(sMonth.length() < 2)
        sMonth.insert("0", 0);
    if(sDay.length() < 2)
        sDay.insert("0", 0);
    if(sHour.length() < 2)
        sHour.insert("0", 0);
    if(sMinute.length() < 2)
        sMinute.insert("0", 0);
   
    chooser->client()->updateDateTimeChooser(String::format("%s-%s-%sT%s:%sZ", sYear.utf8().data(), sMonth.utf8().data(), sDay.utf8().data(), sHour.utf8().data(), sMinute.utf8().data()));
    gtk_widget_hide(GTK_WIDGET(chooser->window()));    
}

void DateTimeLocalChooserOkButtonPressedCallback(GtkWidget* widget, gpointer data)
{
    DateTimeChooserGtk* chooser = reinterpret_cast<DateTimeChooserGtk*>(data);

    if(!chooser || !chooser->client() || !chooser->calendar() || !chooser->combobox() || !chooser->spinbuttonHour() || !chooser->spinbuttonMinute() || !chooser->window())
        return;

    String type(chooser->inputType());
    guint year = 0;
    guint month = 0;
    guint day = 0;

    gtk_calendar_get_date(GTK_CALENDAR(chooser->calendar()), &year, &month, &day);

    String sYear(String::number(year));
    String sMonth(String::number(CURRENT_MONTH(month)));
    String sDay(String::number(day));
    String AmPm(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(chooser->combobox())));

    gint iHour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(chooser->spinbuttonHour()));

    if(AmPm == PM && iHour != NOON) //afternoon ex) 12:32, 14:32, ...
        iHour += 12;
    if(AmPm == AM && iHour == MIDNIGHT) //morning ex) 00:13, 1:13, ...
        iHour = 0;    
    
    String sHour(String::number(iHour));
    String sMinute(gtk_entry_get_text(GTK_ENTRY(chooser->spinbuttonMinute())));

    if(sMonth.length() < 2)
        sMonth.insert("0", 0);
    if(sDay.length() < 2)
        sDay.insert("0", 0);
    if(sHour.length() < 2)
        sHour.insert("0", 0);
    if(sMinute.length() < 2)
        sMinute.insert("0", 0);
        
    chooser->client()->updateDateTimeChooser(String::format("%s-%s-%sT%s:%s", sYear.utf8().data(), sMonth.utf8().data(), sDay.utf8().data(), sHour.utf8().data(), sMinute.utf8().data()));
    gtk_widget_hide(GTK_WIDGET(chooser->window()));
}

DateTimeChooserGtk::DateTimeChooserGtk(ChromeClient* chromeClient, String type) : BaseDateTimeChooserGtk(chromeClient, type)
    , m_calendarDateChooser(NULL)
    , m_comboboxAmPm(NULL)
    , m_spinbuttonHour(NULL)
    , m_spinbuttonMinute(NULL)
    , m_buttonOk(NULL)
    , m_buttonCancle(NULL)
    , m_labelCurrentDate(NULL)
    , m_labelCurrentTime(NULL) 
{
    initialize();
}

DateTimeChooserGtk::~DateTimeChooserGtk()
{
    m_calendarDateChooser = NULL;
    m_comboboxAmPm = NULL;
    m_spinbuttonHour = NULL;
    m_spinbuttonMinute = NULL;
    m_buttonOk = NULL;
    m_buttonCancle = NULL;
    m_labelCurrentDate = NULL;
    m_labelCurrentTime = NULL;
}	

bool DateTimeChooserGtk::createDateTimeChooserWidget()
{
    struct timeval timeVal;
    gettimeofday(&timeVal, 0);
    struct tm* currentTime = localtime( &timeVal.tv_sec);
    
    GtkWidget* label = NULL; 
    
    GtkWidget* containerBox = NULL; //main container box
    GtkWidget* containerSubBox = NULL; //sub container box for date chooser and time chooser
    GtkWidget* containerButtonBox = NULL;
    
    GtkAdjustment* adjHour = NULL;
    GtkAdjustment* adjMinute = NULL;
    
    m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   
    gtk_window_set_title(GTK_WINDOW(m_window), "DateTimeChooser");
    gtk_window_set_default_size(GTK_WINDOW(m_window), WIDGET_DEFAULT_HEIGHT, WIDGET_DEFAULT_WIDTH);
    gtk_window_set_resizable (GTK_WINDOW(m_window), false);
    gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);
  
    if(!m_window) 
		return false;

    containerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    if(!containerBox) 
		return false;
    
    gtk_container_add(GTK_CONTAINER(m_window), containerBox);

    //date     
    m_calendarDateChooser = gtk_calendar_new();
    if(!m_calendarDateChooser) 
		return false;


    String stringBuffer = String::format("%s %d - %d - %d", "Current Date : ", CURRENT_YEAR(currentTime->tm_year), CURRENT_MONTH(currentTime->tm_mon), currentTime->tm_mday);
    m_labelCurrentDate = gtk_label_new(stringBuffer.utf8().data());
    if(!m_labelCurrentDate) 
		return false;
    
    containerSubBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    if(!containerSubBox) 
		return false;

    gtk_box_pack_start(GTK_BOX(containerBox), m_labelCurrentDate, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(containerBox), containerSubBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(containerSubBox), m_calendarDateChooser, TRUE, TRUE, 0);
    
    gtk_calendar_mark_day(GTK_CALENDAR(m_calendarDateChooser), currentTime->tm_mday);    

    adjHour = gtk_adjustment_new(1, 1, 13, 1, 1, 1); //set spinbuttonHour value
    adjMinute = gtk_adjustment_new (1, 0, 60, 1, 1, 1 ); //set spinbuttonMinute value
    if(!adjHour || !adjMinute) 
		return false;

    m_comboboxAmPm = gtk_combo_box_text_new();
    m_spinbuttonHour = gtk_spin_button_new(adjHour, 1, 0);
    m_spinbuttonMinute = gtk_spin_button_new(adjMinute,1, 0);
    if(!m_comboboxAmPm || !m_spinbuttonHour || !m_spinbuttonMinute) 
		return false;

    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(m_spinbuttonHour), TRUE);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(m_spinbuttonMinute), TRUE);
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(m_comboboxAmPm), AM);
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(m_comboboxAmPm), PM);

    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonMinute), currentTime->tm_min);
        
    if(currentTime->tm_hour > NOON) //afternoon
    {
        stringBuffer = String::format("%s %s %d : %d", "Current Time : ", PM, currentTime->tm_hour-12, currentTime->tm_min);
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonHour), currentTime->tm_hour-12);
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxAmPm), 1);
        setPastValues(CURRENT_YEAR(currentTime->tm_year),  CURRENT_MONTH(currentTime->tm_mon), currentTime->tm_mday, currentTime->tm_hour-12 , currentTime->tm_min , PM);
    }  
    else //morning
    {
        stringBuffer = String::format("%s %s %d : %d", "Current Time : ", AM, currentTime->tm_hour, currentTime->tm_min);   
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonHour), currentTime->tm_hour);
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxAmPm), 0);
        setPastValues(CURRENT_YEAR(currentTime->tm_year),  CURRENT_MONTH(currentTime->tm_mon), currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, AM);
    }

    m_labelCurrentTime = gtk_label_new(stringBuffer.utf8().data());
    if(!m_labelCurrentTime) 
		return false;

    gtk_box_pack_start(GTK_BOX(containerBox), m_labelCurrentTime, FALSE, FALSE, 0);
    
    containerSubBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
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
    if(m_inputType == TYPE_DATETIME)
        g_signal_connect(GTK_WIDGET(m_buttonOk), "clicked", G_CALLBACK(DateTimeChooserOkButtonPressedCallback), (gpointer)this);

    if(m_inputType == TYPE_DATETIME_LOCAL)
        g_signal_connect(GTK_WIDGET(m_buttonOk), "clicked", G_CALLBACK(DateTimeLocalChooserOkButtonPressedCallback), (gpointer)this);
    
    g_signal_connect(GTK_WIDGET(m_buttonCancle), "clicked", G_CALLBACK(DateTimeChooserCancleButtonPressedCallback), (gpointer)this);
    g_signal_connect(GTK_WIDGET(m_calendarDateChooser), "day_selected", G_CALLBACK(DateTimeChooserDaySelectedCallback), NULL);

    //calender's listener
    g_signal_connect(GTK_WIDGET(m_window), "delete_event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
    gtk_widget_show_all(m_window);

    return TRUE;
}

void DateTimeChooserGtk::reattachDateTimeChooser()
{
    guint year = 0;
    guint month = 0;
    guint day = 0;
    gint hour = 0;
    gint min = 0;
    gchar* AmPm = NULL;
    
    if(!m_window && !m_spinbuttonHour && !m_spinbuttonMinute && !m_labelCurrentTime && !m_labelCurrentDate && !m_calendarDateChooser)
        return;
    
    AmPm = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox()));
    hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonHour));
    min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonMinute));
    
    String stringBuffer = String::format("%s %s %d : %d", "Current Time : ", AmPm, hour, min);
    gtk_label_set_text (GTK_LABEL(m_labelCurrentTime), stringBuffer.utf8().data());   

    gtk_calendar_get_date(GTK_CALENDAR(m_calendarDateChooser), &year, &month, &day);
    stringBuffer = String::format("%s %d - %d - %d", "Current Date : ", year, CURRENT_MONTH(month), day); 
    gtk_label_set_text(GTK_LABEL(m_labelCurrentDate), stringBuffer.utf8().data());   

    setPastValues(year, month, day, hour, min, AmPm);

    gtk_window_present(GTK_WINDOW(m_window));
}

}//end namespace webcore

#endif
