#pragma once

#include <cmath>

// windows MinGW fix
#ifdef __MINGW32__
#ifndef M_PI
const double M_PI = 3.14159265358979323846264338327950288;
#endif
#endif

const double M_2PI = M_PI * 2.0;

inline float _fmod( float x, float y ) { return fmod( fmod( x, y ) + y, y ); }