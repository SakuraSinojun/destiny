
#pragma once


#include "point.h"
#include "mapdef.h"

class map;
class submap
{
public:
    typedef enum {
        st_null = 0,

        st_dirt,
        st_grass,

        st_shrub,
        st_shrub_blueberry,
        st_shrub_strawberry,

        st_tree, st_tree_young, st_tree_apple,

        st_field,

        st_rock,
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

    submap(map* mp, int x, int y);
    ~submap();

    point& pos() { return loc; }

    ster& ter(int x, int y, int z = 0);

private:
    map_layer   layer[OVERMAP_LAYERS];
    point       loc;
    ster        nullret;

    void generate(map* mp);
    void gen_full(ster_id id);
    void gen_field();
    void gen_forest(map* mp);
};
