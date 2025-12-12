#pragma once
#include <vector>
#include <string>
#include <cstdio>

// Perlin noise generator
struct Perlin {
    std::vector<int> p;
    Perlin(unsigned seed = 0);

    double noise(double x,double y) const;
    double fractal(double x,double y,int octaves,double persistence,double lacunarity) const;

private:
    static double fade(double t);
    static double lerp(double a,double b,double t);
    static double grad(int hash,double x,double y);
};

// Biome struct
struct Biome {
    std::string name;
    unsigned char r,g,b;
};

// Biomy
Biome getBiome(double height,double humidity,double temperature);

// ASCII
void print_ascii(const std::vector<double>& map,int w,int h);

// PPM
void write_ppm(const std::string &filename,const std::vector<double>& map,int w,int h);
void write_ppm_chunked(const std::string &filename,int w,int h,Perlin &per,int octaves,
                       double persistence,double lacunarity,double scale_continent,double scale_local);

// OBJ
void write_obj(const std::string &filename,const std::vector<double>& map,int w,int h,double heightScale=20.0);

// JSON
void write_json(const std::string &filename,const std::vector<double>& map,int w,int h);

// Normalizacja mapy wysokości
void normalize_map(std::vector<double>& map);
