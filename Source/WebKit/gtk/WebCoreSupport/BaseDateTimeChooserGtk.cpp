#include "config.h"

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

#include "ChromeClientGtk.h"
#include "BaseDateTimeChooserGtk.h"
#include "DateTimeChooserClient.h"

namespace WebCore {

BaseDateTimeChooserGtk::BaseDateTimeChooserGtk(ChromeClient* chromeClient, String type)
 : m_chromeClient(chromeClient)
	, m_window(NULL) 
	, m_inputType(type)
{
    setPastValues();
}

BaseDateTimeChooserGtk::~BaseDateTimeChooserGtk()
{	
    m_window = NULL;
    m_chromeClient = NULL;
}

void BaseDateTimeChooserGtk::endChooser()
{
    if(m_window)
        gtk_widget_destroy(GTK_WIDGET(m_window));
    
    if(m_chromeClient)
        m_chromeClient->removeDateTimeChooser();
}

void BaseDateTimeChooserGtk::setPastValues(guint year, guint month, guint day, gint hour, gint min, char* ampm)
{
    m_pastValues.uiPastYear = year;
    m_pastValues.uiPastMonth = month;
    m_pastValues.uiPastDay = day;
    m_pastValues.iPastHour = hour;
    m_pastValues.iPastMin = min;
    m_pastValues.sPastAmPm = String::format("%s", ampm);
}

void BaseDateTimeChooserGtk::initialize()
{
    if(!createDateTimeChooserWidget()) //if it is failed making widgets                                 
        endChooser();//close chooser         
}

}//end namespace webcore

#endif // ENABLE(DATE_AND_TIME_INPUT_TYPES)

