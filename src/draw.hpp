#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <algorithm>
#include <cstring>
#include "vectors.hpp"
#include "field.hpp"
#include "error.hpp"

void gLoadImage( const char * filename, GLuint & texture );

class gSphere {
  public:
    gSphere( const gSphere & sphere );
    gSphere( std::size_t UResolution, std::size_t VResolution );
    void draw( float radius = 1.0f, const vec3d & pos = {0, 0, 0} );
    ~gSphere();

  private:
    const uint8_t vertexCount = 4;
    const uint8_t coordsCount = 3;
    void insertVec3d( std::size_t index, vec3d v );
    std::size_t maxCount;
    float * vertex;
};
