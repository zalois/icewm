/*
 * IceWM
 *
 * Copyright (C) 1997-2001 Marko Macek
 *
 * ScrollBar
 */
#include "config.h"

#ifndef LITE

#include "ykey.h"
#include "yscrollbar.h"

#include "yapp.h"
#include "prefs.h"

YColor *scrollBarBg = 0;
static YColor *scrollBarArrow = 0;
static YColor *scrollBarSlider = 0;
static YColor *scrollBarButton = 0;
static bool didInit = false;

YTimer *YScrollBar::fScrollTimer = 0;

static void initColors() {
    if (didInit)
        return ;
    scrollBarBg = new YColor(clrScrollBar);
    scrollBarArrow = new YColor(clrScrollBarArrow);
    scrollBarSlider= new YColor(clrScrollBarSlider);
    scrollBarButton= new YColor(clrScrollBarButton);
    didInit = true;
}

YScrollBar::YScrollBar(YWindow *aParent): YWindow(aParent) {
    if (!didInit) initColors();
    fOrientation = Vertical;
    fMinimum = fMaximum = fValue = fVisibleAmount = 0;
    fUnitIncrement = fBlockIncrement = 1;
    fListener = 0;
    fScrollTo = goNone;
    fDNDScroll = false;
}


YScrollBar::YScrollBar(Orientation anOrientation, YWindow *aParent):
    YWindow(aParent)
{
    if (!didInit) initColors();
    fOrientation = anOrientation;

    fMinimum = fMaximum = fValue = fVisibleAmount = 0;
    fUnitIncrement = fBlockIncrement = 1;
    fListener = 0;
    fScrollTo = goNone;
    fDNDScroll = false;
}

YScrollBar::YScrollBar(Orientation anOrientation,
                       int aValue, int aVisibleAmount, int aMin, int aMax,
                       YWindow *aParent): YWindow(aParent)
{
    if (!didInit) initColors();
    fOrientation = anOrientation;
    fMinimum = aMin;
    fMaximum = aMax;
    fVisibleAmount = aVisibleAmount;
    fValue = aValue;

    fUnitIncrement = fBlockIncrement = 1;
    fListener = 0;
    fScrollTo = goNone;
}

YScrollBar::~YScrollBar() {
    if (fScrollTimer && fScrollTimer->getTimerListener() == this)
        fScrollTimer->setTimerListener(0);
}

void YScrollBar::setOrientation(Orientation anOrientation) {
    if (anOrientation != fOrientation) {
        fOrientation = anOrientation;
        repaint();
    }
}

void YScrollBar::setMaximum(int aMaximum) {
    if (aMaximum < fMinimum)
        aMaximum = fMinimum;

    if (aMaximum != fMaximum) {
        fMaximum = aMaximum;
        repaint();
    }
}

void YScrollBar::setMinimum(int aMinimum) {
    if (aMinimum > fMaximum)
        aMinimum = fMaximum;

    if (aMinimum != fMinimum) {
        fMinimum = aMinimum;
        repaint();
    }
}

void YScrollBar::setVisibleAmount(int aVisibleAmount) {
    if (fVisibleAmount > fMaximum - fMinimum)
        fVisibleAmount = fMaximum - fMinimum;
    if (fVisibleAmount < 0)
        fVisibleAmount = 0;

    if (aVisibleAmount != fVisibleAmount) {
        fVisibleAmount = aVisibleAmount;
        repaint();
    }
}

void YScrollBar::setUnitIncrement(int anUnitIncrement) {
    fUnitIncrement = anUnitIncrement;
}

void YScrollBar::setBlockIncrement(int aBlockIncrement) {
    fBlockIncrement = aBlockIncrement;
}

void YScrollBar::setValue(int aValue) {
    if (aValue > fMaximum - fVisibleAmount)
        aValue = fMaximum - fVisibleAmount;
    if (aValue < fMinimum)
        aValue = fMinimum;

    if (aValue != fValue) {
        fValue = aValue;
        repaint();
    }
}

