#ifndef DateTimeChooserGtk_h
#define DateTimeChooserGtk_h

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

#include "ChromeClient.h"
#include "BaseDateTimeChooserGtk.h"

namespace WebCore {

class ChromeClient;
struct pastValues; 

class DateTimeChooserGtk : public BaseDateTimeChooserGtk
{
public:
    DateTimeChooserGtk(ChromeClient* chromeClient, String type);
    ~DateTimeChooserGtk();

    void reattachDateTimeChooser();

	GtkWidget* calendar() { return m_calendarDateChooser; }
    GtkWidget* combobox() { return m_comboboxAmPm; }
    GtkWidget* spinbuttonHour() { return m_spinbuttonHour;}
    GtkWidget* spinbuttonMinute() { return m_spinbuttonMinute;}
    
protected:
    bool createDateTimeChooserWidget();
    
private:
	GtkWidget* m_calendarDateChooser;
    GtkWidget* m_comboboxAmPm;
    GtkWidget* m_spinbuttonHour;
    GtkWidget* m_spinbuttonMinute;
    GtkWidget* m_buttonOk;
    GtkWidget* m_buttonCancle;
    GtkWidget* m_labelCurrentDate;
    GtkWidget* m_labelCurrentTime;
};

} //namespace webcore
#endif	//ENABLE(DATE_AND_TIME_INPUT_TYPES)
#endif
