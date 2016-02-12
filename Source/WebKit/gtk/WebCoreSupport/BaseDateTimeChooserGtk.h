#ifndef BaseDateTimeChooserGtk_h
#define BaseDateTimeChooserGtk_h

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

#include "DateTimeChooser.h"
#include "DateTimeChooserClient.h"
#include "ChromeClient.h"

#include <sys/time.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <wtf/CurrentTime.h>
#include <wtf/MathExtras.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

#define AM "AM"
#define PM "PM"

#define WIDGET_DEFAULT_HEIGHT      300
#define WIDGET_DEFAULT_WIDTH       100

#define NOON				12
#define MIDNIGHT			12

#define TYPE_WEEK           "week"
#define TYPE_MONTH          "month"
#define TYPE_DATE           "date"
#define TYPE_DATETIME       "datetime"
#define TYPE_DATETIME_LOCAL "datetime-local"
#define TYPE_TIME           "time"

#define CURRENT_YEAR(x) x+1900 //x is year after 1900 so plus 1900
#define CURRENT_MONTH(x) x+1 // x is month[0-11] so plus 1

namespace WebCore {

struct PastValues
{
    gint iPastHour;
    gint iPastMin;
    guint uiPastMonth;
    guint uiPastDay;
    guint uiPastYear;
    String sPastAmPm;
};
class ChromeClient;

class BaseDateTimeChooserGtk : public DateTimeChooser {
public:
	BaseDateTimeChooserGtk(ChromeClient* chromeClient, String type);
    ~BaseDateTimeChooserGtk();

	void endChooser(); 
    
    GtkWidget* window() { return m_window; }
    ChromeClient* client() { return m_chromeClient; }  
    String inputType() { return m_inputType; }
    PastValues getPastValues() const { return m_pastValues; }

protected:
    void initialize();
    virtual bool createDateTimeChooserWidget() = 0;
    void setPastValues(guint year = 0, guint month = 0, guint day = 0, gint Hour = 0, gint min = 0, char* ampm = "");

protected:
    ChromeClient*   m_chromeClient;
    GtkWidget*      m_window;
    String          m_inputType;
	PastValues      m_pastValues;
};

} //namespace webcore
#endif
#endif