void YScrollBar::setValues(int aValue, int aVisibleAmount, int aMin, int aMax) {
    int v = aValue;

    if (aMax < aMin)
        aMax = aMin;
    if (aVisibleAmount > aMax - aMin)
        aVisibleAmount = aMax - aMin;
    if (aVisibleAmount < 0)
        aVisibleAmount = 0;
    if (aValue > aMax - aVisibleAmount)
        aValue = aMax - aVisibleAmount;
    if (aValue < aMin)
        aValue = aMin;

    if (aMax != fMaximum ||
        aMin != fMinimum ||
        aValue != fValue ||
        aVisibleAmount != fVisibleAmount)
    {
        fMinimum = aMin;
        fMaximum = aMax;
        fValue = aValue;
        fVisibleAmount = aVisibleAmount;
        repaint();
        if (v != fValue)
            fListener->move(this, fValue);
    }
}

void YScrollBar::drawArrow(Graphics &g, bool forward) {
    //!!! optimize this
    if (fOrientation == Vertical) {
        if (forward) {
            int w = width();
            int m = 7;
            int d = 5;
            int t = w / 2 + d / 2 + ((fScrollTo == goDown) ? 1 : 0);
            t += height() - width();
            int b = t - d;


            if (fValue < fMaximum - fVisibleAmount)
                g.setColor(YColor::black);
            else
                g.setColor(YColor::white);

            g.drawLine(m - d, b, m, t);
            g.drawLine(m - d + 1, b, m + 1, t);
            g.drawLine(m + d + 1, b, m + 1, t);
            g.drawLine(m + d, b, m, t);
        } else {
            int w = width();
            int m = 7;
            int d = 5;
            int t = w / 2 - d / 2 - ((fScrollTo == goUp) ? 1 : 0);
            int b = t + d;

            if (fValue > fMinimum)
                g.setColor(YColor::black);
            else
                g.setColor(YColor::white);

            g.drawLine(m - d, b, m, t);
            g.drawLine(m - d + 1, b, m + 1, t);
            g.drawLine(m + d + 1, b, m + 1, t);
            g.drawLine(m + d, b, m, t);
        }
    } else {
        if (forward) {
            int h = height();
            int m = 7;
            int d = 5;
            int l = h / 2 + d / 2 + ((fScrollTo == goDown) ? 1 : 0);
            l += width() - height();
            int b = l - d;

            if (fValue < fMaximum - fVisibleAmount)
                g.setColor(YColor::black);
            else
                g.setColor(YColor::white);

            g.drawLine(b, m - d, l, m);
            g.drawLine(b, m - d + 1, l, m + 1);
            g.drawLine(b, m + d + 1, l, m + 1);
            g.drawLine(b, m + d, l, m);
        } else {
            int h = height();
            int m = 7;
            int d = 5;
            int l = h / 2 - d / 2 - ((fScrollTo == goUp) ? 1 : 0);
            int b = l + d;

            if (fValue > fMinimum)
                g.setColor(YColor::black);
            else
                g.setColor(YColor::white);

            g.drawLine(b, m - d, l, m);
            g.drawLine(b, m - d + 1, l, m + 1);
            g.drawLine(b, m + d + 1, l, m + 1);
            g.drawLine(b, m + d, l, m);
        }
    }
}

void YScrollBar::getCoord(int &beg, int &end, int &min, int &max, int &nn) {
    int dd = (fMaximum - fMinimum);

    if (fOrientation == Vertical) {
        beg = scrollBarHeight;
        end = height() - scrollBarHeight - 1;
    } else {
        beg = scrollBarHeight;
        end = width() - scrollBarHeight - 1;
    }
    
    nn = end - beg;
    if (dd <= 0) {
        min = 0;
        max = nn;
        return ;
    }
    int aa = nn;
    int vv = aa * fVisibleAmount / dd;
    if (vv < SCROLLBAR_MIN) {
        vv = SCROLLBAR_MIN;
        aa = nn - vv;
        if (aa < 0) aa = 0;
        dd -= fVisibleAmount;
        if (dd <= 0) {
            min = 0;
            max = nn;
            return ;
        }
    }
    if (vv > nn)
        vv = nn;
    min = aa * fValue / dd;
    max = min + vv;
}

// !!!! TODO: Warp3, Warp4, Motiv borders
// !!!! TODO: Insensitive buttons

