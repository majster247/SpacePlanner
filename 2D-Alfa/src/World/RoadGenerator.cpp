#include "RoadGenerator.h"

RoadGenerator::RoadGenerator(int s) : seed(s) {}

void RoadGenerator::applyRoads(Chunk& c) {
    // Prosty przykład: horyzontalna droga
    if(c.cy == 0) {
        for(int x=0; x<CHUNK_SIZE; x++) {
            int idx = (CHUNK_SIZE/2) * CHUNK_SIZE + x;
            c.tiles[idx] = TileType::Road;
        }
    }
}
