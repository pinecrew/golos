#include "draw.hpp"

SDL_Renderer * _render = NULL;

void draw_init( SDL_Renderer * render ) {
    _render = render;
}

int set_coloru( Uint32 color ) {
    Uint8 r, ro, g, go, b, bo, ao;

    r = ( color >> 16 );
    g = ( ( color >> 8 ) & 0xff );
    b = ( color & 0xff );
    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    return SDL_SetRenderDrawColor( _render, r, g, b, 0xff );
}

int set_color3u( Uint8 red, Uint8 green, Uint8 blue ) {
    Uint8 ro, go, bo, ao;

    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    return SDL_SetRenderDrawColor( _render, red, green, blue, 0xff );
}

int set_color4u( Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha ) {
    Uint8 ro, go, bo, ao;

    SDL_GetRenderDrawColor( _render, &ro, &go, &bo, &ao );
    return SDL_SetRenderDrawColor( _render, red, green, blue, alpha );
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

int compare_int( const void * a, const void * b ) {
    return ( *( const int * ) a ) - ( *( const int * ) b );
}

// from SDL2_gfxPrimitives.c : filledPolygonRGBAMT
int draw_filled_polygon( const int * vx, const int * vy, const int n, Uint32 color ) {
    int min_y, max_y, result, counts;
    int ind1, ind2, x1, x2, y1, y2;
    int * polygons = new int [n];
    int xa, xb;

    if ( vx == NULL || vy == NULL || n < 3 ) {
        return -1;
    }
    min_y = vy[0]; max_y = vy[0];
    for ( int i = 1; i < n; i++ ) {
        if ( vy[i] < min_y ) {
            min_y = vy[i];
        } else if ( vy[i] > max_y ) {
            max_y = vy[i];
        }
    }
    result = 0;
    for ( int y = min_y; y < max_y; y++ ) {
        counts = 0;
        for ( int i = 0; i < n; i++ ) {
            if ( !i ) {
                ind1 = n - 1;
                ind2 = 0;
            } else {
                ind1 = i - 1;
                ind2 = i;
            }
            y1 = vy[ind1];
            y2 = vy[ind2];
            if ( y1 < y2 ) {
                x1 = vx[ind1];
                x2 = vx[ind2];
            } else if ( y1 > y2 ) {
                y2 = vy[ind1];
                y1 = vy[ind2];
                x2 = vx[ind1];
                x1 = vx[ind2];
            } else {
                continue;
            }
            if ( ( ( y >= y1 ) && ( y < y2 ) ) || ( ( y == max_y ) && ( y > y1 ) && ( y <= y2 ) ) ) {
                polygons[counts++] = ( ( 65536 * ( y - y1 ) ) / ( y2 - y1 ) ) * ( x2 - x1 ) + ( 65536 * x1 );
            }
        }
        qsort( polygons, counts, sizeof( int ), compare_int );
        result = 0;
        result |= SDL_SetRenderDrawBlendMode( _render, SDL_BLENDMODE_NONE );
        result |= set_coloru( color );
        for ( int i = 0; i < counts; i += 2 ) {
            xa = polygons[i+0] + 1;
            xb = polygons[i+1] - 1;
            xa = ( xa >> 16 ) + ( ( xa & 32768 ) >> 15 );
            xb = ( xb >> 16 ) + ( ( xb & 32768 ) >> 15 );
            result |= SDL_RenderDrawLine( _render, xa, y, xb, y );
        }
    } 
    delete[] polygons;
    return result;
}