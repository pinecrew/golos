#pragma once

#include "math.hpp"

struct vec3s {
    float r, theta, phi;
    float operator*( const vec3s & v );
    void rotate( float dtheta, float dphi );
    friend vec3s & operator+=( vec3s & lhs, const vec3s & rhs );
    friend vec3s & operator-=( vec3s & lhs, const vec3s & rhs );
};

struct vec3d {
    float x, y, z;
    vec3d( const vec3s & v );
    vec3d( const float _x, const float _y, const float _z, bool polar = false );
    float operator*( const vec3d & v );
};