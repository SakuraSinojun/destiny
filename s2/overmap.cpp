
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include "game.h"
#include "overmap.h"
#include "rng.h"
#include "tools/logging.h"

overmap::overmap(game& g, int x, int y)
    : m_game(g)
    , loc(x, y)
    , layer(NULL)
{
    init_layers();
    load();
}

overmap::overmap(const overmap& o)
    : m_game(o.m_game)
    , loc(o.loc.x, o.loc.y)
    , layer(NULL)
{
    layer = new map_layer[OVERMAP_LAYERS];
    for(int z = 0; z < OVERMAP_LAYERS; ++z) {
        for(int i = 0; i < OMAPX; ++i) {
            for(int j = 0; j < OMAPY; ++j) {
                layer[z].terrain[i][j] = o.layer[z].terrain[i][j];
                // layer[z].visible[i][j] = o.layer[z].visible[i][j];
            }
        }
        // layer[z].notes = o.layer[z].notes;
    }
}

overmap::~overmap()
{
    if (layer != NULL)
        delete[] layer;
}

void overmap::init_layers()
{
    layer = new map_layer[OVERMAP_LAYERS];
    for(int z = 0; z < OVERMAP_LAYERS; ++z) {
        oter_id default_type = (z < OVERMAP_DEPTH) ? ot_rock : (z == OVERMAP_DEPTH) ? ot_field : ot_null;
        for(int i = 0; i < OMAPX; ++i) {
            for(int j = 0; j < OMAPY; ++j) {
                layer[z].terrain[i][j] = default_type;
                // layer[z].visible[i][j] = false;
            }
        }
    }
}

void overmap::load()
{
    std::vector<overmap*>   pointers;

    for (int i = -1; i <= 1; i += 2) {
        overmap* m = m_game.m().get_overmap(loc.x, loc.y + i, false);
        pointers.push_back(m);  // 0 => north, 2 => south
        m = m_game.m().get_overmap(loc.x + i, loc.y, false);
        pointers.push_back(m);  // 1 => west, 3 => east
    }
    generate(pointers[0], pointers[1], pointers[2], pointers[3]);
}

void overmap::generate(overmap* north, overmap* west, overmap* south, overmap* east)
{
    generate_rivers(north, west, south, east);
    place_cities();
    generate_roads(north, west, south, east);
    place_forest();
    polish(0);
}

