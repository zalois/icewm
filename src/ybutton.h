#ifndef __YBUTTON_H
#define __YBUTTON_H

#include "ywindow.h"

class YAction;
class YActionListener;
class YMenu;

class YButton: public YWindow {
public:
    YButton(YWindow *parent, YAction *action, YMenu *popup = 0);
    virtual ~YButton();

    virtual void paint(Graphics &g, const YRect &r);
    virtual void paintFocus(Graphics &g, const YRect &r);
    virtual bool handleKey(const XKeyEvent &key);
    virtual void handleButton(const XButtonEvent &button);
    virtual void handleCrossing(const XCrossingEvent &crossing);

    void setAction(YAction * action);
    void setPopup(YMenu * popup);
    void setImage(YIconImage * image);
    void setText(const char * str, int hot = -1);

    void setPressed(int pressed);
    virtual bool isFocusTraversable();

    virtual void donePopup(YPopupWindow *popup);

    void popupMenu();
    virtual void updatePopup();

    void setActionListener(YActionListener *listener) { fListener = listener; }
    YActionListener *getActionListener() const { return fListener; }

    void setSelected(bool selected);
    void setOver(bool over);

    void setArmed(bool armed, bool mousedown);
    bool isPressed() const { return fPressed; } 
    bool isSelected() const { return fSelected; }
    bool isArmed() const { return fArmed; }
    bool isPopupActive() const { return fPopupActive; }

    virtual void actionPerformed(YAction *action, unsigned int modifiers);
    virtual YFont * getFont();
    virtual YColor * getColor();
    virtual YSurface getSurface();

    bool fOver;
    
private:
    void paint(Graphics &g, int const d, const YRect &r);

    YAction *fAction;
    YMenu *fPopup;
    YIconImage *fImage;
    char *fText;
    int fPressed;
    int fHotCharPos;
    int hotKey;

    YActionListener *fListener;

    bool fSelected;
    bool fArmed;
    bool wasPopupActive;
    bool fPopupActive;

    void popup(bool mouseDown);
    void popdown();

    static YColor *normalButtonBg;
    static YColor *normalButtonFg;
    
    static YColor *activeButtonBg;
    static YColor *activeButtonFg;
    
    static YFont *normalButtonFont;
    static YFont *activeButtonFont;
};

extern YPixmap *buttonIPixmap;
extern YPixmap *buttonAPixmap;

#ifdef CONFIG_GRADIENTS
extern class YPixbuf *buttonIPixbuf;
extern class YPixbuf *buttonAPixbuf;
#endif

#endif
