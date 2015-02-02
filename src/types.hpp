#pragma once
#include <SDL2/SDL.h>
#include <vector>

struct vec3s {
    float r, theta, phi;
    float operator*(vec3s v);
    friend vec3s & operator += ( vec3s & lhs, const vec3s & rhs );
    friend vec3s & operator -= ( vec3s & lhs, const vec3s & rhs );
};

struct vec3d {
    float x, y, z;
    vec3d(vec3s v);
    float operator*(vec3d v);
};


struct cell {
    int i, j;
    cell( int _i, int _j ) : i( _i ), j( _j ) {};
};

class field {
    private:
        std::vector< std::vector< bool > > f;
    public:
        std::size_t width, height;
        std::vector< bool > & operator[](std::size_t i);
        field(std::size_t h, std::size_t w);
};

SDL_Point surf_to_screen( vec3s n, vec3s sp, SDL_Point center );
bool visible( vec3s n, vec3s v );
