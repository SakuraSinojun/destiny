
#pragma once

#include "point.h"
#include <string>
#include <vector>

#define OMAPX   180
#define OMAPY   180

#define OVERMAP_DEPTH   10
#define OVERMAP_HEIGHT  0
#define OVERMAP_LAYERS  (1 + OVERMAP_DEPTH + OVERMAP_HEIGHT)

#define OVERMAP_CITY_SIZE()         dice(5, 6)
#define OVERMAP_CITIES_PER_BLOCK()  dice(3, 4)
#define OVERMAP_ROAD_LENGTH()       rng(1, 3)
#define OVERMAP_STREET_CHANCE       10


class city {
public:
    int x;
    int y;
    int s;
    std::string name;
    city(int X = -1, int Y = -1, int S = -1) : x(X), y(Y), s(S) {
    }
};

class game;
class overmap
{
public:
    typedef enum {
        ot_null = 0,
        ot_field,
        ot_river_center,
        ot_rock,

        ot_road_null,
        ot_road_ns,
        ot_road_ew,
        ot_road_ne,
        ot_road_es,
        ot_road_sw,
        ot_road_wn,
        ot_road_nes,
        ot_road_new,
        ot_road_nsw,
        ot_road_esw,
        ot_road_nesw,
        ot_road_nesw_manhole,

        ot_shop,
        ot_house,
        ot_park,
    } oter_id;

    class oter {
    public:
        oter_id type;
        oter() : type(ot_null) {}
        oter(oter_id id) : type(id) {}
        oter& operator=(oter_id t) { type = t; return *this; }
        bool operator==(oter_id t) { return (type == t); }
        bool is_river() { return type == ot_river_center; }
        bool is_road() { return type >= ot_road_null && type <= ot_road_nesw_manhole; }
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
    std::vector<city> cities;

    void init_layers();
    void load();
    void generate(overmap* north, overmap* west, overmap* south, overmap* east);
    void place_river(point pa, point pb);
    void place_cities();
    void make_road(int cx, int cy, int cs, int dir, city town);
    void put_buildings(int x, int y, int dir, city town);
    double dist(int x1, int y1, int x2, int y2);

    oter& ter(int x, int y, int z = 0);

};

