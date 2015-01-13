
#pragma once


class point 
{
public:
    int x;
    int y;
    point(int X = 0, int Y = 0) : x (X), y (Y) {}
    point(const point &p) : x (p.x), y (p.y) {}
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