void overmap::generate_rivers(overmap* north, overmap* west, overmap* south, overmap* east)
{
    std::vector<point> river_start; // West/North endpoints of rivers
    std::vector<point> river_end;   // East/South endpoints of rivers

    // {{{
    if (north != NULL) {
        for (int i = 2; i < OMAPX - 2; i++) {
            if (north->ter(i, OMAPY - 1, 0).is_river())
                ter(i, 0, 0) = ot_river_center;
            if (north->ter(i,     OMAPY - 1, 0) == ot_river_center &&
                north->ter(i - 1, OMAPY - 1, 0) == ot_river_center &&
                north->ter(i + 1, OMAPY - 1, 0) == ot_river_center) {
                if (river_start.size() == 0 || river_start[river_start.size() - 1].x < i - 6)
                    river_start.push_back(point(i, 0));
            }
        }
    }

    size_t rivers_from_north = river_start.size();
    if (west != NULL) {
        for (int i = 2; i < OMAPY - 2; i++) {
            if (west->ter(OMAPX - 1, i, 0).is_river())
                ter(0, i, 0) = ot_river_center;
            if (west->ter(OMAPX - 1, i, 0)     == ot_river_center &&
                west->ter(OMAPX - 1, i - 1, 0) == ot_river_center &&
                west->ter(OMAPX - 1, i + 1, 0) == ot_river_center) {
                if (river_start.size() == rivers_from_north || river_start[river_start.size() - 1].y < i - 6)
                    river_start.push_back(point(0, i));
            }
        }
    }

    if (south != NULL) {
        for (int i = 2; i < OMAPX - 2; i++) {
            if (south->ter(i, 0, 0).is_river())
                ter(i, OMAPY - 1, 0) = ot_river_center;
            if (south->ter(i,     0, 0) == ot_river_center &&
                south->ter(i - 1, 0, 0) == ot_river_center &&
                south->ter(i + 1, 0, 0) == ot_river_center) {
                if (river_end.size() == 0 || river_end[river_end.size() - 1].x < i - 6)
                    river_end.push_back(point(i, OMAPY - 1));
            }
        }
    }

    size_t rivers_to_south = river_end.size();
    if (east != NULL) {
        for (int i = 2; i < OMAPY - 2; i++) {
            if (east->ter(0, i, 0).is_river())
                ter(OMAPX - 1, i, 0) = ot_river_center;
            if (east->ter(0, i, 0)     == ot_river_center &&
                east->ter(0, i - 1, 0) == ot_river_center &&
                east->ter(0, i + 1, 0) == ot_river_center) {
                if (river_end.size() == rivers_to_south || river_end[river_end.size() - 1].y < i - 6)
                    river_end.push_back(point(OMAPX - 1, i));
            }
        }
    }
    // }}}

    std::vector<point> new_rivers;
    if (north == NULL || west == NULL) {
        while (river_start.empty() || river_start.size() + 1 < river_end.size()) {
            new_rivers.clear();
            if (north == NULL)
                new_rivers.push_back(point(rng(10, OMAPX - 11), 0));
            if (west == NULL)
                new_rivers.push_back(point(0, rng(10, OMAPY - 11)));
            river_start.push_back(new_rivers[rng(0, new_rivers.size() - 1)]);
        }
    }
    if (south == NULL || east == NULL) {
        while (river_end.empty() || river_end.size() + 1 < river_start.size()) {
            new_rivers.clear();
            if (south == NULL)
                new_rivers.push_back(point(rng(10, OMAPX - 11), OMAPY - 1));
            if (east == NULL)
                new_rivers.push_back(point(OMAPX - 1, rng(10, OMAPY - 11)));
            river_end.push_back(new_rivers[rng(0, new_rivers.size() - 1)]);
        }
    }

    // Now actually place those rivers.
    if (river_start.size() > river_end.size() && river_end.size() > 0) {
        std::vector<point> river_end_copy = river_end;
        int index;
        while (!river_start.empty()) {
            index = rng(0, river_start.size() - 1);
            if (!river_end.empty()) {
                place_river(river_start[index], river_end[0]);
                river_end.erase(river_end.begin());
            } else {
                place_river(river_start[index], river_end_copy[rng(0, river_end_copy.size() - 1)]);
            }
            river_start.erase(river_start.begin() + index);
        }
    } else if (river_end.size() > river_start.size() && river_start.size() > 0) {
        std::vector<point> river_start_copy = river_start;
        int index;
        while (!river_end.empty()) {
            index = rng(0, river_end.size() - 1);
            if (!river_start.empty()) {
                place_river(river_start[0], river_end[index]);
                river_start.erase(river_start.begin());
            } else {
                place_river(river_start_copy[rng(0, river_start_copy.size() - 1)], river_end[index]);
            }
            river_end.erase(river_end.begin() + index);
        }
    } else if (river_end.size() > 0) {
        if (river_start.size() != river_end.size())
            river_start.push_back(point(rng(OMAPX * .25, OMAPX * .75), rng(OMAPY * .25, OMAPY * .75)));
        for (size_t i = 0; i < river_start.size(); i++)
            place_river(river_start[i], river_end[i]);
    }
}

