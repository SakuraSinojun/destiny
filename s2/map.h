
#pragma once

#include <list>

#include "overmap.h"

class game;
class map
{
public:
    map(game& g);
    void load(const char * name);

    overmap* get_overmap(const int x, const int y, bool generate_if_not_exists = true);
    void     add_overmap(overmap& over);

    overmap::oter get_terrain(int x, int y, int z = 0);

private:
    std::list<overmap>  overmap_list;
    game&   m_game;
};

