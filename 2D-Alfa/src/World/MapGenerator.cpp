#include "MapGenerator.h"

MapGenerator::MapGenerator(int seed) {
    noiseHeight.SetSeed(seed);
    noiseHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noiseHeight.SetFrequency(0.002f);

    noiseBiome.SetSeed(seed + 1000);
    noiseBiome.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noiseBiome.SetFrequency(0.001f);
}

void MapGenerator::generateChunk(Chunk& c) {
    for(int y=0; y<CHUNK_SIZE; y++) {
        for(int x=0; x<CHUNK_SIZE; x++) {

            int gx = c.cx * CHUNK_SIZE + x;
            int gy = c.cy * CHUNK_SIZE + y;

            float h = (noiseHeight.GetNoise((float)gx, (float)gy) + 1.0f) * 0.5f;
            float b = noiseBiome.GetNoise(gx, gy);

            TileType type;

            if(h < 0.3f) type = TileType::Water;
            else if(h < 0.33f) type = TileType::Sand;
            else if(b > 0.25f) type = TileType::Forest;
            else type = TileType::Grass;

            int idx = y * CHUNK_SIZE + x;
            c.tiles[idx] = type;
            c.heights[idx] = h;
        }
    }
    c.generated = true;
}
