
#include <stdlib.h>
#include "submap.h"
#include "overmap.h"
#include "rng.h"
#include "map.h"

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
    // ster_id id = st_null;
    if (oter == overmap::ot_null) {
        gen_full(st_null);
    } else if (oter == overmap::ot_field) {
        gen_full(st_field);
    } else if (oter == overmap::ot_rock) {
    } else if (oter.is_water()) {
        gen_full(st_river);
    } else if (oter.is_road()) {
        gen_full(st_road);
    } else if (oter.is_building()) {
        gen_full(st_house);
    } else if (oter.is_forest()) {
        gen_forest(mp);
    } else {
    }
}

void submap::gen_full(ster_id id)
{
    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ter(i, j) = id;
        }
    }
}

void submap::gen_field()
{
    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ster& st = ter(i, j);
            st = one_in(4) ? st_grass : st_dirt;

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
}

void submap::gen_forest(map* mp)
{
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
                st = st_dirt;
            }
        }
    }
}







