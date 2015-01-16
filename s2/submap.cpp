
#include <stdlib.h>
#include "submap.h"
#include "overmap.h"
#include "rng.h"
#include "map.h"

#include "tools/logging.h"

submap::submap(map* mp, int x, int y)
    : loc(x, y)
{
    generate(mp);
}

submap::~submap()
{
}

submap::ster& submap::ter(int x, int y, int z)
{
    if (x < 0 || x >= SMAPX || y < 0 || y >= SMAPY || z < -OVERMAP_DEPTH || z > OVERMAP_HEIGHT) {
        nullret = st_null;
        return nullret;
    }
    return layer[z + OVERMAP_DEPTH].terrain[x][y];
}

void submap::generate(map* mp)
{
    overmap::oter oter = mp->get_oter(loc.x, loc.y); // om->ter(otx, oty);
    if (gen_forest(oter, mp)) return;
    if (gen_field(oter, mp)) return;
    if (gen_river(oter, mp)) return;
    if (gen_road(oter, mp)) return;
    if (gen_house(oter, mp)) return;

    // rock
    // house
}

void submap::rotate(int turns, int z)
{/*{{{*/
    while (turns < 0) {
        turns += 4;
    }
    turns = (turns % 4);

    map_layer   rotated;
    switch (turns) {
    case 1:
        for (int x = 0; x < SMAPX; x++) {
            for (int y = 0; y < SMAPY; y++) {
                rotated.terrain[x][y] = ter(y, SMAPX - 1 - x, z);
            }
        }
        break;
    case 2:
        for (int x = 0; x < SMAPX; x++) {
            for (int y = 0; y < SMAPY; y++) {
                rotated.terrain[x][y] = ter(SMAPX - 1 - x, SMAPY - 1 - y, z);
            }
        }
        break;
    case 3:
        for (int x = 0; x < SMAPX; x++) {
            for (int y = 0; y < SMAPY; y++) {
                rotated.terrain[x][y] = ter(SMAPY - 1 - y, x, z);
            }
        }
        break;
    case 0:
    default:
        return;
    }
    for (int x = 0; x < SMAPX; x++) {
        for (int y = 0; y < SMAPY; y++) {
            ter(x, y, z) = rotated.terrain[x][y];
        }
    }
}/*}}}*/

void submap::gen_full(ster_id id)
{/*{{{*/
    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ter(i, j) = id;
        }
    }
}/*}}}*/

bool submap::gen_field(const overmap::oter& oter, map* mp)
{/*{{{*/
    if (oter != overmap::ot_field && oter != overmap::ot_null)
        return false;

    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ster& st = ter(i, j);
            st = grass_or_dirt();

            if (one_in(120)) {
                if (one_in(30)) {
                    st = st_shrub_blueberry;
                } else if (one_in(30)) {
                    st = st_shrub_strawberry;
                } else {
                    st = st_shrub;
                }
            }
        }
    }
    return true;
}/*}}}*/

