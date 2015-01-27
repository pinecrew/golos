#pragma once
#include <cmath>
#include "types.hpp"

// windows MinGW fix
#ifndef M_PI
    const double M_PI   = 3.14159265358979323846;
    const double M_PI_2 = 1.57079632679489661923;
#endif
void cell_contour( cell c, field f, std::size_t npoints, surfPoint* contour );
cell cell_from_point( field & f, surfPoint & p );
void create( field & f, surfPoint & p );
void kill( field & f, surfPoint & p );
void toggle( field & f, surfPoint & p );
void nextStep(field & f);