void YScrollBar::paint(Graphics &g, int /*x*/, int /*y*/, unsigned int /*width*/, unsigned int /*height*/) {
    int beg, end, min, max, nn;

    getCoord(beg, end, min, max, nn);

    /// !!! optimize this
    if (fOrientation == Vertical) {
	g.setColor(scrollBarBg); // -------------------- background, buttons ---

	switch(wmLook) {
	    case lookWin95:
	    case lookWarp3:
		g.fillRect(0, 0, width(), height());
		
		g.setColor(scrollBarButton);
		g.drawBorderW(0, 0, width() - 1, beg - 1, fScrollTo != goUp);
		g.fillRect(1, 1, width() - 3, beg - 3);

		g.drawBorderW(0, end + 1, width() - 1, beg - 1, fScrollTo != goDown);
		g.fillRect(1, end + 2, width() - 3, beg - 3);
		break;
		
	    case lookNice:
	    case lookPixmap:
	    case lookWarp4:
		g.drawBorderW(0, 0, width() - 1, height() - 1, false);
		g.fillRect(1, 1, width() - 2, height() - 2);
		
		g.setColor(scrollBarButton);
		g.drawBorderW(1, 1, width() - 3, beg - 2, fScrollTo != goUp);
		g.fillRect(2, 2, width() - 5, beg - 4);

		g.drawBorderW(1, end + 1, width() - 3, beg - 2, fScrollTo != goDown);
		g.fillRect(2, end + 2, width() - 5, beg - 4);
		break;
		
	    case lookMotif:
		g.drawBorderW(0, 0, width() - 1, height() - 1, false);
		g.fillRect(2, 2, width() - 3, height() - 3);
		break;

	    case lookGtk:
		g.drawBorderG(0, 0, width() - 1, height() - 1, false);
		g.fillRect(2, 2, width() - 3, height() - 3);
		break;

	    case lookMetal:
		g.fillRect(0, 0, width(), height());

		g.setColor(scrollBarButton);
		g.drawBorderM(0, 0, width() - 1, beg - 1, fScrollTo != goUp);
		g.fillRect(2, 2, width() - 5, beg - 5);
		g.drawBorderM(0, end + 1, width() - 1, beg - 1, fScrollTo != goDown);
		g.fillRect(2, end + 3, width() - 5, beg - 5);
		break;
		
	    case lookMAX:
		break;
	}

	g.setColor(scrollBarSlider); // ----------------------------- slider ---
	const int y(beg + min), h(max - min);
	
	switch(wmLook) {
	    case lookWin95:
	    case lookWarp3:
		g.drawBorderW(0, y, width() - 1, h, true);
		g.fillRect(1, y + 1, width() - 3, h - 2);
		break;
		
	    case lookNice:
	    case lookPixmap:
	    case lookWarp4:
		g.drawBorderW(1, y, width() - 3, h, true);
		g.fillRect(2, y + 1, width() - 5, h - 2);
		
		g.setColor(scrollBarBg->darker());
		for(int hy(y + h / 2 - 6); hy < (y + h / 2 + 5); hy+= 2)
		    g.drawLine(4, hy, width() - 6, hy);
		g.setColor(scrollBarBg->brighter());
		for(int hy(y + h / 2 - 5); hy < (y + h / 2 + 6); hy+= 2)
		    g.drawLine(4, hy, width() - 6, hy);
		
		break;

	    case lookMotif:
		g.drawBorderW(2, y - 1, width() - 5, h + 3, true);
		g.fillRect(3, y, width() - 7, h + 1);
		break;

	    case lookGtk:
		g.drawBorderG(2, y - 1, width() - 5, h + 3, true);
		g.fillRect(3, y, width() - 7, h + 1);
		break;
		
	    case lookMetal:
		g.drawBorderM(0, y, width() - 1, h, true);
		g.fillRect(2, y + 2, width() - 4, h - 3);
		break;
		
	    case lookMAX:
		break;
	}

        g.setColor(scrollBarArrow); // ------------------------------ arrows ---
	switch(wmLook) {
	    case lookWin95:
	    case lookWarp3:
	    case lookWarp4:
		g.drawArrow(Up, 3, (beg - width() + 10) / 2,
			    width() - 8, fScrollTo == goUp);
		g.drawArrow(Down, 3, end + (beg - width() + 12) / 2,
			    width() - 8, fScrollTo == goDown);
		break;
		
	    case lookNice:
	    case lookPixmap:
		g.drawArrow(Up, 4, (beg - width() + 10) / 2,
			    width() - 10, fScrollTo == goUp);
		g.drawArrow(Down, 4, end + (beg - width() + 12) / 2,
			    width() - 10, fScrollTo == goDown);
		break;
		
	    case lookMotif:
	    case lookGtk:
		g.drawArrow(Up, 2, 2, width() - 5, fScrollTo == goUp);
		g.drawArrow(Down, 2, end + 2, width() - 5, fScrollTo == goDown);
		break;
		
	    case lookMetal:
		g.drawArrow(Up, 4, (beg - width() + 12) / 2,
			    width() - 8, fScrollTo == goUp);
		g.drawArrow(Down, 4, end + (beg - width() + 14) / 2,
			    width() - 8, fScrollTo == goDown);
		break;
		
	    case lookMAX:
		break;
	}
    } else {
	g.setColor(scrollBarBg); // -------------------- background, buttons ---

	switch(wmLook) {
	    case lookWin95:
	    case lookWarp3:
		g.fillRect(0, 0, width(), height());
		
		g.setColor(scrollBarButton);
		g.drawBorderW(0, 0, beg - 1, height() - 1, fScrollTo != goUp);
		g.fillRect(1, 1, beg - 3, height() - 3);

		g.drawBorderW(end + 1, 0, beg - 1, height() - 1, fScrollTo != goDown);
		g.fillRect(end + 2, 1, beg - 3, height() - 3);
		break;
		
	    case lookNice:
	    case lookPixmap:
	    case lookWarp4:
		g.drawBorderW(0, 0, width() - 1, height() - 1, false);
		g.fillRect(1, 1, width() - 2, height() - 2);
		
		g.setColor(scrollBarButton);
		g.drawBorderW(1, 1, beg - 2, height() - 3, fScrollTo != goUp);
		g.fillRect(2, 2, beg - 4, height() - 5);

		g.drawBorderW(end + 1, 1, beg - 2, height() - 3, fScrollTo != goDown);
		g.fillRect(end + 2, 2, beg - 4, height() - 5);
		break;
		
	    case lookMotif:
		g.drawBorderW(0, 0, width() - 1, height() - 1, false);
		g.fillRect(2, 2, width() - 3, height() - 3);
		break;

	    case lookGtk:
		g.drawBorderG(0, 0, width() - 1, height() - 1, false);
		g.fillRect(2, 2, width() - 3, height() - 3);
		break;

	    case lookMetal:
		g.fillRect(0, 0, width(), height());

		g.setColor(scrollBarButton);
		g.drawBorderM(0, 0, beg - 1, height() - 1, fScrollTo != goUp);
		g.fillRect(2, 2, beg - 5, height() - 5);
		g.drawBorderM(end + 1, 0, beg - 1, height() - 1, fScrollTo != goDown);
		g.fillRect(end + 3, 2, beg - 5, height() - 5);
		break;
		
	    case lookMAX:
		break;
	}

	g.setColor(scrollBarSlider); // ----------------------------- slider ---
	const int x(beg + min), w(max - min);
	
	switch(wmLook) {
	    case lookWin95:
	    case lookWarp3:
		g.drawBorderW(x, 0, w, height() - 1, true);
		g.fillRect(x + 1, 1, w - 2, height() - 3);
		break;
		
	    case lookNice:
	    case lookPixmap:
	    case lookWarp4:
		g.drawBorderW(x, 1, w, height() - 3, true);
		g.fillRect(x + 1, 2, w - 2, height() - 5);
		
		g.setColor(scrollBarBg->darker());
		for(int hx(x + w / 2 - 6); hx < (x + w / 2 + 5); hx+= 2)
		    g.drawLine(hx, 4, hx, height() - 6);
		g.setColor(scrollBarBg->brighter());
		for(int hx(x + w / 2 - 5); hx < (x + w / 2 + 6); hx+= 2)
		    g.drawLine(hx, 4, hx, height() - 6);
		
		break;

	    case lookMotif:
		g.drawBorderW(x - 1, 2, w + 3, height() - 5, true);
		g.fillRect(x, 3, w + 1, height() - 7);
		break;

	    case lookGtk:
		g.drawBorderG(x - 1, 2, w + 3, height() - 5, true);
		g.fillRect(x, 3, w + 1, height() - 7);
		break;
		
	    case lookMetal:
		g.drawBorderM(x, 0, w, height() - 1, true);
		g.fillRect(x + 2, 2, w - 3, height() - 4);
		break;
		
	    case lookMAX:
		break;
	}

        g.setColor(scrollBarArrow); // ------------------------------ arrows ---
	switch(wmLook) {
	    case lookWin95:
	    case lookWarp3:
	    case lookWarp4:
		g.drawArrow(Left, (beg - height() + 10) / 2, 3,
			    height() - 8, fScrollTo == goUp);
		g.drawArrow(Right, end + (beg - height() + 12) / 2, 3,
			    height() - 8, fScrollTo == goDown);
		break;
		
	    case lookNice:
	    case lookPixmap:
		g.drawArrow(Left, (beg - height() + 10) / 2, 4,
			    height() - 10, fScrollTo == goUp);
		g.drawArrow(Right, end + (beg - height() + 12) / 2, 4,
			    height() - 10, fScrollTo == goDown);
		break;
		
	    case lookMotif:
	    case lookGtk:
		g.drawArrow(Left, 2, 2, height() - 5, fScrollTo == goUp);
		g.drawArrow(Right, end + 2, 2, height() - 5,
			    fScrollTo == goDown);
		break;
		
	    case lookMetal:
		g.drawArrow(Left, (beg - height() + 12) / 2, 4,
			    height() - 8, fScrollTo == goUp);
		g.drawArrow(Right, end + (beg - height() + 14) / 2, 4,
			    height() - 8, fScrollTo == goDown);
		break;
		
	    case lookMAX:
		break;
	}
    }
}

