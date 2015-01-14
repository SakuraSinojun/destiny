
#include "submap.h"
#include "overmap.h"
#include <stdlib.h>

submap::submap(overmap* om, int x, int y)
    : loc(x, y)
    , otx((x >= 0) ? (x % OMAPX) : (OMAPX - 1 - (abs(x + 1) % OMAPX)))
    , oty((y >= 0) ? (y % OMAPY) : (OMAPY - 1 - (abs(y + 1) % OMAPY)))
{
    generate(om);
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

void submap::generate(overmap* om)
{
    overmap::oter& oter = om->ter(otx, oty);
    ster_id id = st_null;
    if (oter.is_road()) {
        id = st_road;
    } else if (oter.is_building()) {
        id = st_house;
    } else if (oter.is_river()) {
        id = st_river;
    } else {
        id = st_field;
    }

    for (int j = 0; j < SMAPY; j++) {
        for (int i = 0; i < SMAPX; i++) {
            ter(i, j) = id;
        }
    }
}








