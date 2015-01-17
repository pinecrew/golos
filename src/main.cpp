#include <cstdio>
#include <SDL2/SDL.h>
#include "draw.hpp"

const char * game_name = "Game Of Life On Surface";
int screen_width = 640;
int screen_height = 480;
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

void draw_point( int x, int y, int size ) {
    SDL_Rect rect = { x, y, size, size };
    SDL_RenderFillRect( render, &rect );
}

void game_render( void ) {
    SDL_RenderClear( render );
    set_coloru( COLOR_WHITE );
    draw_point( 150, 80, 10 );
    draw_point( 240, 80, 10 );
    draw_ellipse_part( 100, 100, 50, 40, false );
    draw_ellipse_part( 200, 100, 50, 40, false );
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