bool submap::gen_forest(const overmap::oter& oter, map* mp)
{/*{{{*/
    if (oter != overmap::ot_forest && oter != overmap::ot_forest_thick && oter != overmap::ot_forest_water) {
        return false;
    }

    overmap::oter n = mp->get_oter(loc.x, loc.y - 1);
    overmap::oter s = mp->get_oter(loc.x, loc.y + 1);
    overmap::oter w = mp->get_oter(loc.x - 1, loc.y);
    overmap::oter e = mp->get_oter(loc.x + 1, loc.y);
    overmap::oter t = mp->get_oter(loc.x, loc.y);

    int cht = (t == overmap::ot_forest_thick) ? 8 : ((t == overmap::ot_forest_water) ? 4 : 0);
    int chn = cht + ((n == overmap::ot_forest_thick) ? 18 : ((n == overmap::ot_forest || n == overmap::ot_forest_water) ? 14 : 0));
    int chs = cht + ((s == overmap::ot_forest_thick) ? 18 : ((s == overmap::ot_forest || s == overmap::ot_forest_water) ? 14 : 0));
    int chw = cht + ((w == overmap::ot_forest_thick) ? 18 : ((w == overmap::ot_forest || w == overmap::ot_forest_water) ? 14 : 0));
    int che = cht + ((e == overmap::ot_forest_thick) ? 18 : ((e == overmap::ot_forest || e == overmap::ot_forest_water) ? 14 : 0));
    
    // 以SMAPX SMAPY都是12为基础做的概率。
    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ster& st = ter(i, j);
            int chance = 0;
            int count = 0;
            
            if (chw > i) {
                count++;
                chance += chw - i;
            }
            if (chn > j) {
                count++;
                chance += chn - j;
            }
            if (che > (SMAPX - i - 1)) {
                chance += che - (SMAPX - i - 1);
                count++;
            }

            if (chs > (SMAPY - j - 1)) {
                chance += chs - (SMAPY - j - 1);
                count++;
            }
            if (count > 0) {
                chance /= count;
            }

            int rn = rng(0, chance);
            if (rn >= 13) {
                st = one_in(200) ? st_tree_apple : st_tree;
            } else if (rn >= 10) {
                st = st_tree_young;
            } else {
                st = grass_or_dirt();
            }
        }
    }
    return true;
}/*}}}*/

bool submap::gen_river(const overmap::oter& oter, map* mp)
{/*{{{*/
    switch (oter.type) {
    case overmap::ot_river_center:
        gen_full(st_water_dp);
        break;
    case overmap::ot_river_c_not_ne:
    case overmap::ot_river_c_not_nw:
    case overmap::ot_river_c_not_se:
    case overmap::ot_river_c_not_sw:
        for (int x = SMAPX - 1; x >= 0; x--) {
            for (int y = 0; y < SMAPY; y++) {
                if (y < 4 && x >= SMAPX - 4)
                    ter(x, y) = st_water_sh;
                else
                    ter(x, y) = st_water_dp;
            }
        }
        if (oter.type == overmap::ot_river_c_not_se)
            rotate(1);
        if (oter.type == overmap::ot_river_c_not_sw)
            rotate(2);
        if (oter.type == overmap::ot_river_c_not_nw)
            rotate(3);
        break;
    case overmap::ot_river_north:
    case overmap::ot_river_east:
    case overmap::ot_river_south:
    case overmap::ot_river_west:
        for (int i = 0; i < SMAPX; i++) {
            for (int j = 0; j < SMAPY; j++) {
                if (j < 4)
                    ter(i, j) = st_water_sh;
                else
                    ter(i, j) = st_water_dp;
            }
        }
        if (oter == overmap::ot_river_east)
            rotate(1);
        if (oter == overmap:: ot_river_south)
            rotate(2);
        if (oter == overmap::ot_river_west)
            rotate(3);
        break;
    case overmap::ot_river_ne:
    case overmap::ot_river_se:
    case overmap::ot_river_sw:
    case overmap::ot_river_nw:
        for (int i = SMAPX - 1; i >= 0; i--) {
            for (int j = 0; j < SMAPY; j++) {
                if (i >= SMAPX - 4 || j < 4)
                    ter(i, j) = st_water_sh;
                else
                    ter(i, j) = st_water_dp;
            }
        }
        if (oter == overmap::ot_river_se)
            rotate(1);
        if (oter == overmap::ot_river_sw)
            rotate(2);
        if (oter == overmap::ot_river_nw)
            rotate(3);
        break;
    default:
        return false;
        break;
    }
    return true;
}/*}}}*/

submap::ster_id submap::grass_or_dirt()
{
    return one_in(4) ? st_grass : st_dirt;
}

