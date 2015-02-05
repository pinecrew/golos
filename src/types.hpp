#pragma once
#include <SDL2/SDL.h>
#include <vector>

struct vec3s {
    float r, theta, phi;
    float operator*( const vec3s & v );
    friend vec3s & operator += ( vec3s & lhs, const vec3s & rhs );
    friend vec3s & operator -= ( vec3s & lhs, const vec3s & rhs );
};

struct vec3d {
    float x, y, z;
    vec3d( const vec3s & v);
    float operator*( const vec3d & v );
};

class field {
    private:
        std::vector< std::vector< bool > > f;
    public:
        std::size_t width, height;
        std::vector< bool > & operator[](std::size_t i);
        field(std::size_t h, std::size_t w);
};

struct cell {
    int i, j;
    vec3s n;
    cell( field & f, int _i, int _j );
};

SDL_Point surf_to_screen( vec3s n, vec3s sp, SDL_Point center );
vec3s screen_to_surf( vec3s n, float r, SDL_Point p, SDL_Point c );
bool visible( vec3s n, vec3s v );
