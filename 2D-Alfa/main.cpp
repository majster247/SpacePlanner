#include "perlin.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <cstdint>

using namespace std;

// funkcja standaryzująca seed do 64-bit jak w Minecraft
uint64_t standardize_seed(int64_t s){
    uint64_t seed = static_cast<uint64_t>(s);
    return seed;
}

int main(int argc,char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(argc<2){
        cerr<<"Usage: "<<argv[0]<<" <mode> [params...]\nModes: ascii, ppm, obj, json\n";
        return 1;
    }

    string mode=argv[1];

    if(mode=="ascii"){
        if(argc<9){ cerr<<"ascii usage: ascii w h [seed] octaves persistence lacunarity scale_cont scale_local\n"; return 1; }

        int argOffset = 2;
        int w = atoi(argv[argOffset++]);
        int h = atoi(argv[argOffset++]);

        uint64_t seed64;
        int oct;
        double pers, lac, scale_cont, scale_local;

        if(argc == 10){ // seed podany
            seed64 = standardize_seed(stoll(argv[argOffset++]));
        } else { // brak seedu
            seed64 = standardize_seed(time(nullptr)); // losowy seed
        }

        oct = atoi(argv[argOffset++]);
        pers = atof(argv[argOffset++]);
        lac = atof(argv[argOffset++]);
        scale_cont = atof(argv[argOffset++]);
        scale_local = atof(argv[argOffset++]);

        Perlin per(seed64);
        vector<double> map(w*h);
        for(int y=0;y<h;y++)
            for(int x=0;x<w;x++){
                double continent=per.fractal(x*scale_cont,y*scale_cont,4,0.5,2.0);
                double local=per.fractal(x*scale_local,y*scale_local,oct,pers,lac);
                double ridge=1.0-std::abs(local); ridge*=ridge;
                map[y*w+x]=0.6*continent+0.3*ridge+0.1*local;
            }
        print_ascii(map,w,h);
        return 0;
    }

    if(mode=="ppm" || mode=="obj"){
        if(argc<9){ cerr<<mode<<" usage: "<<mode<<" w h [seed] octaves persistence lacunarity scale_cont scale_local [out."<<mode<<"]\n"; return 1; }

        int argOffset = 2;
        int w = atoi(argv[argOffset++]);
        int h = atoi(argv[argOffset++]);

        uint64_t seed64;
        int oct;
        double pers, lac, scale_cont, scale_local;

        if(argc >= 10) { // seed podany
            seed64 = standardize_seed(stoll(argv[argOffset++]));
        } else {
            seed64 = standardize_seed(time(nullptr));
        }

        oct = atoi(argv[argOffset++]);
        pers = atof(argv[argOffset++]);
        lac = atof(argv[argOffset++]);
        scale_cont = atof(argv[argOffset++]);
        scale_local = atof(argv[argOffset++]);

        string out;
        if(argc > argOffset){
            out = argv[argOffset++];
        } else {
            out = to_string(seed64) + "." + mode; // automatyczna nazwa pliku
        }

        Perlin per(seed64);

        if((long long)w*h>100000000){
            if(mode=="ppm") write_ppm_chunked(out,w,h,per,oct,pers,lac,scale_cont,scale_local);
            else cerr<<"Chunked OBJ niezaimplementowane.\n";
        } else {
            vector<double> map(w*h);
            for(int y=0;y<h;y++)
                for(int x=0;x<w;x++){
                    double continent=per.fractal(x*scale_cont,y*scale_cont,4,0.5,2.0);
                    double local=per.fractal(x*scale_local,y*scale_local,oct,pers,lac);
                    double ridge=1.0-std::abs(local); ridge*=ridge;
                    map[y*w+x]=0.6*continent+0.3*ridge+0.1*local;
                }
            if(mode=="ppm") write_ppm(out,map,w,h);
            else write_obj(out,map,w,h);
        }
        return 0;
    }

    if(mode=="json"){
        if(argc<9){ cerr<<"json usage: json w h [seed] octaves persistence lacunarity scale_cont scale_local [out.json]\n"; return 1; }

        int argOffset = 2;
        int w = atoi(argv[argOffset++]);
        int h = atoi(argv[argOffset++]);

        uint64_t seed64;
        int oct;
        double pers, lac, scale_cont, scale_local;

        if(argc >= 10) { // seed podany
            seed64 = standardize_seed(stoll(argv[argOffset++]));
        } else {
            seed64 = standardize_seed(time(nullptr));
        }

        oct = atoi(argv[argOffset++]);
        pers = atof(argv[argOffset++]);
        lac = atof(argv[argOffset++]);
        scale_cont = atof(argv[argOffset++]);
        scale_local = atof(argv[argOffset++]);

        string out;
        if(argc > argOffset){
            out = argv[argOffset++];
        } else {
            out = to_string(seed64) + ".json";
        }

        Perlin per(seed64);

        if((long long)w*h>100000000){ cerr<<"Chunked JSON niezaimplementowane.\n"; return 1; }

        vector<double> map(w*h);
        for(int y=0;y<h;y++)
            for(int x=0;x<w;x++){
                double continent=per.fractal(x*scale_cont,y*scale_cont,4,0.5,2.0);
                double local=per.fractal(x*scale_local,y*scale_local,oct,pers,lac);
                double ridge=1.0-std::abs(local); ridge*=ridge;
                map[y*w+x]=0.6*continent+0.3*ridge+0.1*local;
            }
        write_json(out,map,w,h);
        return 0;
    }

    cerr<<"Unknown mode: "<<mode<<"\n";
    return 1;
}
