
#include <stdio.h>
#include <stdlib.h>
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




