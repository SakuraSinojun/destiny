
#pragma once


#include "point.h"
#include "mapdef.h"

class overmap;
class submap
{
public:
    typedef enum {
        st_null = 0,
        st_rock,
        st_field,
        st_river,
        st_road,
        st_house,
    } ster_id;

    class ster {
    public:
        ster_id type;
        ster() : type(st_null) {}
        ster(ster_id id) : type(id) {}
        ster& operator=(ster_id id) { type = id; return *this; }
        bool operator==(ster_id t) { return (type == t); }
        bool operator<(ster_id t) { return (type < t); }
        bool operator>=(ster_id t) { return !operator<(t); }
        bool operator<=(ster_id t) { return operator<(t) || operator==(t); }
        bool operator>(ster_id t) { return !operator<=(t); }
    };
    
    class map_layer {
    public:
        ster    terrain[SMAPX][SMAPY];
    };

    submap(overmap* om, int x, int y);
    ~submap();

    point& pos() { return loc; }

    ster& ter(int x, int y, int z = 0);

private:
    map_layer   layer[OVERMAP_LAYERS];
    point       loc;
    const int   otx;
    const int   oty;
    ster        nullret;

    void generate(overmap* om);
};

