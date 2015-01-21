#pragma once
#include <SDL2/SDL.h>
#include <cmath>

enum COLOR {
    COLOR_WHITE     = 0xffffff,
    COLOR_BLACK     = 0x000000,
    COLOR_RED       = 0xff0000,
    COLOR_GREEN     = 0x00ff00,
    COLOR_BLUE      = 0x0000ff,
    COLOR_YELLOW    = 0xffff00, 
};

void draw_init( SDL_Renderer * render );
void set_coloru( Uint32 color );
void set_color3u( Uint8 red, Uint8 green, Uint8 blue );
void set_color4u( Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha );
void draw_ellipse( unsigned int x, unsigned int y, float a, float b );
