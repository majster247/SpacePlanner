#include "perlin.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ---------------- Perlin ----------------
Perlin::Perlin(unsigned seed){
    p.resize(256);
    std::iota(p.begin(),p.end(),0);
    std::mt19937 g(seed);
    std::shuffle(p.begin(),p.end(),g);
    p.insert(p.end(),p.begin(),p.end());
}

double Perlin::fade(double t){ return t*t*t*(t*(t*6-15)+10); }
double Perlin::lerp(double a,double b,double t){ return a + t*(b-a); }
double Perlin::grad(int hash,double x,double y){
    int h = hash&3;
    double u = h<2?x:y;
    double v = h<2?y:x;
    return ((h&1)?-u:u)+((h&2)?-v:v);
}

double Perlin::noise(double x,double y) const{
    int X=(int)floor(x)&255;
    int Y=(int)floor(y)&255;
    x -= floor(x); y -= floor(y);
    double u=fade(x), v=fade(y);
    int A=p[X]+Y, B=p[X+1]+Y;
    return lerp(
        lerp(grad(p[A],x,y), grad(p[B],x-1,y), u),
        lerp(grad(p[A+1],x,y-1), grad(p[B+1],x-1,y-1), u),
        v
    );
}

double Perlin::fractal(double x,double y,int octaves,double persistence,double lacunarity) const{
    double amplitude=1.0, frequency=1.0, maxAmp=0.0, sum=0.0;
    for(int i=0;i<octaves;i++){
        sum += noise(x*frequency, y*frequency)*amplitude;
        maxAmp += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return maxAmp!=0.0 ? sum/maxAmp : 0.0;
}

// ---------------- Biomy ----------------
Biome getBiome(double height,double humidity,double temperature){
    if(height<0.63) return {"ocean",0,0,128};
    if(height<0.65) return {"beach",238,214,175};
    if(height<0.73){
        if(temperature>0.7){
            if(humidity<0.3) return {"desert",210,180,140};
            return {"savanna",189,183,107};
        }
        if(humidity>0.7) return {"rainforest",0,100,0};
        return {"plains",124,252,0};
    }
    if(height<0.80){
        if(humidity<0.5) return {"forest",34,139,34};
        return {"taiga",0,128,0};
    }
    if(height<0.90) return {"mountain",139,137,137};
    return {"snow",255,250,250};
}

// ---------------- ASCII ----------------
void print_ascii(const std::vector<double>& map,int w,int h){
    static const char* asciiRamp=" .:-=+*#%@";
    int rampLen = strlen(asciiRamp);
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            double v=(map[y*w+x]+1)/2;
            int idx = std::min(rampLen-1,std::max(0,(int)floor(v*(rampLen-1))));
            putchar(asciiRamp[idx]);
        }
        putchar('\n');
    }
}

// ---------------- PPM ----------------
void write_ppm(const std::string &filename,const std::vector<double>& map,int w,int h){
    FILE *f = fopen(filename.c_str(),"wb");
    if(!f){ perror("fopen"); return; }
    fprintf(f,"P6\n%d %d\n255\n",w,h);
    for(int i=0;i<w*h;i++){
        double hval=(map[i]+1)/2;
        Biome b = getBiome(hval,0.5,0.5);
        fputc(b.r,f); fputc(b.g,f); fputc(b.b,f);
    }
    fclose(f);
    std::cerr<<"Saved PPM: "<<filename<<"\n";
}

