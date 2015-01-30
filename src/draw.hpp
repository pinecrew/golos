#pragma once
#include <cmath>
#include "types.hpp"

enum COLOR {
    COLOR_WHITE     = 0xffffff,
    COLOR_BLACK     = 0x000000,
    COLOR_RED       = 0xff0000,
    COLOR_GREEN     = 0x00ff00,
    COLOR_BLUE      = 0x0000ff,
    COLOR_YELLOW    = 0xffff00,
};

void draw_init( SDL_Renderer * render );
int set_coloru( Uint32 color );
int set_color3u( Uint8 red, Uint8 green, Uint8 blue );
int set_color4u( Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha );
int draw_aaline( int x1, int y1, int x2, int y2 );
void draw_sphere( vec3s n, SDL_Point center, float R, field & f );
int draw_filled_polygon( const int * vx, const int * vy, const int n );
