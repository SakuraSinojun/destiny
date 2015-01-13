

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include <string>
#include <string.h>

#include <ncurses.h>
#include <list>
#include <vector>

#include "tools/logging.h"
#include "tools/dump.h"

#define MAPWIDTH    150
#define MAPHEIGHT   40
#define BLOCKSIZE   10
#define ROADDEPTH   5

class Road {
public:
    Road() : weight(0), width(1), x0(0), y0(0), x1(0), y1(0) {}
    int weight;     // 0: main road
    int width;      // road width
    int x0, y0;
    int x1, y1;
    int lastcrossx, lastcrossy;
    std::string name;
    void dump() {
        SLOG() << "Road: " << weight << ", " << width << ", (" << x0 << ", " << y0 << ") - (" << x1 << ", " << y1 << ").";
    }
    bool isAvailable(int mapwidth = MAPWIDTH, int mapheight = MAPHEIGHT) {
        if (x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0)
            return false;
        if (x0 >= mapwidth || x1 >= mapwidth || y0 >= mapheight || y1 >= mapheight)
            return false;
        return true;
    }
    bool isOverlay(const Road& o) {
        if ((x1 == x0) && (o.x1 != o.x0))
            return false;
        if ((y1 == y0) && (o.y1 != o.y0))
            return false;
        int imin, imax, omin, omax;
        if (x1 == x0) {
            if (abs(o.x0 - x0) > BLOCKSIZE)
                return false;
            imin = std::min(y0, y1);
            imax = std::max(y0, y1);
            omin = std::min(o.y0, o.y1);
            omax = std::max(o.y0, o.y1);
        } else {
            if (abs(o.y0 - y0) > BLOCKSIZE)
                return false;
            imin = std::min(x0, x1);
            imax = std::max(x0, x1);
            omin = std::min(o.x0, o.x1);
            omax = std::max(o.x0, o.x1);
        }
        if (imax <= omin || omax <= imin)
            return false;
        return true;
    }
};

bool chance(int pert)
{
    int c = rand() % 100;
    if (c < pert) {
        return true;
    }
    return false;
}

bool isFarAway(Road& r, std::list<Road>& result)
{
    // int dx = r.x1 - r.x0;
    // int dy = r.y1 - r.y0;

    if (result.empty())
        return true;

    std::list<Road>::iterator it;
    for (it = result.begin(); it != result.end(); it++) {
        Road& road = (*it);

        if (road.isOverlay(r)) {
            return false;
        }
#if 0
        if (dx == 0) {
            if (abs(road.x0 - r.x0) < 3) {
                int ddy = r.y1 - r.y0;
                if (dy * ddy > 0) {
                    if (r.y0 >= std::min(road.y0, road.y1) && r.y0 <= std::max(road.y0, road.y1)) {
                        return false;
                    }
                } else {
                    if (r.y0 > std::min(road.y0, road.y1) && r.y0 < std::max(road.y0, road.y1)) {
                        return false;
                    }
                }
            }
        } else {
            if (abs(road.y0 - r.y0) < 3) {
                int ddx = r.x1 - r.x0;
                if (dx * ddx > 0) {
                    if (r.x0 >= std::min(road.x0, road.x1) && r.x0 <= std::max(road.x0, road.x1)) {
                        return false;
                    }
                } else {
                    if (r.x0 > std::min(road.x0, road.x1) && r.x0 < std::max(road.x0, road.x1)) {
                        return false;
                    }
                }
            }
        }
#endif
    }
    return true;
}

class point {
public:
    point(int _x, int _y) : x(_x), y(_y) {}
    int x;
    int y;
};

long rng(long low, long high)
{
    return low + long((high - low + 1) * double(rand() / double(RAND_MAX + 1.0)));
}

int river_test()
{
    char    gmap[MAPHEIGHT][MAPWIDTH];
    int     i, j;
    for (i = 0; i < MAPWIDTH; i++) {
        for (j = 0; j < MAPHEIGHT; j++) {
            gmap[j][i] = ' ';
        }
    }
 
    std::vector<point>  river_start;
    std::vector<point>  river_end;
    std::vector<point>  new_rivers;

    new_rivers.clear();
    new_rivers.push_back(point(rng(10, MAPWIDTH - 11), 0));
    new_rivers.push_back(point(0, rng(10, MAPHEIGHT - 11)));
    river_start.push_back(new_rivers[rng(0, new_rivers.size() - 1)]);

    new_rivers.clear();
    new_rivers.push_back(point(rng(10, MAPWIDTH - 11), MAPHEIGHT - 1));
    new_rivers.push_back(point(MAPWIDTH - 1, rng(10, MAPHEIGHT - 11)));
    river_end.push_back(new_rivers[rng(0, new_rivers.size() - 1)]);


    point   pa = river_start[0];
    point   pb = river_end[0];

    int x = pa.x, y = pa.y;
    do {
        x += rng(-1, 1);
        y += rng(-1, 1);
        if (x < 0) x = 0;
        if (x > MAPWIDTH - 1) x = MAPWIDTH - 2;
        if (y < 0) y = 0;
        if (y > MAPHEIGHT - 1) y = MAPHEIGHT - 1;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j<= 1; j++) {
                if (y + i >= 0 && y + i < MAPHEIGHT && x + j >= 0 && x + j < MAPWIDTH) {
                    DCHECK(y + i >= 0 && y + i < MAPHEIGHT) << "y = " << y << ", y + i = " << y + i;
                    DCHECK(x + j >= 0 && x + j < MAPWIDTH) << "x = " << x << ", x + j = " << x + i;
                    gmap[y + i][x + j] = '~';
                }
            }
        }

