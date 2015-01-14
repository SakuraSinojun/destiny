
#pragma once

// #include "tools/logging.h"

#include <iostream>

class point 
{
public:
    int x;
    int y;
    point(int X = 0, int Y = 0) : x (X), y (Y) {}
    point(const point &p) : x (p.x), y (p.y) {}
    void set(int X, int Y) { x = X; y = Y; }
    bool operator==(const point& o)  { return (x == o.x && y == o.y); }
    bool operator!=(const point& o)  { return !operator==(o); }
    ~point(){}
};

class tripoint 
{
public:
    int x;
    int y;
    int z;
    tripoint(int X = 0, int Y = 0, int Z = 0) : x (X), y (Y), z (Z) {}
    tripoint(const tripoint &p) : x (p.x), y (p.y), z (p.z) {}
    ~tripoint(){}
};

std::ostream& operator<<(std::ostream& o, const point& pt);
// std::ostream& operator<<(std::ostream& o, point pt);


