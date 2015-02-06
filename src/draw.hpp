#pragma once
#include <cmath>
#include <SDL2/SDL.h>
#include "field.hpp"

enum COLOR {
    COLOR_WHITE     = 0xffffff,
    COLOR_BLACK     = 0x000000,
    COLOR_RED       = 0xff0000,
    COLOR_GREEN     = 0x00ff00,
    COLOR_BLUE      = 0x0000ff,
    COLOR_YELLOW    = 0xffff00,
    COLOR_MAGENTA   = 0xff00ff,
};

void draw_init( SDL_Renderer * render );
Uint32 get_coloru( void );
int set_coloru( Uint32 color );
int set_color3u( Uint8 red, Uint8 green, Uint8 blue );
int set_color4u( Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha );
void draw_rectangle_param( int x, int y, int w, int h, bool fill );
int draw_aaline( int x1, int y1, int x2, int y2 );
void draw_sphere( field & f, SDL_Point center, vec3s n, vec3s light );
int draw_filled_polygon( const SDL_Point * vs, const int n );
vec3s screen_to_field(SDL_Point p, vec3s n, SDL_Point c, field & f );
