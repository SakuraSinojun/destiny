
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

    srand(loc.x * 65536 + loc.y);
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

    if (turns == 1 || turns == 3) {
        for (int x = 0; x < SMAPX; x++) {
            for (int y = 0; y < SMAPY; y++) {
                ster& st = ter(x, y, z);
                if (st == st_wall_h)
                    st = st_wall_v;
                else if (st == st_wall_v)
                    st = st_wall_h;
            }
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
    if (oter != overmap::ot_house_north 
        && oter != overmap::ot_house_east
        && oter != overmap::ot_house_south
        && oter != overmap::ot_house_west) {
        return false;
    }

    for (int i = 0; i < SMAPX; i++) {
        for (int j = 0; j < SMAPY; j++) {
            ter(i, j) = grass_or_dirt();
        }
    }
    gen_house_style2();

#if 0
    if (oter == overmap::ot_house_east)
        rotate(1);
    if (oter == overmap::ot_house_south)
        rotate(2);
    if (oter == overmap::ot_house_west)
        rotate(3);
#endif
    return true;
}

void submap::gen_house_style1()
{/*{{{*/
    int lw = rng(1, 4);
    int mw = lw + rng(7, 10);
    int rw = SMAPX - rng(1, 5);
    int tw = rng(1, 6);
    int bw = SMAPY - rng(2, 5);
    int cw = tw + rng(4, 7);

    // 外墙
    for (int i = 0; i < SMAPX; i++) {
        for (int j = 0; j < SMAPY; j++) {
            if (i > lw && i < rw && j > tw && j < bw)
                ter(i, j) = st_floor;
            else
                ter(i, j) = grass_or_dirt();
            if (i >= lw && i <= rw && (j == tw || j == bw))
                ter(i, j) = st_wall_h;
            if ((i == lw || i == rw) && j > tw && j < bw)
                ter(i, j) = st_wall_v;
        }
    }

    // 上面的房间分成两份
    {
        mw = rng(lw + 5, rw - 5);
        cw = rng(tw + 5, bw - 5);
        make_house_room(mw, tw, rw, cw);
        make_house_room(lw, tw, mw, cw);

        // 中间开个门
        ter(mw, rng(tw + 2, cw - 2)) = st_door_c;

        // 上面两个房间各开一到两个窗子
        int rn = rng(lw + 2, mw - 3);
        ter(rn    , tw) = st_window;
        if (one_in(2))
            ter(rn + 1, tw) = st_window;
        rn = rng(mw + 2, rw - 3);
        ter(rn    , tw) = st_window;
        if (one_in(2))
            ter(rn + 1, tw) = st_window;
    }
    int mw1 = mw;

    // 下面的房间
    mw = rng(lw + 3, rw - 3);
    int rn;
    // 左边房间小右边房间大
    if (mw <= (lw + rw) / 2) {
        rn = rng(mw + 2, rw - 2);
        if (bw - cw >= 10 && mw - lw >= 6) {
            make_house_room(lw, bw - 4, mw, bw);
            make_house_room(lw, cw, mw, bw - 4);
            ter(rng(lw + 1, mw - 1), cw) = st_door_c;
            ter(rng(mw1 + 1, rw - 1), cw) = st_door_c;
            ter(mw, rng(bw - 3, bw - 1)) = st_door_c;
            ter(rng(lw + 1, mw - 1), bw - 4) = st_door_c;
        } else {
            make_house_room(lw, cw, mw, bw);
            int m = (mw > mw1) ? mw1 : mw;
            for (int i = lw + 1; i <= m - 1; i++)
                ter(i, cw) = st_floor;
            ter(mw, rng(cw + 1, bw - 1)) = st_door_c;
        }
    } else {
        rn = rng(lw + 2, mw - 2);
        if (bw - cw >= 10 && rw - mw >= 6) {
            make_house_room(mw, bw - 4, rw, bw);
            make_house_room(mw, cw, rw, bw - 4);
            ter(rng(mw + 1, rw - 1), cw) = st_door_c;
            ter(rng(lw + 1, mw1 - 1), cw) = st_door_c;
            ter(mw, rng(bw - 3, bw - 1)) = st_door_c;
            ter(rng(mw + 1, rw - 1), bw - 4) = st_door_c;
        } else {
            make_house_room(mw, cw, rw, bw);
            int m = (mw > mw1) ? mw : mw1;
            for (int i = m + 1; i < rw; i++) {
                ter(i, cw) = st_floor;
            }
            ter(mw, rng(cw + 1, bw - 1)) = st_door_c;
        }
    }
    ter(rn    , bw) = st_window;
    ter(rn + 1, bw) = st_window;
    if (!one_in(3)) {
        rn = rng(tw + 2, bw - 5);
        ter(rw, rn    ) = st_window;
        ter(rw, rn + 4) = st_window;
    }
    if (!one_in(3)) {
        rn = rng(tw + 2, bw - 5);
        ter(lw, rn    ) = st_window;
        ter(lw, rn + 4) = st_window;
    }

    if (one_in(2)) {
        int x = rng(lw + 2, mw1 - 1);
        ter(x - 1, tw) = st_wall_h;
        ter(x + 1, tw) = st_wall_h;
        ter(x, tw) = (one_in(6) ? st_door_c : st_door_locked);
        if (one_in(5)) {
            int y = rng(tw + 2, cw - 2);
            ter(rw, y - 1) = st_wall_v;
            ter(rw, y + 1) = st_wall_v;
            ter(rw, y) = (one_in(6) ? st_door_c : st_door_locked);
        }
    } else {
        int x = rng(lw + 2, mw1 - 1);
        ter(x - 1, tw) = st_wall_h;
        ter(x, tw) = (one_in(6) ? st_door_c : st_door_locked);
        ter(x + 1, tw) = st_wall_h;
        if (one_in(5)) {
            int y = rng(tw + 2, cw - 2);
            ter(lw, y - 1) = st_wall_v;
            ter(lw, y + 1) = st_wall_v;
            ter(lw, y) = (one_in(6) ? st_door_c : st_door_locked);
        }
    }
}/*}}}*/

void submap::gen_house_style2()
{
    int lw = rng(2, 4);
    int rw = SMAPX - rng(2, 5);
    int tw = rng(2, 4);
    int bw = SMAPY - rng(2, 5);
    make_house_room(lw, tw, rw, bw);

    int mw = (bw + tw) / 2;
    int cw = (lw + rw) / 2;
    int mw1 = rng(tw + 4, mw - 2);
    int mw2 = rng(mw + 2, bw - 4);
    int cw1 = rng(lw + 4, cw - 2);
    int cw2 = rng(cw + 2, rw - 4);

    // 卐 型或者 卍 型。
    if (one_in(2)) {
        make_house_room(lw, tw, cw2, mw1);
        make_house_room(cw1, mw2, rw, bw);
        make_house_room(cw1, mw1, cw2, mw2);
    } else {
        make_house_room(lw, tw, cw1, mw2);
        make_house_room(cw2, mw1, rw, bw);
        make_house_room(cw1, mw1, cw2, mw2);
    }

    ter(rng(cw1 + 1, cw2 - 1), mw1) = st_door_c;
    ter(rng(cw1 + 1, cw2 - 1), mw2) = st_door_c;
    ter(cw1, rng(mw1 + 1, mw2 - 1)) = st_door_c;
    ter(cw2, rng(mw1 + 1, mw2 - 1)) = st_door_c;

    // 随机砍掉一堵墙。
    mw = rng(0, 4);
    if (mw == 0) {
        for (int i = cw1 + 1; i < cw2; i++)
            ter(i, mw1) = st_floor;
    } else if (mw == 1) {
        for (int j = mw1 + 1; j < mw2; j++)
            ter(cw2, j) = st_floor;
    } else if (mw == 2) {
        for (int i = cw1 + 1; i < cw2; i++)
            ter(i, mw2) = st_floor;
    } else if (mw == 3) {
        for (int j = mw1 + 1; j < mw2; j++)
            ter(cw1, j) = st_floor;
    } else {
    }

    cw = rng(lw + 2, rw - 2);
    if (cw == cw1 || cw == cw2)
        cw++;
    ter(cw, tw) = st_door_c;
}

void submap::make_house_room(int x0, int y0, int x1, int y1) 
{
    for (int i = x0; i <= x1; i++) {
        for (int j = y0; j <= y1; j++) {
            ster& st = ter(i, j);
            if (j == y0 || j == y1) {
                st = st_wall_h;
            } else if (i == x0 || i == x1) {
                st = st_wall_v;
            } else {
                st = st_floor;
            }
        }
    }
}






















