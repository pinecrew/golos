#include "draw.hpp"

SDL_Renderer * _render = NULL;

void draw_init( SDL_Renderer * render ) {
    _render = render;
}

void set_coloru( Uint32 color ) {
    Uint8 r, ro, g, go, b, bo, ao;

    r = ( color >> 16 );
    g = ( ( color >> 8 ) & 0xff );
    b = ( color & 0xff );
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, r, g, b, 0xff );
}

void set_color3u( Uint8 red, Uint8 green, Uint8 blue ) {
    Uint8 ro, go, bo, ao;
    
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, red, green, blue, 0xff );
}

void set_color4u( Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha ) {
    Uint8 ro, go, bo, ao;
    
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    SDL_SetRenderDrawColor( _render, red, green, blue, alpha );
}

void draw_ellipse_part( int x, int y, int a, int b, bool up, Uint8 segment ) {
    float dt = M_PI / (float) ( segment - 1 );
    SDL_Point pos[segment];
    float t = 0;

    dt = !up ? dt : -dt;
    for ( Uint8 i = 0; i < segment; i++ ) {
        pos[i].x = x + ( cos( t ) + 1 ) * a;
        pos[i].y = y + ( sin( t ) + 1 ) * b;
        t += dt;
    }
    SDL_RenderDrawLines( _render, pos, segment );
}