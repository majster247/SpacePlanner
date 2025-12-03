#pragma once
#include <vector>
#include "../Render/TileTypes.h"

constexpr int CHUNK_SIZE = 32;

struct Chunk {
    int cx, cy;              // pozycja chunków
    bool generated = false;

    std::vector<TileType> tiles;
    std::vector<float> heights;

    Chunk(int x, int y);
};
