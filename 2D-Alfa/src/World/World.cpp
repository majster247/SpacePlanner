#include "World.h"

World::World(int s)
: seed(s), generator(s), roads(s) {}

void World::init() {
    // generujemy pierwszy chunk (0,0)
    getChunk(0,0);
}

void World::update() {
    // prosty loader — zawsze generujemy sąsiednie chunky 3x3
    for(int cy=-1; cy<=1; cy++)
        for(int cx=-1; cx<=1; cx++)
            getChunk(cx, cy);
}

long long World::key(int cx, int cy) {
    return ( (long long)cx << 32 ) | (unsigned long long)cy;
}

Chunk* World::getChunk(int cx, int cy) {
    long long k = key(cx,cy);
    if(chunkMap.count(k)) return chunkMap[k];

    Chunk* c = new Chunk(cx,cy);
    generator.generateChunk(*c);
    roads.applyRoads(*c);

    chunkMap[k] = c;
    return c;
}