// ---------------- Chunked PPM ----------------
void write_ppm_chunked(const std::string &filename,int w,int h,Perlin &per,int octaves,
                       double persistence,double lacunarity,double scale_continent,double scale_local)
{
    FILE *f=fopen(filename.c_str(),"wb");
    if(!f){ perror("fopen"); return; }
    fprintf(f,"P6\n%d %d\n255\n",w,h);

    int chunkSize=512;
    int totalChunksX=(w+chunkSize-1)/chunkSize;
    int totalChunksY=(h+chunkSize-1)/chunkSize;
    int totalChunks=totalChunksX*totalChunksY;
    int processedChunks=0;

    std::vector<double> chunk(chunkSize*chunkSize);
    auto startTime = std::chrono::steady_clock::now();

    for(int cy=0; cy<h; cy+=chunkSize){
        for(int cx=0; cx<w; cx+=chunkSize){
            int curH = std::min(chunkSize, h-cy);
            int curW = std::min(chunkSize, w-cx);

            // Generowanie wysokości chunku
            for(int y=0; y<curH; y++){
                for(int x=0; x<curW; x++){
                    double continent = per.fractal((cx+x)*scale_continent,(cy+y)*scale_continent,4,0.5,2.0);
                    double local = per.fractal((cx+x)*scale_local,(cy+y)*scale_local,octaves,persistence,lacunarity);
                    double ridge = 1.0 - std::abs(local);
                    ridge *= ridge; // wyostrzenie gór
                    chunk[y*curW+x] = 0.6*continent + 0.3*ridge + 0.1*local;
                }
            }

            // Zapisywanie kolorów do pliku
            for(int y=0; y<curH; y++){
                for(int x=0; x<curW; x++){
                    double hval = (chunk[y*curW+x]+1)/2;
                    double humidity = per.fractal((cx+x)*scale_continent*2,(cy+y)*scale_continent*2,2,0.5,2.0);
                    double temperature = per.fractal((cx+x)*scale_continent*2+1000,(cy+y)*scale_continent*2+1000,2,0.5,2.0);
                    Biome b = getBiome(hval,humidity,temperature);
                    fputc(b.r,f); fputc(b.g,f); fputc(b.b,f);
                }
            }

            processedChunks++;

            // Pasek postępu wyświetlany co 1 chunk
            float progress = (float)processedChunks / totalChunks;
            int barWidth = 40;
            int pos = (int)(barWidth * progress);

            std::cout << "\r["; 
            for(int i=0; i<barWidth; i++)
                std::cout << (i < pos ? '=' : ' ');
            std::cout << "] " << int(progress*100.0) << "% "
                      << "(" << processedChunks << "/" << totalChunks << " chunks)";
            std::cout.flush();
        }
    }

    fclose(f);
    std::cout << "\nSaved chunked PPM: " << filename << "\n";
}

// ---------------- OBJ ----------------
void write_obj(const std::string &filename,const std::vector<double>& map,int w,int h,double heightScale){
    FILE *f=fopen(filename.c_str(),"w");
    if(!f){ perror("fopen"); return; }
    for(int y=0;y<h;y++)
        for(int x=0;x<w;x++){
            double v=(map[y*w+x]+1)/2;
            double z=v*heightScale;
            fprintf(f,"v %f %f %f\n",(double)x,z,(double)y);
        }
    for(int y=0;y<h-1;y++)
        for(int x=0;x<w-1;x++){
            int i=y*w+x+1,iR=i+1,iD=i+w,iDR=iD+1;
            fprintf(f,"f %d %d %d\n",i,iD,iDR);
            fprintf(f,"f %d %d %d\n",i,iDR,iR);
        }
    fclose(f);
    std::cerr<<"Saved OBJ: "<<filename<<"\n";
}

// ---------------- JSON ----------------
void write_json(const std::string &filename,const std::vector<double>& map,int w,int h){
    json j;
    j["width"]=w; j["height"]=h;
    j["map"]=json::array();
    for(int y=0;y<h;y++)
        for(int x=0;x<w;x++){
            double hval=(map[y*w+x]+1)/2;
            Biome b=getBiome(hval,0.5,0.5);
            j["map"].push_back({{"x",x},{"y",y},{"height",hval},{"biome",b.name},{"r",b.r},{"g",b.g},{"b",b.b}});
        }
    std::ofstream ofs(filename);
    ofs<<j.dump(2);
    ofs.close();
}

// ---------------- Normalizacja ----------------
void normalize_map(std::vector<double>& map){
    if(map.empty()) return;
    double minV=*std::min_element(map.begin(),map.end());
    double maxV=*std::max_element(map.begin(),map.end());
    if(maxV==minV) return;
    for(auto &v: map) v=(v-minV)/(maxV-minV)*2-1;
}
