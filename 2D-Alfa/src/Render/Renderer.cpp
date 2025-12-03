#include "Renderer.h"

void Renderer::init(SDL_Renderer* r) {
    renderer = r;
    atlas.load(r, "assets/atlas.png");
}

void Renderer::renderWorld(World& world) {
    for(auto& p : world.chunks()) {
        Chunk* c = p.second;
        if(!c->generated) continue;

        for(int y=0; y<CHUNK_SIZE; y++) {
            for(int x=0; x<CHUNK_SIZE; x++) {
                TileType t = c->tiles[y*CHUNK_SIZE+x];
                int sx = (c->cx * CHUNK_SIZE + x) * 16;
                int sy = (c->cy * CHUNK_SIZE + y) * 16;

                atlas.drawTile(renderer, t, sx, sy);
            }
        }
    }
}