void overmap::generate_roads(overmap* north, overmap* west, overmap* south, overmap* east)
{
    std::vector<city>   road_points;

    if (north != NULL) {
        for (size_t i = 0; i < north->roads_out.size(); i++) {
            if (north->roads_out[i].y == OMAPY - 1)
                roads_out.push_back(city(north->roads_out[i].x, 0, 0));
        }
    }
    if (west != NULL) {
        for (size_t i = 0; i < west->roads_out.size(); i++) {
            if (west->roads_out[i].x == OMAPX - 1)
                roads_out.push_back(city(0, west->roads_out[i].y, 0));
        }
    }

    if (south != NULL) {
        for (int i = 2; i < OMAPX - 2; i++) {
            if (south->ter(i, 0, 0) == ot_road_nesw)
                roads_out.push_back(city(i, OMAPY - 1, 0));
        }
        for (size_t i = 0; i < south->roads_out.size(); i++) {
            if (south->roads_out[i].y == 0)
                roads_out.push_back(city(south->roads_out[i].x, OMAPY - 1, 0));
        }
    }

    if (east != NULL) {
        for (int i = 2; i < OMAPY - 2; i++) {
            if (east->ter(0, i, 0) == ot_road_nesw)
                roads_out.push_back(city(OMAPX - 1, i, 0));
        }
        for (size_t i = 0; i < east->roads_out.size(); i++) {
            if (east->roads_out[i].x == 0)
                roads_out.push_back(city(OMAPX - 1, east->roads_out[i].y, 0));
        }
    }

    if (roads_out.size() < 2) {
        std::vector<city> viable_roads;
        int tmp;
        if (north == NULL) {
            do {
                tmp = rng(10, OMAPX - 11);
            } while (ter(tmp, 0, 0).is_river() || ter(tmp - 1, 0, 0).is_river() || ter(tmp + 1, 0, 0).is_river());
            viable_roads.push_back(city(tmp, 0, 0));
        }
        if (east == NULL) {
            do {
                tmp = rng(10, OMAPY - 11);
            } while (ter(OMAPX - 1, tmp, 0).is_river() || ter(OMAPX - 1, tmp - 1, 0).is_river() || ter(OMAPX - 1, tmp + 1, 0).is_river());
            viable_roads.push_back(city(OMAPX - 1, tmp, 0));
        }
        if (south == NULL) {
            do {
                tmp = rng(10, OMAPX - 11);
            } while (ter(tmp, OMAPY - 1, 0).is_river() || ter(tmp - 1, OMAPY - 1, 0).is_river() || ter(tmp + 1, OMAPY - 1, 0).is_river());
            viable_roads.push_back(city(tmp, OMAPY - 1, 0));
        }
        if (west == NULL) {
            do {
                tmp = rng(10, OMAPY - 11);
            } while (ter(0, tmp, 0).is_river() || ter(0, tmp - 1, 0).is_river() || ter(0, tmp + 1, 0).is_river());
            viable_roads.push_back(city(0, tmp, 0));
        }
        while (roads_out.size() < 2 && !viable_roads.empty()) {
            tmp = rng(0, viable_roads.size() - 1);
            roads_out.push_back(viable_roads[tmp]);
            viable_roads.erase(viable_roads.begin() + tmp);
        }
    }

    for (size_t i = 0; i < roads_out.size(); i++)
        road_points.push_back(roads_out[i]);
    for (size_t i = 0; i < cities.size(); i++)
        road_points.push_back(cities[i]);

    // And finally connect them via "highways"
    place_hiways(road_points, 0, ot_road_null);

    // Make the roads out road points;
    for (size_t i = 0; i < roads_out.size(); i++)
        ter(roads_out[i].x, roads_out[i].y, 0) = ot_road_nesw;
}

overmap::oter& overmap::ter(int x, int y, int z)
{
    if (x < 0 || x >= OMAPX || y < 0 || y >= OMAPY || z < -OVERMAP_DEPTH || z > OVERMAP_HEIGHT) {
        nullret = ot_null;
        return nullret;
    }
    return layer[z + OVERMAP_DEPTH].terrain[x][y];
}

overmap::oter& overmap::ter(point& pt, int z)
{
    return ter(pt.x, pt.y, z);
}

