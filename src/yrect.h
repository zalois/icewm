#ifndef __YRECT_H
#define __YRECT_H

#ifndef INT_MAX
#include <limits.h>
#endif

// change this to use x,y,w,h internal representation?
class YRect {
public:
#if 0
    YRect(): x1(0), y1(0), x2(0), y2(0) {}
#endif
    YRect(int x, int y, unsigned w, unsigned h)
        :xx(x), yy(y), ww(w), hh(h)
    {
        PRECONDITION(ww < INT_MAX);
        PRECONDITION(hh < INT_MAX);
    }

    int x() const { return xx; }
    int y() const { return yy; }
    unsigned width() const { return ww; }
    unsigned height() const { return hh; }

#if 0
    int left() const { return x1; }
    int right() const { return x2; }
    int top() const { return y1; }
    int bottom() const { return y2; }

    void setX(int x) { x1 = x; }
    void setY(int y) { y2 = y; }
    void setWidth(int w) { x2 = x1 + w - 1; }
    void setHeight(int h) { y2 = y1 + h - 1; }

    void setLeft(int x) { x1 = x; }
    void setRight(int x) { x2 = x; }
    void setTop(int y) { y1 = y; }
    void setBottom(int y) { y2 = y; }

#endif

    void setRect(int x, int y, unsigned w, unsigned h) {
        xx = x;
        yy = y;
        ww = w;
        hh = h;
    }

    void setRect(const YRect &r) {
        xx = r.x();
        yy = r.y();
        ww = r.width();
        hh = r.height();
    }

    // does the same as gdk_rectangle_union
    void unionRect(int x, int y, unsigned width, unsigned height) {
        int dest_x, dest_y;

        dest_x = min(xx, x);
        dest_y = min(yy, y);
        ww = max(xx + ww, x + width) - dest_x;
        hh = max(yy + hh, y + height) - dest_y;
        xx = dest_x;
        yy = dest_y;
    }

    //YPoint center() { return YPoint((x1 + x2) / 2,
    //                                (y1 + y2) / 2); }

#if 0
        bool inside(const YPoint &p) {
        return (p.x() >= x1 &&
                p.y() >= y1 &&
                p.x() <= x2 &&
                p.y() <= y2) ? true : false;
        }
#endif

private:
    int xx, yy;
    unsigned ww, hh;
};

#endif

// vim: set sw=4 ts=4 et:
