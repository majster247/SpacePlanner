#pragma once
#include <vector>
#include "Chunk.h"
#include "../../extern/FastNoiseLite.h"

class MapGenerator {
public:
    MapGenerator(int seed);
    void generateChunk(Chunk& chunk);

private:
    FastNoiseLite noiseHeight;
    FastNoiseLite noiseBiome;
};
