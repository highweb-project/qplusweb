#ifndef TimeChooserGtk_h
#define TimeChooserGtk_h

#if ENABLE(INPUT_TYPE_COLOR)

#include "DateTimeChooser.h"
#include "DateTimeChooserClient.h"
#include "BaseDateTimeChooserGtk.h"
#include "ChromeClient.h"

namespace WebCore {

class ChromeClient;
struct pastValues;

class TimeChooserGtk : public BaseDateTimeChooserGtk {
public:
	TimeChooserGtk(ChromeClient* chromeClient, String type);
	~TimeChooserGtk();

    void reattachDateTimeChooser();

    GtkWidget* combobox() { return m_comboboxAmPm; }
    GtkWidget* spinbuttonHour() { return m_spinbuttonHour; }
    GtkWidget* spinbuttonMinute() { return m_spinbuttonMinute; }

protected:
    bool createDateTimeChooserWidget();
   
private:
    GtkWidget* m_comboboxAmPm;
    GtkWidget* m_spinbuttonHour;
    GtkWidget* m_spinbuttonMinute;
    GtkWidget* m_buttonOk;
    GtkWidget* m_buttonCancle;
    GtkWidget* m_labelCurrentTime;
    };
} //namespace webcore
#endif
#endif
