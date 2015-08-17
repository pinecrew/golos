#pragma once
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <algorithm>
#include <cstring>
#include "field.hpp"

class gSphere {
public:
    gSphere( const gSphere & sphere );
    gSphere( float radius, std::size_t UResolution, std::size_t VResolution );
    void draw();
    ~gSphere();
private:
    const uint8_t vertex_count = 4;
    const uint8_t coords_count = 3;
    void insert_vec3d( std::size_t index, vec3d v );
    std::size_t max_count;
    float * vertex;
};

//vec3s screen_to_field( );