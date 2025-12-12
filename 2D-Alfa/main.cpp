#include "perlin.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(argc < 2) {
        cerr << "Usage: " << argv[0] << " <mode> [params...]\nModes: ascii, ppm, obj, json\n";
        return 1;
    }

    string mode = argv[1];

    if(mode == "ascii") {
        if(argc < 8) { cerr << "ascii usage: ascii w h seed octaves persistence lacunarity\n"; return 1; }

        int w = atoi(argv[2]);
        int h = atoi(argv[3]);
        unsigned seed = (unsigned)stoul(argv[4]);
        int oct = atoi(argv[5]);
        double pers = atof(argv[6]);
        double lac = atof(argv[7]);

        Perlin per(seed);
        vector<double> map(w*h);
        double scale = 0.005;

        for(int y=0; y<h; y++)
            for(int x=0; x<w; x++)
                map[y*w+x] = per.fractal(x*scale, y*scale, oct, pers, lac);

        print_ascii(map, w, h);
        return 0;
    }

    // ppm / obj
    if(mode == "ppm" || mode == "obj") {
        if(argc < 9) { 
            cerr << mode << " usage: " << mode << " w h seed octaves persistence lacunarity out." << mode << "\n"; 
            return 1; 
        }

        int w = atoi(argv[2]);
        int h = atoi(argv[3]);
        unsigned seed = (unsigned)stoul(argv[4]);
        int oct = atoi(argv[5]);
        double pers = atof(argv[6]);
        double lac = atof(argv[7]);
        string out = argv[8];

        Perlin per(seed);
        double scale = 0.004;

        // jesli gigantyczna mapa, uzyj chunkow
        if((long long)w * h > 100000000) { // np >100 mln pikseli
            if(mode == "ppm") write_ppm_chunked(out, w, h, per, oct, pers, lac, scale);
            else cerr << "Chunked OBJ niezaimplementowane - uzyj mniejszej rozdzielczosci.\n";
        } else {
            vector<double> map(w*h);
            for(int y=0; y<h; y++)
                for(int x=0; x<w; x++)
                    map[y*w+x] = per.fractal(x*scale, y*scale, oct, pers, lac);

            if(mode == "ppm") write_ppm(out, map, w, h);
            else write_obj(out, map, w, h);
        }
        return 0;
    }

    // json
    if(mode == "json") {
        if(argc < 9) { cerr << "json usage: json w h seed octaves persistence lacunarity out.json\n"; return 1; }

        int w = atoi(argv[2]);
        int h = atoi(argv[3]);
        unsigned seed = (unsigned)stoul(argv[4]);
        int oct = atoi(argv[5]);
        double pers = atof(argv[6]);
        double lac = atof(argv[7]);
        string out = argv[8];

        Perlin per(seed);
        double scale = 0.004;

        if((long long)w * h > 100000000) {
            std::cerr << "Chunked JSON niezaimplementowane, uzyj mniejszej rozdzielczosci.\n";
            return 1;
        } else {
            vector<double> map(w*h);
            for(int y=0; y<h; y++)
                for(int x=0; x<w; x++)
                    map[y*w+x] = per.fractal(x*scale, y*scale, oct, pers, lac);

            write_json(out, map, w, h);
        }
        return 0;
    }

    cerr << "Unknown mode: " << mode << "\n";
    return 1;
}
