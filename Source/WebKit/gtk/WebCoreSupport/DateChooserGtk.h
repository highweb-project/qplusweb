#ifndef DateChooserGtk_h
#define DateChooserGtk_h

#if ENABLE(INPUT_TYPE_COLOR)

#include "DateTimeChooser.h"
#include "DateTimeChooserClient.h"
#include "DateTimeChooserGtk.h"
#include "BaseDateTimeChooserGtk.h"
#include "ChromeClient.h"

namespace WebCore {

class ChromeClient;

class DateChooserGtk : public BaseDateTimeChooserGtk {
public:
    DateChooserGtk(ChromeClient* chromeClient, String type);
    ~DateChooserGtk();
    
    void reattachDateTimeChooser();

    GtkWidget* calendar() { return m_calendarDateChooser; }

protected:
    virtual bool createDateTimeChooserWidget();
    

private:
	GtkWidget* m_calendarDateChooser;
	GtkWidget* m_labelCurrentDate;    
};

} //namespace webcore
#endif
#endif

 