void overmap::place_river(point pa, point pb)
{
    int x = pa.x, y = pa.y;
    do {
        x += rng(-1, 1);
        y += rng(-1, 1);
        if (x < 0) x = 0;
        if (x > OMAPX - 1) x = OMAPX - 2;
        if (y < 0) y = 0;
        if (y > OMAPY - 1) y = OMAPY - 1;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (y+i >= 0 && y+i < OMAPY && x+j >= 0 && x+j < OMAPX)
                    ter(x+j, y+i, 0) = ot_river_center;
            }
        }
        if (pb.x > x && (rng(0, int(OMAPX * 1.2) - 1) < pb.x - x ||
                (rng(0, int(OMAPX * .2) - 1) > pb.x - x &&
                 rng(0, int(OMAPY * .2) - 1) > abs(pb.y - y))))
            x++;
        if (pb.x < x && (rng(0, int(OMAPX * 1.2) - 1) < x - pb.x ||
                (rng(0, int(OMAPX * .2) - 1) > x - pb.x &&
                 rng(0, int(OMAPY * .2) - 1) > abs(pb.y - y))))
            x--;
        if (pb.y > y && (rng(0, int(OMAPY * 1.2) - 1) < pb.y - y ||
                (rng(0, int(OMAPY * .2) - 1) > pb.y - y &&
                 rng(0, int(OMAPX * .2) - 1) > abs(x - pb.x))))
            y++;
        if (pb.y < y && (rng(0, int(OMAPY * 1.2) - 1) < y - pb.y ||
                (rng(0, int(OMAPY * .2) - 1) > y - pb.y &&
                 rng(0, int(OMAPX * .2) - 1) > abs(x - pb.x))))
            y--;
#if 1
        x += rng(-1, 1);
        y += rng(-1, 1);
        if (x < 0) x = 0;
        if (x > OMAPX - 1) x = OMAPX - 2;
        if (y < 0) y = 0;
        if (y > OMAPY - 1) y = OMAPY - 1;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if ((y+i >= 1 && y+i < OMAPY - 1 && x+j >= 1 && x+j < OMAPX - 1))
                    ter(x+j, y+i, 0) = ot_river_center;
            }
        }
#endif
    } while (pb.x != x || pb.y != y);
}

void overmap::place_cities()
{
    // 3d4 cities per overmap block.
    size_t  num = OVERMAP_CITIES_PER_BLOCK(); // dice(3, 4);

    int cx, cy, cs;
    int start_dir;
    while (cities.size() < num) {
        cx = rng(12, OMAPX - 12);
        cy = rng(12, OMAPY - 12);
        cs = OVERMAP_CITY_SIZE(); // dice(3, 4);
        if (ter(cx, cy) == ot_field) {
            ter(cx, cy) = ot_road_nesw;
            city tmp(cx, cy, cs);
            cities.push_back(tmp);
            start_dir = rng(0, 3);
            for (int j = 0; j < 4; j++)
                make_road(cx, cy, cs, (start_dir + j) % 4, tmp);
        }
    }
}

