#include "config.h"

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

#include "ChromeClientGtk.h"
#include "BaseDateTimeChooserGtk.h"
#include "DateTimeChooserClient.h"
#include "DateChooserGtk.h"

namespace WebCore {

void dateChooserDayClickedCallback(GtkWidget* widget, gpointer data)
{
    DateChooserGtk* chooser = reinterpret_cast<DateChooserGtk*>(data);

    if(!chooser || !chooser->client() || !widget)
        return;

    guint year = 0;
    guint month = 0;
    guint day = 0;
    gtk_calendar_clear_marks(GTK_CALENDAR(widget));
    gtk_calendar_get_date(GTK_CALENDAR(widget), &year, &month, &day);
    gtk_calendar_mark_day(GTK_CALENDAR(widget), day);

    String sYear(String::number(year));
    String sMonth(String::number(CURRENT_MONTH(month))); 
    String sDay(String::number(day));   

    if(sMonth.length() < 2)
        sMonth.insert("0", 0);
    if(sDay.length() < 2)
        sDay.insert("0", 0);
   
    chooser->client()->updateDateTimeChooser(String::format("%s-%s-%s", sYear.utf8().data(), sMonth.utf8().data(), sDay.utf8().data()));
    
}

void weekChooserDayClickedCallback(GtkWidget* widget, gpointer data)
{
    DateChooserGtk* chooser = reinterpret_cast<DateChooserGtk*>(data);

    if(!chooser || !chooser->client() || !widget)
        return;
    
    guint year = 0;
    guint month = 0;
    guint day = 0;
    gtk_calendar_clear_marks(GTK_CALENDAR(widget));
    gtk_calendar_get_date(GTK_CALENDAR(widget), &year, &month, &day);
    gtk_calendar_mark_day(GTK_CALENDAR(widget), day);

    String sYear(String::number(year));

    GDate date;
    g_date_set_dmy(&date, (GDateDay)day, (GDateMonth)(CURRENT_MONTH(month)), (GDateYear)year); //month : [0-11]
    int weekOfYear = g_date_get_iso8601_week_of_year(&date);
    
    String sWeekOfYear(String::number(weekOfYear));
    
    if(sWeekOfYear.length() < 2)
        sWeekOfYear.insert("0", 0);
    
    chooser->client()->updateDateTimeChooser(String::format("%s-W%s", sYear.utf8().data(), sWeekOfYear.utf8().data()));
}

void monthChooserDayClickedCallback(GtkWidget* widget, gpointer data)
{
    DateChooserGtk* chooser = reinterpret_cast<DateChooserGtk*>(data);

    if(!chooser || !chooser->client() || !widget)
        return;
    
    guint year = 0;
    guint month = 0;
    guint day = 0;

    gtk_calendar_clear_marks(GTK_CALENDAR(widget));
    gtk_calendar_get_date(GTK_CALENDAR(widget), &year, &month, &day);
    gtk_calendar_mark_day(GTK_CALENDAR(widget), day);

    String sYear(String::number(year));
    String sMonth(String::number(CURRENT_MONTH(month)));

    if(sMonth.length() < 2)
        sMonth.insert("0", 0);
    
    chooser->client()->updateDateTimeChooser(String::format("%s-%s", sYear.utf8().data(), sMonth.utf8().data()));
    
}

void dateChooserDayDoubleClickedCallback(GtkWidget* widget, gpointer data)
{
    DateChooserGtk* chooser = reinterpret_cast<DateChooserGtk*>(data);
    
    if(!chooser || !chooser->window())
        return;

    gtk_widget_hide(GTK_WIDGET(chooser->window()));
    
}

DateChooserGtk::DateChooserGtk(ChromeClient* chromeClient, String type) : BaseDateTimeChooserGtk(chromeClient, type)
    ,m_calendarDateChooser(NULL)
    ,m_labelCurrentDate(NULL)
{
    initialize();
}

DateChooserGtk::~DateChooserGtk()
{
    m_calendarDateChooser = NULL;
    m_labelCurrentDate = NULL;
}   

bool DateChooserGtk::createDateTimeChooserWidget()
{
    struct timeval timeVal;
    gettimeofday(&timeVal, 0);
    struct tm* currentTime = localtime( &timeVal.tv_sec);

    m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);    
    if(!m_window) 
		return FALSE;
   
