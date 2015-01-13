
#pragma once

#include "point.h"

#define OMAPX   180
#define OMAPY   180

#define OVERMAP_DEPTH   10
#define OVERMAP_HEIGHT  0
#define OVERMAP_LAYERS  (1 + OVERMAP_DEPTH + OVERMAP_HEIGHT)

class game;
class overmap
{
public:
    typedef enum {
        ot_null = 0,
        ot_field,
        ot_river_center,
        ot_rock,
    } oter_id;
    class oter {
    public:
        oter_id type;
        oter() : type(ot_null) {}
        oter(oter_id id) : type(id) {}
        oter& operator=(oter_id t) { type = t; return *this; }
        bool operator==(oter_id t) { return (type == t); }
        bool is_river() { return type == ot_river_center; }
    };

    class map_layer {
    public:
        oter    terrain[OMAPX][OMAPY];
        bool    visible[OMAPX][OMAPY];
    };

    overmap(game& g, int x, int y);
    virtual ~overmap();

    point& pos() { return loc; }

private:
    friend class map;
    game&       m_game;
    point       loc;
    map_layer   layer[OVERMAP_LAYERS];
    oter        nullret;

    void init_layers();
    void load();
    void generate(overmap* north, overmap* west, overmap* south, overmap* east);
    void place_river(point pa, point pb);

    oter& ter(int x, int y, int z = 0);

};

