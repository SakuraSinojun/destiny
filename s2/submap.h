
#pragma once


#include "point.h"
#include "mapdef.h"
#include "overmap.h"

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

        st_water_sh,
        st_water_dp,

        st_sidewalk,
        st_pavement,
        st_pavement_y,

        st_floor,
        st_wall_h,
        st_wall_v,

        st_door_c,
        st_door_o,
        st_door_locked,

        st_window,

        ///////////////////

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
        ster& operator=(const ster& o) { type = o.type; return *this; }
        ster& operator=(ster_id id) { type = id; return *this; }
        bool operator==(ster_id t) { return (type == t); }
        bool operator<(ster_id t) { return (type < t); }
        bool operator>=(ster_id t) { return !operator<(t); }
        bool operator<=(ster_id t) { return operator<(t) || operator==(t); }
        bool operator>(ster_id t) { return !operator<=(t); }

        bool is_wall() const { return (type == st_wall_h || type == st_wall_v); }
        bool is_door() const { return (type == st_door_c || type == st_door_o || type == st_door_locked); }
        bool is_wall_or_door() const { return is_wall() || is_door(); }
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

    bool gen_field(const overmap::oter& oter, map* mp);
    bool gen_forest(const overmap::oter& oter, map* mp);
    bool gen_river(const overmap::oter& oter, map* mp);
    bool gen_road(const overmap::oter& oter, map* mp);
    bool gen_house(const overmap::oter& oter, map* mp);
    void make_house_room(int x0, int y0, int x1, int y1);

    ster_id grass_or_dirt();

    void rotate(int turns, int z = 0);
};















