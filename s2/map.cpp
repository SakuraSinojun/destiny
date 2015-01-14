
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

void map::add_overmap(overmap& over)
{
    overmap_list.push_back(over);
}

overmap::oter map::get_terrain(int x, int y, int z)
{
    int layerx = (x >= 0) ? (x / OMAPX) : (((x + 1) / OMAPX) - 1);
    int layery = (y >= 0) ? (y / OMAPY) : (((y + 1) / OMAPY) - 1);
    int terx = (x >= 0) ? (x % OMAPX) : (OMAPX - 1 - (abs(x + 1) % OMAPX));
    int tery = (y >= 0) ? (y % OMAPY) : (OMAPY - 1 - (abs(y + 1) % OMAPY));
    
    // printf("(%d, %d) => (%d, %d)\n", layerx, layery, terx, tery);
    overmap* m = get_overmap(layerx, layery);

    // if (x == 5 && y == 5)
    //     return overmap::ot_rock;
    return m->ter(terx, tery, z);
    // return m->layer[z + OVERMAP_DEPTH].terrain[terx][tery];
}