void overmap::place_forest()
{
    int x, y;
    int forx;
    int fory;
    int fors;
    for (int i = 0; i < OVERMAP_NUM_FOREST; i++) {
        bool regen = true;
        while (regen) {
            forx = rng(0, OMAPX - 1);
            fory = rng(0, OMAPY - 1);
            fors = rng(15, 40);
            regen = false;
            for (size_t j = 0; j < cities.size(); j++) {
                if (dist(forx, fory, cities[j].x, cities[j].y) - fors / 2 < cities[j].s) {
                    regen = true;
                    break;
                }
            }
        } // while regen

        int swamps = OVERMAP_SWAMPINESS;    // How big the swamp may be...
        x = forx;
        y = fory;
        // Depending on the size on the forest...
        for (int j = 0; j < fors; j++) {
            int swamp_chance = 0;
            for (int k = -2; k <= 2; k++) {
                for (int l = -2; l <= 2; l++) {
                    if (ter(x + k, y + l, 0) == ot_forest_water || ter(x + k, y + l, 0).is_river())
                        swamp_chance += 5;
                }
            }
            bool swampy = false;
            if (swamps > 0 && swamp_chance > 0 && !one_in(swamp_chance) && (ter(x, y, 0) == ot_forest || ter(x, y, 0) == ot_forest_thick || ter(x, y, 0) == ot_field  || one_in(OVERMAP_SWAMPCHANCE))) {
                ter(x, y, 0) = ot_forest_water;
                swampy = true;
                swamps--;
            } else if (swamp_chance == 0) {
                swamps = OVERMAP_SWAMPINESS;
            }

            if (ter(x, y, 0) == ot_field)
                ter(x, y, 0) = ot_forest;
            else if (ter(x, y, 0) == ot_forest)
                ter(x, y, 0) = ot_forest_thick;

            if (swampy && (ter(x, y - 1, 0) == ot_field || ter(x, y - 1, 0) == ot_forest))
                ter(x, y - 1, 0) = ot_forest_water;
            else if (ter(x, y - 1, 0) == ot_forest)
                ter(x, y - 1, 0) = ot_forest_thick;
            else if (ter(x, y - 1, 0) == ot_field)
                ter(x, y - 1, 0) = ot_forest;

            if (swampy && (ter(x, y + 1, 0) == ot_field || ter(x, y + 1, 0) == ot_forest))
                ter(x, y + 1, 0) = ot_forest_water;
            else if (ter(x, y + 1, 0) == ot_forest)
                ter(x, y + 1, 0) = ot_forest_thick;
            else if (ter(x, y + 1, 0) == ot_field)
                ter(x, y + 1, 0) = ot_forest;

            if (swampy && (ter(x - 1, y, 0) == ot_field || ter(x - 1, y, 0) == ot_forest))
                ter(x - 1, y, 0) = ot_forest_water;
            else if (ter(x - 1, y, 0) == ot_forest)
                ter(x - 1, y, 0) = ot_forest_thick;
            else if (ter(x - 1, y, 0) == ot_field)
                ter(x - 1, y, 0) = ot_forest;

            if (swampy && (ter(x + 1, y, 0) == ot_field || ter(x + 1, y, 0) == ot_forest))
                ter(x + 1, y, 0) = ot_forest_water;
            else if (ter(x + 1, y, 0) == ot_forest)
                ter(x + 1, y, 0) = ot_forest_thick;
            else if (ter(x + 1, y, 0) == ot_field)
                ter(x + 1, y, 0) = ot_forest;

            // Random walk our forest
            x += rng(-2, 2);
            if (x < 0    ) x = 0;
            if (x > OMAPX) x = OMAPX;
            y += rng(-2, 2);
            if (y < 0    ) y = 0;
            if (y > OMAPY) y = OMAPY;
        }
    }
}

void overmap::place_hiways(std::vector<city> cities, int z, overmap::oter_id base)
{
    if (cities.size() == 1)
        return;
    city best;
    int closest = -1;
    int distance;
    bool maderoad = false;
    for (size_t i = 0; i < cities.size(); i++) {
        maderoad = false;
        closest = -1;
        for (size_t j = i + 1; j < cities.size(); j++) {
            distance = dist(cities[i].x, cities[i].y, cities[j].x, cities[j].y);
            if (distance < closest || closest < 0) {
                closest = distance;
                best = cities[j];
            }
            if (distance < OVERMAP_TOP_HIWAY_DIST) {
                maderoad = true;
                make_hiway(cities[i].x, cities[i].y, cities[j].x, cities[j].y, z, base);
            }
        }
        if (!maderoad && closest > 0)
            make_hiway(cities[i].x, cities[i].y, best.x, best.y, z, base);
    }
}