#if 1
        if (pb.x > x && (rng(0, int(MAPWIDTH * 1.2) - 1) < pb.x - x ||
                         (rng(0, int(MAPWIDTH * .2) - 1) > pb.x - x &&
                          rng(0, int(MAPHEIGHT* .2) - 1) > abs(pb.y - y))))
            x++;
        if (pb.x < x && (rng(0, int(MAPWIDTH * 1.2) - 1) < x - pb.x ||
                         (rng(0, int(MAPWIDTH * .2) - 1) > x - pb.x &&
                          rng(0, int(MAPHEIGHT* .2) - 1) > abs(pb.y - y))))
            x--;
        if (pb.y > y && (rng(0, int(MAPHEIGHT * 1.2) - 1) < pb.y - y ||
                         (rng(0, int(MAPHEIGHT* 0.2) - 1) > pb.y - y &&
                          rng(0, int(MAPWIDTH * 0.2) - 1) > abs(x - pb.x))))
            y++;
        if (pb.y < y && (rng(0, int(MAPHEIGHT * 1.2) - 1) < y - pb.y ||
                         (rng(0, int(MAPHEIGHT * .2) - 1) > y - pb.y &&
                          rng(0, int(MAPWIDTH * 0.2) - 1) > abs(x - pb.x))))
            y--;
#else
        if (rng(0, 100) < 20) {
            if (pb.x > x)
                x++;
            else
                x--;
        }

        if (rng(0, 100) < 20) {
            if (pb.y > y)
                y++;
            else
                y--;
        }
#endif
#if 0
        x += rng(-1, 1);
        y += rng(-1, 1);
        if (x < 0) x = 0;
        if (x > MAPWIDTH - 1) x = MAPWIDTH - 2;
        if (y < 0) y = 0;
        if (y > MAPHEIGHT - 1) y = MAPHEIGHT - 1;

        for (int i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if ((y + i >= 1 && y + i < MAPHEIGHT - 1 && x + j >= 1 && x + j < MAPWIDTH - 1) ||
                    (false && abs(pb.y - (y + i)) < 4 && abs(pb.x - (x + j)) < 4)) {
                    DCHECK(y + i >= 0 && y + i < MAPHEIGHT) << "y = " << y << ", y + i = " << y + i;
                    DCHECK(x + j >= 0 && x + j < MAPWIDTH) << "x = " << x << ", x + j = " << x + i;
                    gmap[y + i][x + j] = '~';
                }
            }
        }
#endif
    } while (pb.x != x || pb.y != y);

    gmap[pa.y][pa.x] = 'O';
    gmap[pb.y][pb.x] = 'O';

    for (int j = 0; j < MAPHEIGHT; j++) {
        for (int i = 0; i < MAPWIDTH; i++) {
            printf("%c", gmap[j][i]);
        }
        printf("\n");
    }

    return 0;
}



//      LINE_NESW  - X for on, O for off
#define LINE_XOXO 4194424
#define LINE_OXOX 4194417
#define LINE_XXOO 4194413
#define LINE_OXXO 4194412
#define LINE_OOXX 4194411
#define LINE_XOOX 4194410
#define LINE_XXXO 4194420
#define LINE_XXOX 4194422
#define LINE_XOXX 4194421
#define LINE_OXXX 4194423
#define LINE_XXXX 4194414

