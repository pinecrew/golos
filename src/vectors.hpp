#pragma once
#include <cmath>

struct vec3s {
    float r, theta, phi;
    float operator*( const vec3s & v );
    void rotate( float dtheta, float dphi );
    friend vec3s & operator += ( vec3s & lhs, const vec3s & rhs );
    friend vec3s & operator -= ( vec3s & lhs, const vec3s & rhs );
};

struct vec3d {
    float x, y, z;
    vec3d( const vec3s & v);
    float operator*( const vec3d & v );
};

inline float _fmod( float x, float y ) { return fmod( fmod( x, y ) + y, y ); }