
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

    int chance = 10;
    chance += (t == overmap::ot_forest) ? 0 : ((t == overmap::ot_forest_thick) ? 8 : 4);
    chance += (n == overmap::ot_forest) ? 0 : ((n == overmap::ot_forest_thick) ? 8 : 4);
    chance += (s == overmap::ot_forest) ? 0 : ((s == overmap::ot_forest_thick) ? 8 : 4);
    chance += (w == overmap::ot_forest) ? 0 : ((w == overmap::ot_forest_thick) ? 8 : 4);
    chance += (e == overmap::ot_forest) ? 0 : ((e == overmap::ot_forest_thick) ? 8 : 4);
    chance *= 100;
    // 20, 60, 100
    
    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ster& st = ter(i, j);
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







