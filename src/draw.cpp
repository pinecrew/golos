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


void draw_ellipse( int x, int y, int a, int b, float angle, Uint8 segments ) {
    float dt = 2.0 * M_PI / (float) ( segments - 1 );
    SDL_Point pos[segments];
    float t = 0, xp, yp;

    if ( b != 0 ) {
        for ( Uint8 i = 0; i < segments; i++ ) {
            xp = x + ( cos( t ) + 1 ) * a;
            yp = y + ( sin( t ) + 1 ) * b;
            pos[i].x = ceil( xp * cos( angle ) + yp * sin( angle ) );
            pos[i].y = ceil( xp * sin( angle ) + yp * cos( angle ) );
            t += dt;
        }
        SDL_RenderDrawLines( _render, pos, segments );
    } else {
        float tmp_cos_x, tmp_cos_y, tmp_sin_x, tmp_sin_y;
        int x1, y1, x2, y2;
        tmp_cos_x = x * cos( angle );
        tmp_cos_y = y * cos( angle );
        tmp_sin_x = x * sin( angle );
        tmp_sin_y = y * sin( angle );
        x1 = ceil( tmp_cos_x + tmp_sin_y );
        y1 = ceil( tmp_sin_x + tmp_cos_y );
        x2 = ceil( ( x + 2 * a ) * cos( angle ) + tmp_sin_y );
        y2 = ceil( ( x + 2 * a ) * sin( angle ) + tmp_cos_y );
        SDL_RenderDrawLine( _render, x1, y1, x2, y2 );
    }
}

void draw_arc( int x, int y, int a, int b, float angle, bool up, Uint8 segments ) {
    float dt = M_PI / (float) ( segments - 1 );
    SDL_Point pos[segments];
    float t = 0, xp, yp;

    if ( b != 0 ) {
        dt = !up ? dt : -dt;
        for ( Uint8 i = 0; i < segments; i++ ) {
            xp = x + ( cos( t ) + 1 ) * a;
            yp = y + ( sin( t ) + 1 ) * b;
            pos[i].x = ceil( xp * cos( angle ) + yp * sin( angle ) );
            pos[i].y = ceil( xp * sin( angle ) + yp * cos( angle ) );
            t += dt;
        }
        SDL_RenderDrawLines( _render, pos, segments );
    } else {
        float tmp_cos_x, tmp_cos_y, tmp_sin_x, tmp_sin_y;
        int x1, y1, x2, y2;
        tmp_cos_x = x * cos( angle );
        tmp_cos_y = y * cos( angle );
        tmp_sin_x = x * sin( angle );
        tmp_sin_y = y * sin( angle );
        x1 = ceil( tmp_cos_x + tmp_sin_y );
        y1 = ceil( tmp_sin_x + tmp_cos_y );
        x2 = ceil( ( x + 2 * a ) * cos( angle ) + tmp_sin_y );
        y2 = ceil( ( x + 2 * a ) * sin( angle ) + tmp_cos_y );
        SDL_RenderDrawLine( _render, x1, y1, x2, y2 );
    }
}