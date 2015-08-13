#pragma once
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include "field.hpp"

class gSphere {
public:
    gSphere( float radius, size_t UResolution, size_t VResolution );
    void draw();
    ~gSphere();
private:
    void insert_vec3d( size_t index, vec3d v );
    size_t max_count;
    float * vertex;
};

vec3s screen_to_field( SDL_Point p, vec3s n, SDL_Point c, field & f );
SDL_Point field_to_screen( vec3s sp, vec3s n, SDL_Point center );