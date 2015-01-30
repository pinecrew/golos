#pragma once
#include <cmath>
#include "types.hpp"

// windows MinGW fix
#ifndef M_PI
    const double M_PI   = 3.14159265358979323846;
    const double M_PI_2 = 1.57079632679489661923;
#endif
void cell_contour( cell c, field f, std::size_t npoints, vec3s* contour );
cell cell_from_point( field & f, vec3s & p );
void create( field & f, vec3s & p );
void kill( field & f, vec3s & p );
void toggle( field & f, vec3s & p );
void nextStep(field & f);

