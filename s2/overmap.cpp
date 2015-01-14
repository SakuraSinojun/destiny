
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
{
    init_layers();
    load();
}

overmap::~overmap()
{
}

void overmap::init_layers()
{
    for(int z = 0; z < OVERMAP_LAYERS; ++z) {
        oter_id default_type = (z < OVERMAP_DEPTH) ? ot_rock : (z == OVERMAP_DEPTH) ? ot_field : ot_null;
        for(int i = 0; i < OMAPX; ++i) {
            for(int j = 0; j < OMAPY; ++j) {
                layer[z].terrain[i][j] = default_type;
                layer[z].visible[i][j] = false;
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

    place_cities();
}

overmap::oter& overmap::ter(int x, int y, int z)
{
    if (x < 0 || x >= OMAPX || y < 0 || y >= OMAPY || z < -OVERMAP_DEPTH || z > OVERMAP_HEIGHT) {
        nullret = ot_null;
        return nullret;
    }
    return layer[z + OVERMAP_DEPTH].terrain[x][y];
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
                ter(x + i * xchange, y + i * ychange, 0) = ot_shop; // shop(((dir % 2) - i) % 4);
            else {
                if (rng(0, 99) > 130 * dist(x, y, town.x, town.y) / town.s)
                    ter(x + i * xchange, y + i * ychange, 0) = ot_park;
                else
                    ter(x + i * xchange, y + i * ychange, 0) = ot_house; // house(((dir%2)-i)%4);
            }
        }
    }
}

