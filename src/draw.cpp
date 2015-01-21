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

/* рисуем эллипс с центром в точке (x, y) с полуосями a и b */
void draw_ellipse( int x, int y, int a, int b ) {
    const unsigned segments = 64;
    const float dt = 2.0 * M_PI / segments;
    SDL_Point pos[segments + 2];

    if ( b != 0 ) {
        int i = 0;
        for ( float t = 0.0; t < dt * ( segments + 2 ); t += dt ) {
            pos[i].x = x + round( cos( t ) * a );
            pos[i].y = y + round( sin( t ) * b );
            i++;
        }
        SDL_RenderDrawLines( _render, pos, i );
    } else {
        int x1, y1, x2, y2;
        x1 = x - a;
        y1 = y - b;
        x2 = x + a;
        y2 = y + b;
        SDL_RenderDrawLine( _render, x1, y1, x2, y2 );
    }
}