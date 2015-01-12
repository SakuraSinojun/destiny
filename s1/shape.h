
#pragma once

template <typename T>
class DPoint {
public:
    DPoint(T x_, T y_) : x(x_), y(y_) {}
    DPoint() : x(0), y(0) {}
    ~DPoint() {}
    T x;
    T y;
    bool operator==(const DPoint<T>& o) {
        return (x == o.x && y == o.y);
    }
    DPoint<T>& operator=(const DPoint<T>& o) {
        x = o.x;
        y = o.y;
        return *this;
    }
};

template <typename T>
DPoint<T> mkpoint(T x, T y) { return DPoint<T>(x, y); }


template <typename T>
class DSize {
public:
    DSize() : w(0), h(0) {}
    DSize(T w_, T h_) : w(w_), h(h_) {}
    ~DSize() {}
    T w;
    T h;
};

template <typename T>
class DRect {
public:
    DRect() : l(0), t(0), r(0), b(0) {}
    DRect(T x, T y, T w, T h)
        : l(x), t(y), r(x + w), b(y + h)
    {
    }

    DRect(DPoint<T>& p, DSize<T>& s)
        : l(p.x), t(p.y), r(p.x + s.w), b(p.y + s.h)
    {
    }

    T Width(void) { return r - l; }
    T Height(void) { return b - t; }

    T l;
    T t;
    T r;
    T b;
};

template <typename T>
class DBezier {
public:
    DBezier() {}
    DBezier(DPoint<T> p0, DPoint<T> p1, DPoint<T> p2, DPoint<T> p3)
        : P0(p0), P1(p1), P2(p2), P3(p3)
    {
    }
    ~DBezier() {}
    DPoint<T>  P0, P1, P2, P3;
    void set(int index, DPoint<T> p) {
        switch (index) {
        case 0:
            P0 = p;
            break;
        case 1:
            P1 = p;
            break;
        case 2:
            P2 = p;
            break;
        case 3:
            P3 = p;
            break;
        case -1:
            P0 = P1 = P2 = P3 = p;
            break;
        default:
            break;
        }
    }
    DPoint<T>  PointOnCubicBezier(float t) {
        // Bezier:
        // B(t) = P0 * (1 - t) ^ 3
        //        + 3 * P1 * t * (1 - t) ^ 2
        //        + 3 * P2 * (t ^ 2) * (1 - t)
        //        + P3 * (t ^ 3)
        //      = (3P1 + P3 - P0 - 3P2) t^3
        //        + (3P0 -6P1 + 3P2) t^2
        //        + (3P1 - 3P0) t
        //        + P0

        float   ax, bx, cx, ay, by, cy;
        float   tSquared, tCubed;
        DPoint<T> result;

        cx = 3 * (P1.x - P0.x);
        bx = 3 * (P2.x - P1.x) - cx;
        ax = P3.x - P0.x - cx - bx;
        cy = 3 * (P1.y - P0.y);
        by = 3 * (P2.y - P1.y) - cy;
        ay = P3.y - P0.y -cy - by;

        tSquared = t * t;
        tCubed = tSquared * t;
        result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + P0.x;
        result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + P0.y;
        return result;
    }
};



