
#pragma once

#include <string>
#include <sstream>
#include <map>
#include <list>
#include "item.h"
#include "shape.h"

#define CHUNKWIDTH  10
#define CHUNKHEIGHT CHUNKWIDTH
#define TERRAINRANGE   5 

class Creature;
class Map {
public:
    typedef enum {
        NODE_GROUND = 0,
        NODE_ROCK,
        NODE_TREE,
        NODE_WATER,
        NODE_VOLCANO,
        NODE_MAX
    } NODETYPE;

    typedef enum {
        TERRAIN_RANDOM = 0,
        TERRAIN_LAND,
        TERRAIN_MOUNTAIN,
        TERRAIN_FOREST,
        TERRAIN_RIVER,
        TERRAIN_LAKE,
        TERRAIN_OCEAN,
        TERRAIN_ISLAND,
        TERRAIN_CAVE,
        TERRAIN_MAX
    } TerrainType;

    class Node {
    public:
        Node() : type(NODE_GROUND), icon('.'), blocksight(false), canpass(true), x(0), y(0), creature(NULL) {}
        ~Node() {}
        NODETYPE type;
        char icon;
        bool blocksight;
        bool canpass;
        int x;
        int y;
        Creature *  creature;
        std::list<Item*> items;
        std::string desc() {
            std::stringstream   ss;
            ss << "(" << x << ", " << y << ")";
            return ss.str();
        }
    };

    class ChunkId {
    public:
        ChunkId(int _x, int _y) : x(_x), y(_y) {}
        ~ChunkId() {}
        ChunkId(const ChunkId& o) {
            x = o.x;
            y = o.y;
        }
        ChunkId& operator=(const ChunkId& o) {
            x = o.x;
            y = o.y;
            return *this;
        }
        bool operator<(const ChunkId& o) const {
            if (x == o.x)
                return (y < o.y);
            return (x < o.x);
        }

        int x;
        int y;
    };

    class Chunk {
    public:
        Chunk(Map* mp, ChunkId _id);
        ~Chunk();
        Node   nodes[CHUNKHEIGHT][CHUNKWIDTH];
        ChunkId id;
    private:
        void MakeRandomNode(void);
    };

    class Terrain {
    public:
        Terrain(int x, int y, TerrainType t = TERRAIN_RANDOM);
        ~Terrain();
        // random Bezier curve.
        DBezier<int> b;
        TerrainType type;
        int     Distance(int x, int y);
        int     Radius(void) { return radius * radius; }
    private:
        int     radius;
    };

    ~Map();
    Node* uiat(int x, int y);
    Node* at(int x, int y, bool gen = true);

    int HeroX(void) { return herox; }
    int HeroY(void) { return heroy; }

    bool IsNodeCanBeSeen(Node* n, Node* from);
    bool IsNodeCanBeSeenByHero(Node* n);

    void CenterMap(int x, int y);
    static Map* Get(void);
private:
    Map();
public:
    std::map<ChunkId, Chunk*>   chunks;
    std::list<Terrain*>         ters;

    int  herox;
    int  heroy;

    ChunkId GetCurrentChunkId(void);
    ChunkId GetChunkIdFromPos(int x, int y);
    Chunk* GetChunkFromId(ChunkId id);

    void GenChunks(void);
    Chunk* GenChunk(ChunkId id);
    ChunkId GenChunkId(int x, int y);
    int GetXFromChunkId(ChunkId id);
    int GetYFromChunkId(ChunkId id);
    int GetMapXFromChunkId(ChunkId id);
    int GetMapYFromChunkId(ChunkId id);
    bool IsChunkGenerated(int x, int y);
    bool IsChunkGenerated(ChunkId id);


    Terrain* NearestTerrain(int x, int y);
};

