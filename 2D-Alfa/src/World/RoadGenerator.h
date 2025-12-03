#pragma once
#include "Chunk.h"

class RoadGenerator {
public:
    RoadGenerator(int seed);
    void applyRoads(Chunk& chunk);

private:
    int seed;
};
