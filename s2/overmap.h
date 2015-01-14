
#pragma once

#include "point.h"
#include <string>
#include <vector>

#define OMAPX   180
#define OMAPY   180

#define OVERMAP_DEPTH   10
#define OVERMAP_HEIGHT  0
#define OVERMAP_LAYERS  (1 + OVERMAP_DEPTH + OVERMAP_HEIGHT)

#define OVERMAP_CITY_SIZE()         dice(3, 6)
#define OVERMAP_CITIES_PER_BLOCK()  dice(3, 4)
#define OVERMAP_ROAD_LENGTH()       rng(1, 3)
#define OVERMAP_STREET_CHANCE       5
#define OVERMAP_SWAMPINESS          4
#define OVERMAP_NUM_FOREST          250
#define OVERMAP_SWAMPCHANCE         8500 // Chance that a swamp will spawn instead of forest
#define OVERMAP_TOP_HIWAY_DIST      999

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
        ot_dirtlot, 
        ot_rock,

        /////////////////////////////////////////////
        ot_river_start,
        ot_river_center,
        ot_river_c_not_ne, ot_river_c_not_nw, ot_river_c_not_se, ot_river_c_not_sw,
        ot_river_north, ot_river_east, ot_river_south, ot_river_west,
        ot_river_ne, ot_river_se, ot_river_sw, ot_river_nw,
        ot_river_end,


        ot_forest, ot_forest_thick, ot_forest_water,

        /////////////////////////////////////////////
        ot_road_start,
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

        ot_bridge_ns, ot_bridge_ew,

        ot_road_end,

        /////////////////////////////////////////////
        ot_building_start,

        ot_house_north,
        ot_house_east,
        ot_house_south,
        ot_house_west,

        ot_shop,
        ot_house,
        ot_park,
        ot_building_end,

        ot_max,
    } oter_id;

    class oter {
    public:
        oter_id type;
        int     level;
        int     n;
        int     s;
        int     w;
        int     e;

        oter() : type(ot_null), level(0) {}
        oter(oter_id id) : type(id), level(0) {}
        oter& operator=(oter_id t) { type = t; return *this; }

        bool operator==(oter_id t) { return (type == t); }
        bool operator<(oter_id t) { return (type < t); }
        bool operator>=(oter_id t) { return !operator<(t); }
        bool operator<=(oter_id t) { return operator<(t) || operator==(t); }
        bool operator>(oter_id t) { return !operator<=(t); }

        bool is_river() { return (type > ot_river_start && type <= ot_river_end) || type == ot_bridge_ns || type == ot_bridge_ew; }
        bool is_water() { return (type > ot_river_start && type <= ot_river_end); }
        bool is_road() { return type > ot_road_start && type <= ot_road_end; }
        bool is_bridge() { return type == ot_bridge_ns || type == ot_bridge_ew; }
        bool is_building() { return type > ot_building_start && type <= ot_building_end; }
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
    std::vector<city> roads_out;

    void init_layers();
    void load();
    void generate(overmap* north, overmap* west, overmap* south, overmap* east);
    void generate_rivers(overmap* north, overmap* west, overmap* south, overmap* east);
    void generate_roads(overmap* north, overmap* west, overmap* south, overmap* east);
    void place_river(point pa, point pb);
    void place_cities();
    void place_forest();
    void place_hiways(std::vector<city> cities, int z, oter_id base);

    void make_road(int cx, int cy, int cs, int dir, city town);
    void make_hiway(int x1, int y1, int x2, int y2, int z, oter_id base);

    void put_buildings(int x, int y, int dir, city town);
    oter_id house(int dir);
    oter_id shop(int dir);

    // void polish(int z, oter_id min = ot_null, oter_id max = ot_max);
    // void good_road(int x, int y, int z);

    double dist(int x1, int y1, int x2, int y2);

    oter& ter(int x, int y, int z = 0);
    oter& ter(point& pt, int z = 0);

};

