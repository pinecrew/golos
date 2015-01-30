#pragma once
#include <SDL2/SDL.h>
#include <vector>

struct vec3s {
    float r, theta, phi;
    float operator*(vec3s v);
};

struct vec3d {
    float x, y, z;
    vec3d(vec3s v);
    float operator*(vec3d v);
};


struct cell {
    int i, j;
};

struct field {
    std::size_t width, height;
    std::vector< std::vector< bool > > f;
    std::vector< bool > operator[](std::size_t i);
    field(std::size_t h, std::size_t w);
};

SDL_Point surf_to_screen( vec3s n, vec3s sp, SDL_Point center );
bool visible( vec3s n, vec3s v );