void overmap::make_road(int cx, int cy, int cs, int dir, city town)
{
    int x = cx, y = cy;
    int c = cs, croad = cs;
    switch (dir) {
    case 0:
        while (c > 0 && y > 0 && (ter(x, y - 1, 0) == ot_field || c == cs)) {
            y--;
            c--;
            ter(x, y, 0) = ot_road_ns;
            for (int i = -1; i <= 0; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (abs(j) != abs(i) && (ter(x + j, y + i, 0) == ot_road_ew || ter(x + j, y + i, 0) == ot_road_ns)) {
                        ter(x, y, 0) = ot_road_null;
                        c = -1;
                    }
                }
            }
            put_buildings(x, y, dir, town);
            if (c < croad - 1 && c >= 2 && ter(x - 1, y, 0) == ot_field && ter(x + 1, y, 0) == ot_field) {
                croad = c;
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 1, town);
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 3, town);
            }
        }
        if (ter(x, y - 2, 0).is_road())
            ter(x, y - 1, 0) = ot_road_ns;
        break;
    case 1:
        while (c > 0 && x < OMAPX - 1 && (ter(x + 1, y, 0) == ot_field || c == cs)) {
            x++;
            c--;
            ter(x, y, 0) = ot_road_ew;
            for (int i = -1; i <= 1; i++) {
                for (int j = 0; j <= 1; j++) {
                    if (abs(j) != abs(i) && (ter(x + j, y + i, 0) == ot_road_ew || ter(x + j, y + i, 0) == ot_road_ns)) {
                        ter(x, y, 0) = ot_road_null;
                        c = -1;
                    }
                }
            }
            put_buildings(x, y, dir, town);
            if (c < croad - 2 && c >= 3 && ter(x, y - 1, 0) == ot_field && ter(x, y + 1, 0) == ot_field) {
                croad = c;
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 0, town);
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 2, town);
            }
        }
        if (ter(x - 2, y, 0).is_road())
            ter(x - 1, y, 0) = ot_road_ew;
        break;
    case 2:
        while (c > 0 && y < OMAPY-1 && (ter(x, y+1, 0) == ot_field || c == cs)) {
            y++;
            c--;
            ter(x, y, 0) = ot_road_ns;
            for (int i = 0; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (abs(j) != abs(i) && (ter(x+j, y+i, 0) == ot_road_ew ||
                            ter(x+j, y+i, 0) == ot_road_ns)) {
                        ter(x, y, 0) = ot_road_null;
                        c = -1;
                    }
                }
            }
            put_buildings(x, y, dir, town);
            if (c < croad - 2 && ter(x - 1, y, 0) == ot_field && ter(x + 1, y, 0) == ot_field) {
                croad = c;
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 1, town);
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 3, town);
            }
        }
        if (ter(x, y + 2, 0).is_road())
            ter(x, y + 1, 0) = ot_road_ns;
        break;
    case 3:
        while (c > 0 && x > 0 && (ter(x-1, y, 0) == ot_field || c == cs)) {
            x--;
            c--;
            ter(x, y, 0) = ot_road_ew;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 0; j++) {
                    if (abs(j) != abs(i) && (ter(x + j, y + i, 0) == ot_road_ew || ter(x + j, y + i, 0) == ot_road_ns)) {
                        ter(x, y, 0) = ot_road_null;
                        c = -1;
                    }
                }
            }
            put_buildings(x, y, dir, town);
            if (c < croad - 2 && c >= 3 && ter(x, y - 1, 0) == ot_field && ter(x, y + 1, 0) == ot_field) {
                croad = c;
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 0, town);
                make_road(x, y, cs - OVERMAP_ROAD_LENGTH(), 2, town);
            }
        }
        if (ter(x + 2, y, 0).is_road())
            ter(x + 1, y, 0) = ot_road_ew;
        break;
    }
    // return;
    cs -= OVERMAP_ROAD_LENGTH();
    if (cs >= 2 && c == 0) {
        int dir2;
        if (dir % 2 == 0)
            dir2 = rng(0, 1) * 2 + 1;
        else
            dir2 = rng(0, 1) * 2;
        make_road(x, y, cs, dir2, town);
        if (one_in(5))
            make_road(x, y, cs, (dir2 + 2) % 4, town);
    }
}

