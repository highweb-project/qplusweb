#ifndef ColorChooserGtk_h
#define ColorChooserGtk_h

#if ENABLE(INPUT_TYPE_COLOR)

#include "ColorChooser.h"
#include "ChromeClient.h"

namespace WebCore {

class Color;
class ChromeClient;

class ColorChooserGtk : public ColorChooser {
public:	
    ColorChooserGtk(ChromeClient* chromeclient, const Color& color);
    ~ColorChooserGtk();

    void showColorChooser();        //[2013.09.14][infraware][hyunseok] add method
    void reattachColorChooser(const Color& color);
    void setSelectedColor(const Color& color);
    void endChooser();

    ChromeClient* client() { return m_chromeClient; }
    GtkWidget*  dialog() { return m_colorSelectionDialog; }

private:
    ChromeClient* m_chromeClient;
    GtkWidget* 	m_colorSelectionDialog = NULL;
};

} // namespace WebCore

#endif // ENABLE(INPUT_TYPE_COLOR)

#endif // ColorChooserGtk_h
