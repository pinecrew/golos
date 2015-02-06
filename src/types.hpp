#pragma once

// windows MinGW fix
#ifdef __MINGW32__
#ifndef M_PI
    const double M_PI   = 3.14159265358979323846264338327950288;
#endif
#endif