void YScrollBar::scroll(int delta) {
    int fNewPos = fValue;

    if (delta == 0)
        return ;

    fNewPos += delta;

    if (fNewPos >= fMaximum - fVisibleAmount)
        fNewPos = fMaximum - fVisibleAmount;
    if (fNewPos < fMinimum)
        fNewPos = fMinimum;
    if (fNewPos != fValue) {
        delta = fNewPos - fValue;
        fValue = fNewPos;
        repaint();
        fListener->scroll(this, delta);
    }
}

void YScrollBar::move(int pos) {
    int fNewPos = pos;

    if (fNewPos >= fMaximum - fVisibleAmount)
        fNewPos = fMaximum - fVisibleAmount;
    if (fNewPos < fMinimum)
        fNewPos = fMinimum;
    if (fValue != fNewPos) {
        fValue = fNewPos;
        repaint();
        fListener->move(this, fValue);
    }
}

void YScrollBar::handleButton(const XButtonEvent &button) {
    if (handleScrollMouse(button) == true)
        return ;

    if (button.button != 1)
        return ;

    if (fMinimum >= fMaximum)
        return ;

    if (button.type == ButtonPress) {
        fScrollTo = getOp(button.x, button.y);
        doScroll();
        if (fScrollTimer == 0)
            fScrollTimer = new YTimer(scrollBarStartDelay);
        if (fScrollTimer) {
            fScrollTimer->setInterval(scrollBarStartDelay);
            fScrollTimer->setTimerListener(this);
            fScrollTimer->startTimer();
        }
        repaint();
    } else if (button.type == ButtonRelease) {
        fScrollTo = goNone;
        if (fScrollTimer && fScrollTimer->getTimerListener() == this) {
            fScrollTimer->setTimerListener(0);
            fScrollTimer->stopTimer();
        }
        repaint();
    }
}

