

#include "gamemap.h"
#include "dice.h"
#include "ui.h"

#include <sstream>
#include <string>
#include <stdlib.h>

#include "log.h"
#include "tools/dump.h"


static Map::Node * emptyNode = NULL;
static Map* g_map = NULL;

Map* Map::Get(void)
{
    if (!g_map)
        g_map = new Map();
    return g_map;
}

Map::Map()
    : herox(0), heroy(0)
{
    GenChunks();
}

Map::~Map()
{
}

Map::Node* Map::uiat(int x, int y)
{
    int rx, ry;
    rx = herox + x;
    ry = heroy + y;
    return at(rx, ry);
}

Map::Node* Map::at(int x, int y, bool gen)
{
    ChunkId id = GetChunkIdFromPos(x, y);
    Chunk* c = GetChunkFromId(id);
    
    if (!c && gen)
        c = GenChunk(id);

    if (!c) {
        if (!emptyNode) {
            emptyNode = new Map::Node();
        }
        return emptyNode;
    }
    int mx = GetMapXFromChunkId(id);
    int my = GetMapYFromChunkId(id);
    int dx = x - mx;
    int dy = y - my;
    Node* n = &(c->nodes[dy][dx]);
    return n;
}

void Map::CenterMap(int x, int y)
{
    herox = x;
    heroy = y;
    GenChunks();
}

void Map::GenChunks(void)
{
    const int GENATONCE = 3;
    const int GENRANGE = 5;

    ChunkId id = GetCurrentChunkId();
    int x = GetXFromChunkId(id);
    int y = GetYFromChunkId(id);
    int i, j;

    std::map<ChunkId, Chunk*>::iterator it;
    for (i = x - GENATONCE; i < x + GENATONCE; i++) {
        for (j = y - GENATONCE; j < y + GENATONCE; j++) {
            id = GenChunkId(i, j);
            if (!IsChunkGenerated(id)) {
                GenChunk(id);
            }
        }
    }

    for (j = y - GENATONCE; j < y + GENATONCE; j++) {
        id = GenChunkId(x + GENATONCE, j);
        if (!IsChunkGenerated(id)) {
            for (i = x + GENATONCE; i < x + GENRANGE + GENATONCE; i++) {
                id = GenChunkId(i, j);
                GenChunk(id);
            }
        }
        id = GenChunkId(x - GENATONCE - 1, j);
        if (!IsChunkGenerated(id)) {
            for (i = x - GENATONCE - 1; i > x - GENATONCE - GENRANGE; i--) {
                id = GenChunkId(i, j);
                GenChunk(id);
            }
        }
    }

    for (i = x - GENATONCE; i < x + GENATONCE; i++) {
        id = GenChunkId(i, y + GENATONCE);
        if (!IsChunkGenerated(id)) {
            for (j = y + GENATONCE; j < y + GENRANGE + GENATONCE; j++) {
                id = GenChunkId(i, j);
                GenChunk(id);
            }
        }
        id = GenChunkId(i, y - GENATONCE - 1);
        if (!IsChunkGenerated(id)) {
            for (j = y - GENATONCE - 1; j > y - GENATONCE - GENRANGE; j--) {
                id = GenChunkId(i, j);
                GenChunk(id);
            }
        }
    }

}


bool Map::IsChunkGenerated(ChunkId id)
{
    std::map<ChunkId, Chunk*>::iterator it;
    it = chunks.find(id);
    if (it == chunks.end())
        return false;
    return true;
}

bool Map::IsChunkGenerated(int x, int y)
{
    ChunkId id = GenChunkId(x, y);
    return IsChunkGenerated(id);
}

Map::Chunk* Map::GetChunkFromId(ChunkId id)
{
    std::map<ChunkId, Map::Chunk*>::iterator it;
    it = chunks.find(id);
    if (it == chunks.end())
        return NULL;
    return it->second;
}

Map::ChunkId Map::GetCurrentChunkId(void)
{
    return GetChunkIdFromPos(herox, heroy);
}

Map::ChunkId Map::GetChunkIdFromPos(int x, int y)
{
    if (x < 0) {
        x -= (CHUNKWIDTH - 1);
    }
    
    if (y < 0) {
        y -= (CHUNKHEIGHT - 1);
    }
    x /= CHUNKWIDTH;
    y /= CHUNKHEIGHT;
    return GenChunkId(x, y);
}

