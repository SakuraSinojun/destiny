
#pragma once

#include <list>

#include "overmap.h"
#include "submap.h"
#include "point.h"

class game;
class map
{
public:
    map(game& g);
    void load(const char * name);

    overmap* get_overmap(const int x, const int y, bool generate_if_not_exists = true);

    submap::ster get_ster(int x, int y, int z = 0);
    overmap::oter get_oter(int x, int y, int z = 0);

    static point x2sm(const point& pt);
    static point x2om(const point pt);
    static point x2ster(const point& pt);
    static point x2oter(const point& pt);


    int get_overmap_list_size() { return overmap_list.size(); }
    int get_submap_list_size() { return submap_list.size(); }

    // for rotate test.
    submap*  get_current_submap(const int standx, const int standy);

private:
    std::list<overmap>  overmap_list;
    std::list<submap>   submap_list;

    game&   m_game;

    submap*  get_submap(const int x, const int y);
};

