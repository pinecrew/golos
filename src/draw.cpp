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
void draw_ellipse( unsigned int x, unsigned int y, float a, float b ) {
    const std::size_t npoints = 64;
    const float dt = 2.0 * M_PI / ( npoints - 1 );
    SDL_Point pos[npoints];
    unsigned int i = 0;

    if ( a > 0 && b > 0 ) {
        for ( float t = 0; t < npoints * dt; ++i, t += dt ) {
            pos[i].x = x + round( cos( t ) * a );
            pos[i].y = y + round( sin( t ) * b );
        }
    } else {
        pos[0] = { ( int )( x - a ), ( int )( y - b ) };
        pos[1] = { ( int )( x + a ), ( int )( y + b ) };
        i = 2;
    }
    SDL_RenderDrawLines( _render, pos, i );
}

void draw_sphere( screenPoint center, std::size_t R, field & f ) {
    // меридианы
    for ( unsigned int i = 0; i < f.width; ++i ) {
        float p = i * 2 * M_PI / f.width;
        draw_ellipse( center.x, center.y, R * fabs( cos( p ) ), R );
    }
    // широты
    for ( unsigned int i = 1; i < f.height; ++i ) {
        float p = i * M_PI / f.height;
        draw_ellipse( center.x, center.y + R * cos( p ), R * sin( p ), 0 );
    }
}