void overmap::make_hiway(int x1, int y1, int x2, int y2, int z, oter_id base)
{
    if (x1 == x2 && y1 == y2)
        return;

    int x = x1, y = y1;

    point   nexth;
    point   nextv;

    bool hor = true;
    while (true) {
        if (x == x2 && y == y2) break;
        if ((dist(x, y, x1, y1) > dist(x, y, x2, y2))) {
            if (ter(x, y - 1).is_road()) break;
            if (ter(x, y + 1).is_road()) break;
            if (ter(x - 1, y).is_road()) break;
            if (ter(x + 1, y).is_road()) break;
        }

        if (x2 > x)
            nexth.set(x + 1, y);
        else if (x2 < x)
            nexth.set(x - 1, y);
        else
            nexth.set(x, y);

        if (y2 > y)
            nextv.set(x, y + 1);
        else if (y2 < y)
            nextv.set(x, y - 1);
        else
            nextv.set(x, y);

        point& current = hor ? nexth : nextv;
        point& other = hor ? nextv : nexth;

        // ILOG() << "hor = " << hor;
        // ILOG() << "xy = " << point(x, y);
        // ILOG() << "nexth = " << nexth;
        // ILOG() << "nextv = " << nextv;

        // 走不通时换方向。。 或者概率换方向。
        if (current == point(x, y)) {
            hor = !hor;
        } else {
            if (!ter(other, z).is_river() && !ter(other, z).is_building()) {
                if (one_in(12) || ter(current, z).is_building())
                    hor = !hor;
            }
        }

        // 决定的目标方向。
        point& next = hor ? nexth : nextv;

        // ILOG() << "next = " << next;

        x = next.x;
        y = next.y;

        overmap::oter& ot = ter(next, z);

        // ot = base;
        // continue;

        if (ot.is_river()) {
            ot = hor ? ot_bridge_ew : ot_bridge_ns;
#if 0
            if (hor) {
                int xdir = (x2 > x) ? 1 : -1; 
                bool bridge = true;
                int tmp = x;
                while (tmp >= 0 && tmp < OMAPX && ter(tmp, y, z).is_river()) {
                    if (ter(tmp, y, z).is_road()) {
                        bridge = false;
                    }
                    tmp += xdir;
                }
                if (bridge) {
                    while (x >= 0 && x < OMAPX && ter(x, y, z).is_river()) {
                        ter(x, y, z) = ot_bridge_ew;
                        x += xdir;
                    }
                    ter(x, y, z) = base;
                }
            } else {
                int ydir = (y2 > y) ? 1 : -1; 
                bool bridge = true;
                int tmp = y;
                while (tmp >= 0 && tmp < OMAPY && ter(x, tmp, z).is_river()) {
                    if (ter(x, tmp, z).is_road()) {
                        bridge = false;
                    }
                }
                if (bridge) {
                    while (x >= 0 && y < OMAPY && ter(x, y, z).is_river()) {
                        ter(x, y, z) = ot_bridge_ns;
                        y += ydir;
                    }
                    ter(x, y, z) = base;
                }
            }
#endif
        } else if (!ot.is_road()) {
            ot = base;
        }

        if (one_in(20)) {
            put_buildings(x, y, hor ? 0 : 1, city(x, y, 5));
            put_buildings(x, y, hor ? 2 : 3, city(x, y, 5));
        }

    } // while
}


