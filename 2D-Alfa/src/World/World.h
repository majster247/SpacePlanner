#pragma once
#include <map>
#include "Chunk.h"
#include "MapGenerator.h"
#include "RoadGenerator.h"

class World {
public:
    World(int seed);
    void init();
    void update();

    Chunk* getChunk(int cx, int cy);
    const std::map<long long, Chunk*>& chunks() const { return chunkMap; }

private:
    int seed;
    MapGenerator generator;
    RoadGenerator roads;

    std::map<long long, Chunk*> chunkMap;

    long long key(int cx, int cy);
};