    gtk_window_set_title(GTK_WINDOW(m_window), "DateChooser");
    gtk_window_set_default_size(GTK_WINDOW(m_window), WIDGET_DEFAULT_HEIGHT, WIDGET_DEFAULT_WIDTH);
    gtk_window_set_resizable (GTK_WINDOW(m_window), FALSE);
    gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);
 
    GtkWidget* containerBox = NULL; //main container box
    GtkWidget* containerSubBox = NULL; //sub container box for date chooser and time chooser 

    containerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    if(!containerBox) 
		return FALSE;

    gtk_container_add(GTK_CONTAINER(m_window), containerBox);

    m_calendarDateChooser = gtk_calendar_new();
    if(!m_calendarDateChooser) 
		return FALSE;

    String stringBuffer = String::format("%s %d - %d - %d", "Current Date : ", CURRENT_YEAR(currentTime->tm_year), CURRENT_MONTH(currentTime->tm_mon), currentTime->tm_mday);
    m_labelCurrentDate = gtk_label_new(stringBuffer.utf8().data());
    if(!m_labelCurrentDate) 
		return FALSE;

    gtk_box_pack_start(GTK_BOX(containerBox), m_labelCurrentDate, FALSE, FALSE, 0);
    containerSubBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    if(!containerSubBox) 
		return FALSE;

    gtk_box_pack_start(GTK_BOX(containerBox), containerSubBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(containerSubBox), m_calendarDateChooser, TRUE, TRUE, 0);

    if(m_inputType == TYPE_WEEK) 
        gtk_calendar_set_display_options(GTK_CALENDAR(m_calendarDateChooser), GtkCalendarDisplayOptions(GTK_CALENDAR_SHOW_WEEK_NUMBERS | GTK_CALENDAR_SHOW_HEADING | GTK_CALENDAR_SHOW_DAY_NAMES));
    
    gtk_calendar_mark_day(GTK_CALENDAR(m_calendarDateChooser), currentTime->tm_mday);    

    if(m_inputType == TYPE_DATE)
        g_signal_connect(GTK_WIDGET(m_calendarDateChooser), "day_selected", G_CALLBACK(dateChooserDayClickedCallback), (gpointer)this);
    else if(m_inputType == TYPE_WEEK)
        g_signal_connect(GTK_WIDGET(m_calendarDateChooser), "day_selected", G_CALLBACK(weekChooserDayClickedCallback), (gpointer)this);
    else if(m_inputType == TYPE_MONTH)
        g_signal_connect(GTK_WIDGET(m_calendarDateChooser), "day_selected", G_CALLBACK(monthChooserDayClickedCallback), (gpointer)this);
    
    g_signal_connect(GTK_WIDGET(m_calendarDateChooser), "day_selected_double_click", G_CALLBACK(dateChooserDayDoubleClickedCallback), (gpointer)this);
    g_signal_connect(GTK_WIDGET(m_window), "delete_event", G_CALLBACK(gtk_widget_hide_on_delete), NULL); 

    gtk_widget_show_all(m_window);   

    return TRUE;
} 	



void DateChooserGtk::reattachDateTimeChooser()
{
    guint year = 0;
    guint month = 0;
    guint day = 0;
    
    if(!m_window || !m_calendarDateChooser || !m_labelCurrentDate)
        return;
    
    gtk_calendar_get_date(GTK_CALENDAR(m_calendarDateChooser), &year, &month, &day);
    String stringBuffer = String::format("%s %d - %d - %d", "Current Date : ", year, CURRENT_MONTH(month), day);
    gtk_label_set_text (GTK_LABEL(m_labelCurrentDate), stringBuffer.utf8().data());   

    gtk_window_present(GTK_WINDOW(m_window));
}


}

#endif
