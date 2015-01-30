#include <cstdio>
#include <SDL2/SDL.h>
#include "draw.hpp"

const char * game_name = "Game Of Life On Surface";
int screen_width = 640;
int screen_height = 640;
float R = 200;
int px, py;
vec3s delta = { 0, 0, 0 };
vec3s view_direction = {1, M_PI / 4, 0 };
field f(16, 32);
bool quit_flag = false;
bool button_set = false;
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
                case SDLK_UP:
                    view_direction.theta -= 0.01f;
                    break;
                case SDLK_DOWN:
                    view_direction.theta += 0.01f;
                    break;
                case SDLK_LEFT:
                    view_direction.phi -= 0.01f;
                    break;
                case SDLK_RIGHT:
                    view_direction.phi += 0.01f;
                    break;
                default:
                    break;
            }
        case SDL_MOUSEMOTION:
            if ( button_set ) {
                // experimental scrolling
                delta.phi = -( event->button.x - px ) / 100.0f;
                delta.theta = ( event->button.y - py ) / 100.0f;
                px = event->button.x;
                py = event->button.y;
                // theta & phi borders
                /*
                if ( view_direction.theta >= M_PI ) {
                    view_direction.theta -= M_PI;
                } else if ( view_direction.theta < 0.0f ) {
                    view_direction.theta += M_PI;
                }
                if ( view_direction.phi >= 2.0f * M_PI ) {
                    view_direction.phi -= 2.0f * M_PI;
                } else if ( view_direction.phi < 0.0f ) {
                    view_direction.phi += 2.0f * M_PI;
                }
                */
                view_direction += delta;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( event->button.button ) {
                case SDL_BUTTON_LEFT:
                    if ( button_set == false ) {
                        px = event->button.x;
                        py = event->button.y;
                    }
                    break;
                default:
                    break;
            }
            button_set = true;
            break;
        case SDL_MOUSEBUTTONUP:
            button_set = false;
            break;
        default:
            break;
    }
}

void game_loop( void ) {
    // insert code
}

void game_render( void ) {
    SDL_RenderClear( render );
    set_coloru( COLOR_WHITE );
    draw_sphere( view_direction, {screen_width / 2, screen_height / 2}, R, f );
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