double overmap::dist(int x1, int y1, int x2, int y2)
{
    return sqrt(double((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}

void overmap::put_buildings(int x, int y, int dir, city town)
{
    int ychange = dir % 2, xchange = (dir + 1) % 2;
    for (int i = -1; i <= 1; i += 2) {
        if ((ter(x + i * xchange, y + i * ychange, 0) == ot_field) && !one_in(OVERMAP_STREET_CHANCE)) {
            if (rng(0, 99) > 80 * dist(x, y, town.x, town.y) / town.s)
                ter(x + i * xchange, y + i * ychange, 0) = shop(((dir % 2) - i) % 4);
            else {
                if (rng(0, 99) > 130 * dist(x, y, town.x, town.y) / town.s)
                    ter(x + i * xchange, y + i * ychange, 0) = ot_park;
                else
                    ter(x + i * xchange, y + i * ychange, 0) = house(((dir % 2) - i) % 4);
            }
        }
    }
}

overmap::oter_id overmap::house(int dir)
{
    while (dir < 0)
        dir += 4;
    switch (dir) {
    case 0:  return ot_house_north;
    case 1:  return ot_house_east;
    case 2:  return ot_house_south;
    case 3:  return ot_house_west;
    default: return ot_null;
    }
}

overmap::oter_id overmap::shop(int dir)
{
    return ot_shop;
}

void overmap::polish(int z, oter_id min, oter_id max)
{
    for (int x = 0; x < OMAPX; x++) {
        for (int y = 0; y < OMAPY; y++) {
            oter& ot = ter(x, y, z);
            if (ot < min || ot > max)
                continue;
            if (ot >= ot_road_null && ot <= ot_road_nesw) {
                good_road(x, y, z);
            } else if (ot.is_bridge() && ter(x - 1, y, z).is_bridge() && ter(x + 1, y, z).is_bridge() && ter(x, y - 1, z).is_bridge() && ter(x, y + 1, z).is_bridge()) {
                ot = ot_road_nesw;
            } else if (ot == ot_bridge_ns && (!ter(x - 1, y, z).is_river() || !ter(x + 1, y, z).is_river())) {
                ot = ot_road_ns;
            } else if (ot == ot_bridge_ew && (!ter(x, y - 1, z).is_river() || !ter(x, y + 1, z).is_river())) {
                ot = ot_road_ew;
            } else if (ot > ot_river_start && ot < ot_river_end) {
                good_river(x, y, z);
            } else {
            }
        }
    }
}

void overmap::good_road(int x, int y, int z)
{
    oter& ot = ter(x, y, z);

    bool n = ter(x, y - 1, z).is_road();
    bool s = ter(x, y + 1, z).is_road();
    bool w = ter(x - 1, y, z).is_road();
    bool e = ter(x + 1, y, z).is_road();

    if (n) {
        if (e) {
            if (s) {
                ot = w ? ot_road_nesw : ot_road_nes;
            } else {
                ot = w ? ot_road_new : ot_road_ne;
            }
        } else {
            if (w)
                ot = s ? ot_road_nsw : ot_road_wn;
            else
                ot = ot_road_ns;
        }
    } else {
        if (e) {
            if (s) {
                ot = w ? ot_road_esw : ot_road_es;
            } else {
                ot = w ? ot_road_ew : ot_road_es;
            }
        } else {
            if (s) {
                ot = w ? ot_road_sw : ot_road_ns;
            } else {
                ot = w ? ot_road_ew : ot_road_nesw;
            }
        }
    }
    // if (ter(x, y, z) == ot_road_nesw && one_in(4))
    //     ter(x, y, z) = ot_road_nesw_manhole;
}

void overmap::good_river(int x, int y, int z)
{
    oter& ot = ter(x, y, z);

    bool w = ter(x - 1, y, z).is_river();
    bool e = ter(x + 1, y, z).is_river();
    bool n = ter(x, y - 1, z).is_river();
    bool s = ter(x, y + 1, z).is_river();

    bool nw = ter(x - 1, y - 1, z).is_river();
    bool ne = ter(x + 1, y - 1, z).is_river();
    bool sw = ter(x - 1, y + 1, z).is_river();
    bool se = ter(x + 1, y + 1, z).is_river();

    if (w) {
        if (n) {
            if (s) {
                if (e) {
                    if (!nw)
                        ot = ot_river_c_not_nw;
                    else if (!ne)
                        ot = ot_river_c_not_ne;
                    else if (!sw)
                        ot = ot_river_c_not_sw;
                    else if (!se)
                        ot = ot_river_c_not_se;
                    else
                        ot = ot_river_center;
                } else {
                    ot = ot_river_east;
                }
            } else {
                ot = e ? ot_river_south : ot_river_se;
            }
        } else {
            if (s) {
                ot = e ? ot_river_north : ot_river_ne;
            } else {
                if (e) {
                    ot = ot_forest_water;
                }
            }
        }
    } else {
        if (n) {
            if (s) {
                ot = e ? ot_river_west : ot_forest_water;
            } else {
                ot = e ? ot_river_sw : ot_forest_water;
            }
        } else {
            if (s) {
                ot = e ? ot_river_nw : ot_forest_water;
            } else {
                ot = ot_forest_water;
            }
        }
    }

}







