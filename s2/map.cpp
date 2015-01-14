
#include <stdlib.h>
#include <stdio.h>

#include "map.h"
#include "game.h"


map::map(game& g)
    : m_game(g)
{
}

void map::load(const char * name)
{
}


overmap* map::get_overmap(const int x, const int y, bool generate_if_not_exists)
{
    for (std::list<overmap>::iterator it = overmap_list.begin();
        it != overmap_list.end(); it++) {
        if (it->pos().x == x && it->pos().y == y) {
            return &(*it);
        }
    }

    if (generate_if_not_exists) {
        overmap new_overmap(m_game, x, y);
        overmap_list.push_back(new_overmap);
        return &overmap_list.back();
    }
    return NULL;
}

submap* map::get_submap(const int x, const int y)
{
    for (std::list<submap>::iterator it = submap_list.begin(); it != submap_list.end(); it++) {
        if (it->pos().x == x && it->pos().y == y) {
            return &(*it);
        }
    }

    // point   op = x2om(point(x, y));
    // overmap* om = get_overmap(op.x, op.y);

    submap  new_submap(this, x, y);
    submap_list.push_back(new_submap);
    return &submap_list.back();
}

overmap::oter map::get_oter(int x, int y, int z)
{
    point sp = x2om(point(x, y));
    point st = x2oter(point(x, y));
    overmap* m = get_overmap(sp.x, sp.y);
    return m->ter(st.x, st.y, z);
}

submap::ster map::get_ster(int x, int y, int z)
{
    point   sp = x2sm(point(x, y));
    point   st = x2ster(point(x, y));
    submap* m = get_submap(sp.x, sp.y);
    return m->ter(st.x, st.y, z);
}

point map::x2sm(const point& pt)
{
    int x = pt.x, y = pt.y;
    int smx = (x >= 0) ? (x / SMAPX) : (((x + 1) / SMAPX) - 1);
    int smy = (y >= 0) ? (y / SMAPY) : (((y + 1) / SMAPY) - 1);
    return point(smx, smy);
}

point map::x2ster(const point& pt)
{
    int x = pt.x, y = pt.y;
    int terx = (x >= 0) ? (x % SMAPX) : (SMAPX - 1 - (abs(x + 1) % SMAPX));
    int tery = (y >= 0) ? (y % SMAPY) : (SMAPY - 1 - (abs(y + 1) % SMAPY));
    return point(terx, tery);
}

point map::x2om(const point pt)
{
    int x = pt.x, y = pt.y;
    int omx = (x >= 0) ? (x / OMAPX) : (((x + 1) / OMAPX) - 1);
    int omy = (y >= 0) ? (y / OMAPY) : (((y + 1) / OMAPY) - 1);
    return point(omx, omy);
}

point map::x2oter(const point& pt)
{
    int x = pt.x, y = pt.y;
    int terx = (x >= 0) ? (x % OMAPX) : (OMAPX - 1 - (abs(x + 1) % OMAPX));
    int tery = (y >= 0) ? (y % OMAPY) : (OMAPY - 1 - (abs(y + 1) % OMAPY));
    return point(terx, tery);
}




