int Map::GetXFromChunkId(ChunkId id)
{
    return id.x;
}

int Map::GetYFromChunkId(ChunkId id)
{
    return id.y;
}

int Map::GetMapXFromChunkId(ChunkId id)
{
    return CHUNKWIDTH * id.x; //GetXFromChunkId(id);
}

int Map::GetMapYFromChunkId(ChunkId id)
{
    return CHUNKHEIGHT * id.y; //GetYFromChunkId(id);
}

Map::ChunkId Map::GenChunkId(int x, int y)
{
    ChunkId id(x, y);
    return id;
}

Map::Chunk* Map::GenChunk(ChunkId id)
{
    Chunk* c = new Chunk(this, id);
    if (c) {
        chunks[id] = c;
    }
    // Debug() << "GenChunk " << id;
    return c;
}

bool Map::IsNodeCanBeSeen(Node* n, Node* from)
{
#if 0
#define SGN(a) (((a) < 0) ? -1 : 1)

    int t, x, y, ax, ay, sx, sy, dx, dy;
    int st;
    int tc;
    dx = from->x - n->x;
    dy = from->y - n->y;

    ax = abs(dx) << 1;
    ay = abs(dy) << 1;
    sx = SGN(dx);
    sy = SGN(dy);
    x = n->x;
    y = n->y;
    if (ax > ay) {
        st = SGN(ay - (ax >> 1));
        for (tc = abs(ay - (ax >>1)) * 2 + 1; tc >= -1; tc--) {
            t = tc * st;
            x = n->x;
            y = n->y;
        }
        do {
            if (t > 0) {
                y += sy;
                t -= ax;
            }
            x += sx;
            t += ay;
            if (x == from->x && y == from->y) {
                tc *= st;
                return true;
            }
        } while (!at(x, y)->blocksight);
        return false;
    } else if (ax < ay) {
        st = SGN(ax - (ay >> 1));
        for (tc = abs(ax - (ay >> 1)) * 2 + 1; tc >= -1; tc--) {
            t = tc * st;
            x = n->x;
            y = n->y;
        }
        do {
            if (t > 0) {
                x += sx;
                t -= ay;
            }
            y += sy;
            t += ax;
            if (x == from->x && y == from->y) {
                tc *= st;
                return  true;
            }
        } while (!at(x, y)->blocksight);
        return false;
    } else {
        return true;
    }
#else
    // RUN_HERE() << "n: " << n->desc() << " | from: " << from->desc();
    int dx = n->x - from->x;
    int dy = n->y - from->y;
    int ax = abs(dx);
    int ay = abs(dy);

    if (dx == 0 && dy == 0)
        return true;

    if (dx == 0) {
        int y;
        int sy = dy / abs(dy);
        for (y = from->y + sy; (n->y > from->y) ? (y < n->y) : (y > n->y) > 0; y += sy) {
            Node * nd = at(n->x, y);
            if (nd->blocksight)
                return false;
        }
        return true;
    }
    
    if (dy == 0) {
        int x;
        int sx = dx / abs(dx);
        for (x = from->x; (n->x > from->x) ? (x < n->x) : (x > n->x); x += sx) {
            Node * nd = at(x, n->y);
            if (nd->blocksight)
                return false;
        }
        return true;
    }
    
    int sx = dx / abs(dx);
    int sy = dy / abs(dy);

    if (ax == ay) {
        int x, y;
        for (x = from->x, y = from->y; (n->x > from->x) ? (x < n->x) : (x > n->x); x += sx, y += sy) {
            Node* nd = at(x, y);
            if (nd->blocksight)
                return false;
        }
    } else if (ax > ay) {
        int x, y;
        for (x = from->x; (n->x > from->x) ? (x < n->x) : (x > n->x); x += sx) {
            y = from->y + (x - from->x) * dy / dx;
            Node* nd = at(x, y);
            if (nd->blocksight)
                return false;
        }
    } else if (ax < ay) {
        int x, y;
        for (y = from->y; (n->y - y) * sy > 0; y += sy) {
            x = from->x + (y - from->y) * dx / dy;
            Node* nd = at(x, y);
            if (nd->blocksight)
                return false;
        }
    }

    return true;
#endif

}