bool submap::gen_road(const overmap::oter& oter, map* mp)
{/*{{{*/
    if (!oter.is_road())
        return false;

    bool n = mp->get_oter(loc.x, loc.y - 1).is_building();
    bool s = mp->get_oter(loc.x, loc.y + 1).is_building();
    bool w = mp->get_oter(loc.x - 1, loc.y).is_building();
    bool e = mp->get_oter(loc.x + 1, loc.y).is_building();
    bool bb = n || s || w || e;

    switch (oter.type) {
    case overmap::ot_road_ns:
    case overmap::ot_road_ew:
        for (int i = 0; i < SMAPX; i++) {
            for (int j = 0; j < SMAPY; j++) {
                if (i < 4 || i >= SMAPX- 4) {
                    ter(i, j) = bb ? st_sidewalk : grass_or_dirt();
                } else {
                    if ((i == SMAPX / 2 || i == SMAPX / 2 - 1) && j % 4 != 0)
                        ter(i, j) = st_pavement_y;
                    else
                        ter(i, j) = st_pavement;
                }
            }
        }
        if (oter == overmap::ot_road_ew)
            rotate(1);
        break;
    case overmap::ot_road_ne:
    case overmap::ot_road_es:
    case overmap::ot_road_sw:
    case overmap::ot_road_wn:
        for (int i = 0; i < SMAPX; i++) {
            for (int j = 0; j < SMAPY; j++) {
                if ((i >= SMAPX - 4 && j < 4) || i < 4 || j >= SMAPY - 4) {
                    ter(i, j) = bb ? st_sidewalk : grass_or_dirt();
                } else {
                    if (((i == SMAPX / 2 - 1 || i == SMAPX / 2) && j % 4 != 0 && j < SMAPY / 2 - 1) 
                        || ((j == SMAPY / 2 - 1 || j == SMAPY / 2) && i % 4 != 0 && i > SMAPX / 2))
                        ter(i, j) = st_pavement_y;
                    else
                        ter(i, j) = st_pavement;
                }
            }
        }
        if (oter == overmap::ot_road_es)
            rotate(1);
        if (oter == overmap::ot_road_sw)
            rotate(2);
        if (oter == overmap::ot_road_wn)
            rotate(3);

    case overmap::ot_road_nes:
    case overmap::ot_road_new:
    case overmap::ot_road_nsw:
    case overmap::ot_road_esw:
        for (int i = 0; i < SMAPX; i++) {
            for (int j = 0; j < SMAPY; j++) {
                if (i < 4 || (i >= SMAPX - 4 && (j < 4 || j >= SMAPY - 4))) {
                    ter(i, j) = bb ? st_sidewalk : grass_or_dirt();
                } else {
                    if (((i == SMAPX / 2 - 1 || i == SMAPX / 2) && j % 4 != 0) ||
                        ((j == SMAPY / 2 - 1 || j == SMAPY) && i % 4 != 0 && i > SMAPX / 2))
                        ter(i, j) = st_pavement_y;
                    else
                        ter(i, j) = st_pavement;
                }
            }
        }
        if (oter == overmap::ot_road_esw)
            rotate(1);
        if (oter == overmap::ot_road_nsw)
            rotate(2);
        if (oter == overmap::ot_road_new)
            rotate(3);
        break;
    case overmap::ot_road_nesw:
        n = mp->get_oter(loc.x, loc.y - 1) == overmap::ot_road_nesw;
        s = mp->get_oter(loc.x, loc.y + 1) == overmap::ot_road_nesw;
        w = mp->get_oter(loc.x - 1, loc.y) == overmap::ot_road_nesw;
        e = mp->get_oter(loc.x + 1, loc.y) == overmap::ot_road_nesw;
        bb = n && s && w && e;

        for (int i = 0; i < SMAPX; i++) {
            for (int j = 0; j < SMAPY; j++) {
                if (bb)
                    ter(i, j) = st_sidewalk;
                else if ((i < 4 || i >= SMAPX - 4) && (j < 4 || j >= SMAPY - 4)) {
                    ter(i, j) = bb ? grass_or_dirt() : st_sidewalk;
                } else {
                    if (((i == SMAPX / 2 - 1 || i == SMAPX / 2) && j % 4 != 0) ||
                        ((j == SMAPY / 2 - 1 || j == SMAPY / 2) && i % 4 != 0))
                        ter(i, j) = st_pavement_y;
                    else
                        ter(i, j) = st_pavement;
                }
            }
        }
        break;
    default:
        break;
    }
    return true;
}/*}}}*/

bool submap::gen_house(const overmap::oter& oter, map* mp)
{
    return false;
}




