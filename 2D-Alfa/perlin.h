#pragma once
#include <iostream>

#include <vector>
#include <string>
#include <cstdio>


struct Perlin {
    std::vector<int> p;
    Perlin(unsigned seed = 0);

    double noise(double x, double y) const;
    double fractal(double x, double y, int octaves, double persistence, double lacunarity) const;

private:
    static double fade(double t);
    static double lerp(double a, double b, double t);
    static double grad(int hash, double x, double y);
};

// Biome struct do kolorowania mapy
struct Biome {
    std::string name;
    unsigned char r, g, b;
};

Biome getBiome(double h); // zwraca biome wedlug wysokosci

void print_ascii(const std::vector<double>& map, int w, int h);
void write_ppm(const std::string &filename, const std::vector<double>& map, int w, int h);
void write_obj(const std::string &filename, const std::vector<double>& map, int w, int h, double heightScale=20.0);
void write_json(const std::string &filename, const std::vector<double>& map, int w, int h);

void write_ppm_chunked(const std::string &filename, int w, int h, Perlin &per, int octaves, double persistence, double lacunarity, double scale = 0.004)