int main()
{
    unsigned int seed = time(NULL);

    seed = 1421125093;

    printf("seed = %u\n", seed);
    srand(seed);

    return river_test();

    // gen road
    std::list<Road> result;
    std::list<Road> roads;

    Road    mainroad;
    mainroad.weight = 0;
    mainroad.width = 3;
    mainroad.x0 = MAPWIDTH / 2;
    mainroad.y0 = 0;
    mainroad.x1 = mainroad.x0;
    mainroad.y1 = mainroad.y0 + 1;
    mainroad.lastcrossx = mainroad.x0;
    mainroad.lastcrossy = mainroad.y0;

    roads.push_back(mainroad);

    mainroad.weight = 0;
    mainroad.width = 3;
    mainroad.x0 = 0;
    mainroad.y0 = MAPHEIGHT / 2;
    mainroad.x1 = mainroad.x0 + 1;
    mainroad.y1 = mainroad.y0;
    mainroad.lastcrossx = mainroad.x0;
    mainroad.lastcrossy = mainroad.y0;
    roads.push_back(mainroad);


    while (!roads.empty()) {
        Road& road = roads.front();
        if (!isFarAway(road, result)) {
            roads.pop_front();
            continue;
        }

        int dx = road.x1 - road.x0;
        int dy = road.y1 - road.y0;
        
        int length = (dx != 0) ? (dx > 0 ? (road.x1 - road.lastcrossx) : (road.lastcrossx - road.x1)) : (dy > 0 ? (road.y1 - road.lastcrossy) : (road.lastcrossy - road.y1));

        dx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        dy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

        bool cross = length > BLOCKSIZE ? chance(20) : false;
        bool Tjunction = chance(50);
        bool Tdirection = chance(50);
        bool end = cross ? chance(road.weight * 6) : (length > BLOCKSIZE ? chance(road.weight * 5) : false);

        if (dx == 0) {
            if (cross) {
                Road r;
                r.weight = road.weight + 1;
                if (r.weight < ROADDEPTH) {
                    r.x0 = road.x1 + (Tdirection ? road.width : -1);
                    r.y0 = road.y1;
                    r.x1 = r.x0 + (Tdirection ? 1 : -1);
                    r.y1 = r.y0;
                    r.lastcrossx = r.x0;
                    r.lastcrossy = r.y0;
                    if (r.isAvailable())
                        roads.push_back(r);
                    if (!Tjunction) {
                        r.x1 = r.x0 + (Tdirection ? -1 : 1);
                        r.lastcrossx = r.x0;
                        r.lastcrossy = r.y0;
                        if (r.isAvailable())
                            roads.push_back(r);
                    }
                    road.lastcrossx = road.x1;
                    road.lastcrossy = road.y1;
                }
            }
            if (end) {
                result.push_back(road);
                roads.pop_front();
            } else {
                int y = road.y1 + dy;
                if (y < 0 || y >= MAPHEIGHT) {
                    // end
                    result.push_back(road);
                    roads.pop_front();
                } else {
                    road.y1 = y;
                }
            }
        } else {
            if (cross) {
                Road r;
                r.weight = road.weight + 1;
                if (r.weight < ROADDEPTH) {
                    r.x0 = road.x1;
                    r.y0 = road.y1 + (Tdirection ? road.width : -1);
                    r.x1 = r.x0;
                    r.y1 = r.y0 + (Tdirection ? 1 : -1);
                    r.lastcrossx = r.x0;
                    r.lastcrossy = r.y0;
                    if (r.isAvailable())
                        roads.push_back(r);
                    if (!Tjunction) {
                        r.y1 = r.y0 + (Tdirection ? -1 : 1);
                        r.lastcrossx = r.x0;
                        r.lastcrossy = r.y0;
                        if (r.isAvailable())
                            roads.push_back(r);
                    }
                    road.lastcrossx = road.x1;
                    road.lastcrossy = road.y1;
                }
            }
            if (end) {
                result.push_back(road);
                roads.pop_front();
            } else {
                int x = road.x1 + dx;
                if (x < 0 || x >= MAPWIDTH) {
                    result.push_back(road);
                    roads.pop_front();
                } else {
                    road.x1 = x;
                }
            }
        }
    }


    char    gmap[MAPHEIGHT][MAPWIDTH];
    int     i, j;

    for (i = 0; i < MAPWIDTH; i++) {
        for (j = 0; j < MAPHEIGHT; j++) {
            gmap[j][i] = ' ';
        }
    }

    // const char * _table = "01234567";

    std::list<Road>::iterator it;
    for (it = result.begin(); it != result.end(); it++) {
        // (*it).dump();
        Road& r = *it;

        int dx = r.x1 - r.x0;
        int dy = r.y1 - r.y0;

        if (dx == 0) {
            dy = (dy > 0) ? 1 : -1;
            for (int y = r.y0; y != r.y1; y += dy) {
                DCHECK(y >= 0 && y < MAPHEIGHT) << "y = " << y << ", r.y1 = " << r.y1;
                if (gmap[y][r.x0] == '-') {
                    gmap[y][r.x0] = '+'; // _table[r.weight];
                } else {
                    gmap[y][r.x0] = '|'; // _table[r.weight];
                }
            }
        } else {
            dx = (dx > 0) ? 1 : -1;
            for (int x = r.x0; x != r.x1; x += dx) {
                DCHECK(x >= 0 && x < MAPWIDTH);
                if (gmap[r.y0][x] == '|') {
                    gmap[r.y0][x] = '+';
                } else {
                    gmap[r.y0][x] = '-';
                }
            }
        }
    }

    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            printf("%c", gmap[j][i]);
        }
        printf("\n");
    }
    // RUN_HERE();

    return 0;
}
















