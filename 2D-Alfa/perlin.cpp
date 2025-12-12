#include "perlin.h"
#include <iostream>
#include <vector>
#include <algorithm>  // shuffle, iota, min, max
#include <random>     // mt19937
#include <cmath>      // floor, pow
#include <cstdio>     // FILE*, fopen, fprintf, fputc, fclose, perror
#include <fstream>    // ofstream
#include <string>     // std::string
#include <cstring>    // strlen

#include <nlohmann/json.hpp>


using json = nlohmann::json;

Perlin::Perlin(unsigned seed) {
    p.resize(256);
    iota(p.begin(), p.end(), 0);
    std::mt19937 g(seed);
    shuffle(p.begin(), p.end(), g);
    p.insert(p.end(), p.begin(), p.end());
}

double Perlin::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double Perlin::lerp(double a, double b, double t) {
    return a + t * (b - a);
}

double Perlin::grad(int hash, double x, double y) {
    int h = hash & 3;
    double u = h < 2 ? x : y;
    double v = h < 2 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

double Perlin::noise(double x, double y) const {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floor(x);
    y -= floor(y);
    double u = fade(x);
    double v = fade(y);

    int A = p[X] + Y;
    int B = p[X+1] + Y;

    return lerp(
        lerp(grad(p[A], x, y), grad(p[B], x-1, y), u),
        lerp(grad(p[A+1], x, y-1), grad(p[B+1], x-1, y-1), u),
        v
    );
}

double Perlin::fractal(double x, double y, int octaves, double persistence, double lacunarity) const {
    double amplitude = 1.0;
    double frequency = 1.0;
    double maxAmp = 0.0;
    double sum = 0.0;
    for(int i=0;i<octaves;i++){
        sum += noise(x*frequency, y*frequency) * amplitude;
        maxAmp += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return maxAmp != 0.0 ? sum/maxAmp : 0.0;
}

// Biomy
Biome getBiome(double h) {
    if(h < 0.3) return {"ocean",0,0,128};
    if(h < 0.35) return {"beach",238,214,175};
    if(h < 0.6) return {"plains",124,252,0};
    if(h < 0.75) return {"forest",34,139,34};
    if(h < 0.85) return {"mountain",139,137,137};
    return {"snow",255,250,250};
}

// ASCII
void print_ascii(const std::vector<double>& map, int w, int h){
    static const char* asciiRamp = " .:-=+*#%@";
    int rampLen = strlen(asciiRamp);
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            double v = (map[y*w+x]+1)/2;
            int idx = std::min(rampLen-1,std::max(0,(int)floor(v*(rampLen-1))));
            putchar(asciiRamp[idx]);
        }
        putchar('\n');
    }
}

// PPM
void write_ppm(const std::string &filename, const std::vector<double>& map, int w, int h){
    FILE *f = fopen(filename.c_str(),"wb");
    if(!f){ perror("fopen"); return; }
    fprintf(f,"P6\n%d %d\n255\n",w,h);

    for(int i=0;i<w*h;i++){
        double hval = (map[i]+1)/2;
        Biome b = getBiome(hval);
        fputc(b.r,f);
        fputc(b.g,f);
        fputc(b.b,f);
    }
    fclose(f);
    fprintf(stderr,"Saved PPM: %s\n",filename.c_str());
}

// OBJ
void write_obj(const std::string &filename,const std::vector<double>& map,int w,int h,double heightScale){
    FILE *f=fopen(filename.c_str(),"w");
    if(!f){ perror("fopen"); return; }
    for(int y=0;y<h;y++)
        for(int x=0;x<w;x++){
            double v = (map[y*w+x]+1)/2;
            double z = v*heightScale;
            fprintf(f,"v %f %f %f\n",(double)x,z,(double)y);
        }
    for(int y=0;y<h-1;y++)
        for(int x=0;x<w-1;x++){
            int i=y*w+x+1,iR=i+1,iD=i+w,iDR=iD+1;
            fprintf(f,"f %d %d %d\n",i,iD,iDR);
            fprintf(f,"f %d %d %d\n",i,iDR,iR);
        }
    fclose(f);
    fprintf(stderr,"Saved OBJ: %s\n",filename.c_str());
}

void write_json(const std::string &filename, const std::vector<double>& map, int w, int h){
    json j;
    j["width"] = w;
    j["height"] = h;
    j["map"] = json::array();

    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            double hval = (map[y*w+x]+1)/2;
            Biome b = getBiome(hval);
            j["map"].push_back({
                {"x",x},
                {"y",y},
                {"height",hval},
                {"biome",b.name},
                {"r",b.r},
                {"g",b.g},
                {"b",b.b}
            });
        }
    }

    std::ofstream ofs(filename);
    ofs << j.dump(2);
    ofs.close();
}

void write_ppm_chunked(const std::string &filename, int w, int h, Perlin &per, int octaves, double persistence, double lacunarity, double scale = 0.004) {
    FILE *f = fopen(filename.c_str(),"wb");
    if(!f){ perror("fopen"); return; }
    fprintf(f,"P6\n%d %d\n255\n",w,h);

    int chunkSize = 512;
    std::vector<double> chunk(chunkSize*chunkSize);

    for(int cy=0; cy<h; cy+=chunkSize){
        for(int cx=0; cx<w; cx+=chunkSize){
            int curH = std::min(chunkSize, h-cy);
            int curW = std::min(chunkSize, w-cx);

            for(int y=0;y<curH;y++){
                for(int x=0;x<curW;x++){
                    double nx = (cx+x)*scale;
                    double ny = (cy+y)*scale;
                    chunk[y*curW + x] = per.fractal(nx, ny, octaves, persistence, lacunarity);
                }
            }

            // zapis RGB do pliku
            for(int y=0;y<curH;y++){
                for(int x=0;x<curW;x++){
                    double hval = (chunk[y*curW+x]+1)/2;
                    Biome b = getBiome(hval);
                    fputc(b.r,f);
                    fputc(b.g,f);
                    fputc(b.b,f);
                }
            }
        }
    }

    fclose(f);
    fprintf(stderr,"Saved chunked PPM: %s\n",filename.c_str());
}