bool Map::IsNodeCanBeSeenByHero(Node* n)
{
    Node from;
    from.x = herox;
    from.y = heroy;
    return IsNodeCanBeSeen(n, &from);
}


Map::Terrain* Map::NearestTerrain(int x, int y)
{
    const int range = TERRAINRANGE * CHUNKWIDTH * TERRAINRANGE * CHUNKWIDTH;
    std::list<Terrain*>::iterator   it;
    Map::Terrain* ter = NULL;
    int           dist = 0;
    for (it = ters.begin(); it != ters.end(); it++) {
        Terrain* t = *it;
        if (t->Distance(x, y) <= range || t->Distance(x, y) <= t->Radius()) {
            if (!ter) {
                ter = t;
                dist = ter->Distance(x, y);
            } else {
                if (t->Distance(x, y) < dist) {
                    ter = t;
                    dist = t->Distance(x, y);
                }
            }
        }
    }
    if (!ter) {
        ter = new Terrain(x, y);
        ters.push_back(ter);
    }
    return ter;
}


void Map::Chunk::MakeRandomNode(void)
{
    int  x = CHUNKWIDTH * id.x;
    int  y = CHUNKHEIGHT * id.y;

    int d = dice(100, 1);
    Map::Node* n;
    int j, i;
    for (j=0; j<CHUNKHEIGHT; j++) {
        for (i=0; i<CHUNKWIDTH; i++) {
            nodes[j][i].x = x + i;
            nodes[j][i].y = y + j;
            n = &nodes[j][i];
            n->type = NODE_GROUND;
            n->blocksight = false;
            n->canpass = true;
            n->icon = '.';
        }
    }
    return;
    if (d < 50) {
        int dd = dice(31, 1);
        if (dd == 31) {
            for (j=0; j<CHUNKHEIGHT; j++) {
                for (i=0; i<CHUNKWIDTH; i++) {
                    int ddd = dice(20, 1);
                    if (ddd > 15) {
                        nodes[j][i].x = x + i;
                        nodes[j][i].y = y + j;
                        n = &nodes[j][i];
                        n->type = NODE_VOLCANO;
                        n->blocksight = false;
                        n->canpass = false;
                        n->icon = '.';
                    }
                }
            }
        } else if (dd > 20) {
            for (j=0; j<CHUNKHEIGHT; j++) {
                for (i=0; i<CHUNKWIDTH; i++) {
                    int ddd = dice(20, 1);
                    if (ddd > 10) {
                        nodes[j][i].x = x + i;
                        nodes[j][i].y = y + j;
                        n = &nodes[j][i];
                        n->type = NODE_TREE;
                        n->blocksight = true;
                        n->canpass = false;
                        n->icon = '7';
                    }
               }
            }
        } else if (dd > 10) {
            for (j=0; j<CHUNKHEIGHT; j++) {
                for (i=0; i<CHUNKWIDTH; i++) {
                    int ddd = dice(20, 1);
                    if (ddd > 15) {
                        nodes[j][i].x = x + i;
                        nodes[j][i].y = y + j;
                        n = &nodes[j][i];
                        n->type = NODE_ROCK;
                        n->blocksight = true;
                        n->canpass = false;
                        n->icon = 'M';
                    }
               }
            }
        } else {
            for (j=0; j<CHUNKHEIGHT; j++) {
                for (i=0; i<CHUNKWIDTH; i++) {
                    int ddd = dice(20, 1);
                    if (ddd > 10) {
                        nodes[j][i].x = x + i;
                        nodes[j][i].y = y + j;
                        n = &nodes[j][i];
                        n->type = NODE_WATER;
                        n->blocksight = false;
                        n->canpass = false;
                        n->icon = '~';
                    }
               }
            }
        }
    }
}