void YScrollBar::handleMotion(const XMotionEvent &motion) {
    if (motion.state != Button1Mask)
        return ;

    if (fOrientation == Vertical) {
        int h = height() - 2 * width();

        if (h <= 0 || fMinimum >= fMaximum)
            return ;

        int min, max;

        min = h * fValue / (fMaximum - fMinimum);
        max = h * (fValue + fVisibleAmount) / (fMaximum - fMinimum);

        if (fScrollTo == goPosition) {
            int y = motion.y - fGrabDelta - width();
            if (y < 0) y = 0;
            int pos = y * (fMaximum - fMinimum) / h;
            move(pos);
        }
    } else {
        int w = width() - 2 * height();

        if (w <= 0 || fMinimum >= fMaximum)
            return ;

        int min, max;

        min = w * fValue / (fMaximum - fMinimum);
        max = w * (fValue + fVisibleAmount) / (fMaximum - fMinimum);

        if (fScrollTo == goPosition) {
            int x = motion.x - fGrabDelta - height();
            if (x < 0) x = 0;
            int pos = x * (fMaximum - fMinimum) / w;
            move(pos);
        }
    }
}

bool YScrollBar::handleScrollKeys(const XKeyEvent &key) {
    if (key.type == KeyPress) {
        KeySym k = XKeycodeToKeysym(app->display(), key.keycode, 0);
        int m = KEY_MODMASK(key.state);

        switch (k) {
        case XK_Prior:
            if (m & ControlMask)
                move(0);
            else
                scroll(-fBlockIncrement);
            return true;
        case XK_Next:
            if (m & ControlMask)
                move(fMaximum - fVisibleAmount);
            else
                scroll(+fBlockIncrement);
            return true;
        }
    }
    return false;
}

