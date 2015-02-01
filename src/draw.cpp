#include <algorithm>
#include "draw.hpp"
#include "logics.hpp"

SDL_Renderer * _render = NULL;

void draw_init( SDL_Renderer * render ) {
    _render = render;
}

Uint32 get_coloru( void ) {
    Uint8 r, g, b;

    SDL_GetRenderDrawColor( _render, &r, &g, &b, NULL );
    return ( r << 16 ) + ( g << 8 ) + b;
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

int draw_aaline( int x1, int y1, int x2, int y2 ) {
    Uint32 intshift, erracc, erradj, erracctmp, wgt;
    int dx, dy, tmp, xdir, y0p1, x0pxdir, result;
    Sint32 xx0, yy0, xx1, yy1;
    Uint8 r, g, b, a;

    result = SDL_GetRenderDrawColor( _render, &r, &g, &b, &a );
    xx0 = x1; yy0 = y1;
    xx1 = x2; yy1 = y2;
    if ( yy0 > yy1 ) {
        tmp = yy0; yy0 = yy1;
        yy1 = tmp; tmp = xx0;
        xx0 = xx1; xx1 = tmp;
    }
    dx = xx1 - xx0;
    dy = yy1 - yy0;
    if ( dx == 0 || dy == 0 ) {
        return SDL_RenderDrawLine( _render, x1, y1, x2, y2 );
    }
    xdir = 1;
    if ( dx < 0 ) {
        xdir = -1;
        dx = -dx;
    }
    erracc = 0;
    intshift = 24;
    result |= set_color3u( r, g, b );
    result |= SDL_RenderDrawPoint( _render, x1, y1 );
    if ( dy > dx ) {
        erradj = ( ( dx << 16 ) / dy ) << 16;
        x0pxdir = xx0 + xdir;
        while ( --dy ) {
            erracctmp = erracc;
            erracc += erradj;
            if ( erracc <= erracctmp ) {
                xx0 = x0pxdir;
                x0pxdir += xdir;
            }
            yy0++;
            wgt = ( erracc >> intshift ) & 255;
            result |= set_color4u( r, g, b, 255 - wgt );
            result |= SDL_RenderDrawPoint( _render, xx0, yy0 );
            result |= set_color4u( r, g, b, wgt );
            result |= SDL_RenderDrawPoint( _render, x0pxdir, yy0 );
        }
    } else {
        erradj = ( ( dy << 16 ) / dx ) << 16;
        y0p1 = yy0 + 1;
        while ( --dx ) {
            erracctmp = erracc;
            erracc += erradj;
            if ( erracc <= erracctmp ) {
                yy0 = y0p1;
                y0p1++;
            }
            xx0 += xdir;
            wgt = ( erracc >> intshift ) & 255;
            result |= set_color4u( r, g, b, 255 - wgt );
            result |= SDL_RenderDrawPoint( _render, xx0, yy0 );
            result |= set_color4u( r, g, b, wgt );
            result |= SDL_RenderDrawPoint( _render, xx0, y0p1 );
        }
    }
    return result;
}

void draw_path(vec3s n, SDL_Point center, std::vector<vec3s> vs) {
    // отрисовываем путь
    // пока без проверки невидимых линий
    SDL_Point prev;
    bool pe = false;
    for ( auto v: vs ) {
        if ( visible ( n, v ) )
        {
            SDL_Point cur = surf_to_screen( n, v, center);
            if ( pe )
                draw_aaline( prev.x, prev.y, cur.x, cur.y );
            prev = cur;
            pe = true;
        }
        else
            pe = false;
    }
}

void draw_sphere( vec3s n, SDL_Point center, float R, field & f ) {
    Uint32 color = get_coloru();

    // отрисуем все клетки для теста
    set_color3u(255, 0, 255);
    for ( size_t i = 0; i < f.f.size(); i++ ) {
        for ( size_t j = 0; j < f.f[i].size(); j++ ) {
            if ( f.f[i][j] ) {
                auto cc = cell_contour( { (int)i, (int)j }, f, 32 );
                cc[0].r = R;
                if ( n * cc[0] >= 0 ) { // так не видно косяков
                // if ( visible( n, cc[0] ) ) {
                    SDL_Point sc[cc.size()];
                    for (std::size_t i = 0; i < cc.size(); ++i)
                    {
                        cc[i].r = R;
                        sc[i] = surf_to_screen( n, cc[i], center);
                    }
                    draw_filled_polygon( sc, 32 );
                }
            }
        }
    }

    set_coloru( color );

    // набор точек от 0 до 2pi
    int size = 65;
    std::vector<vec3s> v(size);
    for (int i = 0; i < size; ++i) {
        v[i].r = R;
        v[i].theta = 2 * M_PI * i / (size - 1);
    }
    // меридианы
    for ( unsigned int i = 0; i < f.width; ++i ) {
        float p = i * 2 * M_PI / f.width;
        for (int i = 0; i < size; ++i) {v[i].phi = p;};
        draw_path( n, center, v);
    }

    for (int i = 0; i < size; ++i) {v[i].phi = 2 * M_PI * i / (size - 1);}
    // широты
    for ( unsigned int i = 1; i < f.height; ++i ) {
        float p = i * M_PI / f.height;
        for (int i = 0; i < size; ++i) {v[i].theta = p;};
        draw_path( n, center, v);
    }

    // оси координат (для проверки корректности отрисовки)
    set_color3u(255, 0, 0);
    draw_path( n, center, {{0,0,0}, {1.2f*R, M_PI/2, 0}});
    set_color3u(0, 255, 0);
    draw_path( n, center, {{0,0,0}, {1.2f*R, M_PI/2, M_PI / 2}});
    set_color3u(0, 0, 255);
    draw_path( n, center, {{0,0,0}, {1.2f*R, 0, 0}});
}

int draw_filled_polygon( const SDL_Point* vs, const int n ) {
    int min_y, max_y, result, counts;
    int ind1, ind2, x1, x2, y1, y2;
    int * polygons = new int [n];
    int xa, xb;

    if ( vs == nullptr || n < 3 ) {
        return -1;
    }
    // нужно тестирование
    min_y = std::min_element( vs, vs + n,
            [](const SDL_Point & a, const SDL_Point & b)
            { return a.y < b.y; } ) -> y;
    max_y = std::max_element( vs, vs + n,
            [](const SDL_Point & a, const SDL_Point & b)
            { return a.y < b.y; } ) -> y;
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
            y1 = vs[ind1].y;
            y2 = vs[ind2].y;
            if ( y1 < y2 ) {
                x1 = vs[ind1].x;
                x2 = vs[ind2].x;
            } else if ( y1 > y2 ) {
                y2 = vs[ind1].y;
                y1 = vs[ind2].y;
                x2 = vs[ind1].x;
                x1 = vs[ind2].x;
            } else {
                continue;
            }
            if ( ( ( y >= y1 ) && ( y < y2 ) ) ||
                 ( ( y == max_y ) && ( y > y1 ) && ( y <= y2 ) ) ) {
                polygons[counts++] =
                    ( ( 65536 * ( y - y1 ) ) / ( y2 - y1 ) ) * ( x2 - x1 ) +
                    ( 65536 * x1 );
            }
        }
        std::sort( polygons, polygons + counts );
        result = 0;
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