Map::Chunk::Chunk(Map* mp, ChunkId _id)
    : id(_id)
{
    // MakeRandomNode();
    // return;
    int i, j;

    // Debug() << "Gen Chunk: (" << id.x << ", " << id.y << ")";
    int  x = CHUNKWIDTH * id.x;
    int  y = CHUNKHEIGHT * id.y;

    for (j=0; j<CHUNKHEIGHT; j++) {
        for (i=0; i<CHUNKWIDTH; i++) {
            nodes[j][i].x = x + i;
            nodes[j][i].y = y + j;
            Map::Terrain * ter = mp->NearestTerrain(nodes[j][i].x, nodes[j][i].y);
            if (ter) {
                // int d = dice(100, 1);
                // int redu = dice(100, 1);
                int dist = ter->Distance(nodes[j][i].x, nodes[j][i].y);
                bool draw = false;
                if (dist <= ter->Radius()) {
                    draw = true;
                }
                if (draw) {
                    nodes[j][i].type = NODE_GROUND;
                    nodes[j][i].blocksight = false;
                    nodes[j][i].canpass = true;
                    nodes[j][i].icon = '.';

                    switch (ter->type) {
                    case TERRAIN_LAND:
                    case TERRAIN_ISLAND:
                    case TERRAIN_CAVE:
                        nodes[j][i].type = NODE_GROUND;
                        nodes[j][i].blocksight = false;
                        nodes[j][i].canpass = true;
                        nodes[j][i].icon = '.';
                        break;
                    case TERRAIN_MOUNTAIN:
                        if (dice(200, 1) + dist * 100 / ter->Radius() < 120) {
                            nodes[j][i].type = NODE_ROCK;
                            nodes[j][i].blocksight = true;
                            nodes[j][i].canpass = false;
                            nodes[j][i].icon = 'M';
                        }
                        break;
                    case TERRAIN_FOREST:
                        if (dice(200, 1) + dist * 100 / ter->Radius() < 120) {
                            nodes[j][i].type = NODE_TREE;
                            nodes[j][i].blocksight = true;
                            nodes[j][i].canpass = false;
                            nodes[j][i].icon = '7';
                        }
                        break;
                    case TERRAIN_RIVER:
                    case TERRAIN_LAKE:
                    case TERRAIN_OCEAN:
                        nodes[j][i].type = NODE_WATER;
                        nodes[j][i].blocksight = false;
                        nodes[j][i].canpass = false;
                        nodes[j][i].icon = '~';
                        break;
                    case TERRAIN_MAX:
                    case TERRAIN_RANDOM:
                    default:
                        MakeRandomNode();
                        break;
                    };
                } else { // if (draw)
                    MakeRandomNode();
                }
            } else {// if (!ter)
                MakeRandomNode();
            }
        }
    }
}

Map::Chunk::~Chunk()
{
}

Map::Terrain::Terrain(int x, int y, TerrainType t)
{
    if (t == TERRAIN_RANDOM) {
        t = (TerrainType)(dice((int)TERRAIN_MAX - 1, 1));
    }
    type = t;
    const int range = TERRAINRANGE * CHUNKWIDTH;
    // if (type == TERRAIN_RIVER)
    //    type = TERRAIN_LAKE;
    if (type == TERRAIN_RIVER) {
        int i;
        for (i=0; i<4; i++) {
            int x1 = dice(range * 2, 1) - range + x;
            int y1 = dice(range * 2, 1) - range + y;
            b.set(i, mkpoint(x1, y1));
            // Debug() << "Gen Terrain Bezier: (" <<  x1 << ", " << y1 << ")";
        }
    } else {
        int x1 = dice(range * 2, 1) - range + x;
        int y1 = dice(range * 2, 1) - range + y;
        b.set(-1, mkpoint(x1, y1));
        // Debug() << "Gen Terrain: (" <<  x1 << ", " << y1 << ")";
    }

    if (type == TERRAIN_OCEAN) {
        radius = range * 5 + dice(range * 10, 1);
    } else {
        radius = dice(range, 1);
    }
    radius = dice(range, 1);
    if (radius < CHUNKWIDTH)
        radius = CHUNKWIDTH;
    // Debug() << "radius: " << radius;
}

Map::Terrain::~Terrain()
{
}

int Map::Terrain::Distance(int x, int y)
{
    if ((b.P0 == b.P1) && (b.P0 == b.P2) && (b.P0 == b.P3)) {
        int dist = (x - b.P0.x) * (x - b.P0.x) + (y - b.P0.y) * (y - b.P0.y);
        return dist;
    } else {
        int dist = -1;
        float t;
        for (t=0; t<=1.0f; t+= 0.2f) {
            DPoint<int> p = b.PointOnCubicBezier(t); 
            int d = (x - p.x) * (x - p.x) + (y - p.y) * (y - p.y);
            if (dist == -1)
                dist = d;
            if (d < dist)
                dist = d;
        }
        return dist;
    }
    return 0;
}










