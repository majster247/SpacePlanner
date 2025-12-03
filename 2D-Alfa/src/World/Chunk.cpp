#include "Chunk.h"

Chunk::Chunk(int x, int y) : cx(x), cy(y) {
    tiles.resize(CHUNK_SIZE * CHUNK_SIZE);
    heights.resize(CHUNK_SIZE * CHUNK_SIZE);
}