bool YScrollBar::handleScrollMouse(const XButtonEvent &button) {
    if (button.type == ButtonPress) {
        if (button.button == 4) {
            scroll(-fBlockIncrement);
            return true;
        } else if (button.button == 5) {
            scroll(+fBlockIncrement);
            return true;
        }
    }
    return false;
}

void YScrollBar::doScroll() {
    switch (fScrollTo) {
    case goNone:
        break;
    case goUp:
        scroll(-fUnitIncrement);
        break;
    case goDown:
        scroll(+fUnitIncrement);
        break;
    case goPageUp:
        scroll(-fBlockIncrement);
        break;
    case goPageDown:
        scroll(+fBlockIncrement);
        break;
    case goPosition:
        break;
    }
}

bool YScrollBar::handleTimer(YTimer *timer) {
    if (timer != fScrollTimer)
        return false;
    doScroll();
    if (!fDNDScroll || (fScrollTo != goPageUp && fScrollTo != goPageDown))
        timer->setInterval(scrollBarDelay);
    return true;
}

YScrollBar::ScrollOp YScrollBar::getOp(int x, int y) {
    int beg, end, min, max, nn;
    ScrollOp fScrollTo;

    getCoord(beg, end, min, max, nn);

    fScrollTo = goNone;
    if (fOrientation == Vertical) {

        if (y > end) {
            if (fValue < fMaximum - fVisibleAmount)
                fScrollTo = goDown;
        } else if (y < beg) {
            if (fValue > 0)
                fScrollTo = goUp;
        } else {
            if (y < beg + min) {
                fScrollTo = goPageUp;
            } else if (y > beg + max) {
                fScrollTo = goPageDown;
            } else {
                fScrollTo = goPosition;
                fGrabDelta = y - min - beg;
            }
        }
    } else {
        if (x > end) {
            if (fValue < fMaximum - fVisibleAmount)
                fScrollTo = goDown;
        } else if (x < beg) {
            if (fValue > 0)
                fScrollTo = goUp;
        } else {
            if (x < beg + min) {
                fScrollTo = goPageUp;
            } else if (x > beg + max) {
                fScrollTo = goPageDown;
            } else {
                fScrollTo = goPosition;
                fGrabDelta = x - min - beg;
            }
        }
    }
    return fScrollTo;
}

void YScrollBar::handleDNDEnter() {
    fScrollTo = goNone;
    fDNDScroll = true;
    if (fScrollTimer && fScrollTimer->getTimerListener() == this) {
        fScrollTimer->setTimerListener(0);
        fScrollTimer->stopTimer();
    }
}

void YScrollBar::handleDNDLeave() {
    fScrollTo = goNone;
    fDNDScroll = false;
    repaint();
    if (fScrollTimer && fScrollTimer->getTimerListener() == this) {
        fScrollTimer->setTimerListener(0);
        fScrollTimer->stopTimer();
    }
}

void YScrollBar::handleDNDPosition(int x, int y) {
    fScrollTo = getOp(x, y);
    if (fScrollTimer == 0)
        fScrollTimer = new YTimer(scrollBarStartDelay);
    if (fScrollTimer) {
        fScrollTimer->setInterval(scrollBarStartDelay);
        fScrollTimer->setTimerListener(this);
        fScrollTimer->startTimer();
    }
    repaint();
}
#endif
