#include <cstdio>
#include <SDL2/SDL.h>
#include "draw.hpp"

const char * game_name = "Game Of Life On Surface";
int screen_width = 640;
int screen_height = 640;
int R = 200;
field f(16, 32);
bool quit_flag = false;
SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Event event;

void game_send_error( int code ) {
    printf( "[error]: %s\n", SDL_GetError() );
    exit( code );
}

void game_event( SDL_Event * event ) {
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            quit_flag = true;
            break;
        case SDL_WINDOWEVENT:
            if ( event->window.event == SDL_WINDOWEVENT_RESIZED ) {
                screen_width  = event->window.data1;
                screen_height = event->window.data2;
            }
            break;
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_ESCAPE:
                    quit_flag = true;
                    break;
                default:
                    break;
            }
        default:
            break;
    }
}

void game_loop( void ) {
    // insert code
}

// testing function
void draw_star( int x0, int y0, float R, float r, float angle, int n, Uint32 color ) {
    int * x = new int [n * 2 + 1];
    int * y = new int [n * 2 + 1];
    float a = 0.0f, A = 0.0f;

    angle *= M_PI / 180.0f;
    for ( int i = 0; i < n * 2; i++ ) {
        if ( i % 2 == 0 ) {
            A = R;
        } else {
            A = r / 2;
        }
        x[i] = x0 + round( A * cos( a * M_PI / 180.0f + angle ) );
        y[i] = y0 - round( A * sin( a * M_PI / 180.0f + angle ) );
        a += 180.0f / (float) n;
    }
    x[n * 2 + 1] = x[0];
    y[n * 2 + 1] = y[0];
    draw_filled_polygon( x, y, 2 * n, color );
    delete[] x;
    delete[] y;
}

void game_render( void ) {
    SDL_RenderClear( render );
    set_coloru( COLOR_WHITE );
    draw_sphere( { screen_width / 2, screen_height / 2 }, R, f );
    draw_star( screen_width / 2, screen_height / 2, R, R / 1.5f, 18.0f, 5, COLOR_RED );
    set_coloru( COLOR_BLACK );
    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void ) {
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( window == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |
                                 SDL_RENDERER_TARGETTEXTURE );
    if ( render == NULL ) {
        game_send_error( EXIT_FAILURE );
    }
    SDL_SetRenderDrawBlendMode( render, SDL_BLENDMODE_BLEND );
    draw_init( render );
}

int main( int argc, char * argv[] ) {
    Uint32 FPS_MAX = 1000 / 63; // ~ 60 fps

    game_init();
    while ( quit_flag == false ) {
        game_event( &event );
        game_loop();
        game_render();
        SDL_Delay( FPS_MAX );
    }
    game_destroy();
    return EXIT_SUCCESS;
}
