#include <cstdio>
#include <ctime>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "font.hpp"
#include "logics.hpp"

#define sqr(i) (i) * (i)

const int SDL_RENDER_DRIVER = 3; // use software renderer
const char * game_name = "Game Of Life On Surface";
const wchar_t tmp_str[] = L"(%s) FPS: %.2f; theta: %.2f; phi: %.2f; delay %d";
const wchar_t * game_status[] = {
    (const wchar_t *) "pause",
    (const wchar_t *) "play"
};
const wchar_t help_info[] =
    L"help menu:\n\n"
    L"   F1 -- this menu\n"
    L"  ESC -- quit\n"
    L"SPACE -- play/pause\n"
    L"    F -- random fill\n"
    L"    R -- clear field\n"
    L"    > -- speed up\n"
    L"    < -- speed down\n"
    L" []{} -- rotate";
int game_counter = 0, MAX_COUNT = 5;
int screen_width = 640;
int screen_height = 640;
const int help_box_width = 200;
const int help_box_height = 90;
float R = 200;
int px, py;
vec3s delta = { 1, 0, 0 };
vec3s view_direction = { 1, M_PI / 4, 0 };
vec3s light_source = { 1, M_PI / 4, 0 };
field f(16, 32);
bool quit_flag = false;
bool button_left_set = false;
bool button_right_set = false;
bool game_step = false;
bool help_flag = false;
SDL_Window * window = nullptr;
SDL_Renderer * render = nullptr;
SDL_Event event;
font_table_t * ft = nullptr;

void game_send_error( int code ) {
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr );
    exit( code );
}

float get_fps( void ) {
    static float NewCount = 0.0f, LastCount = 0.0f, FpsRate = 0.0f;
    static int FrameCount = 0;

    NewCount = (float) SDL_GetTicks();
    if ( ( NewCount - LastCount ) > 1000.0f ) {
        FpsRate = ( FrameCount * 1000.0f ) / ( NewCount - LastCount );
        LastCount = NewCount;
        FrameCount = 0;
    }
    FrameCount++;
    return FpsRate;
}

void random_fill( void ) {
    for ( std::size_t i = 0; i < 128; i++ ) {
        f[rand()%16][rand()%32] = true;
    }
}

void set_cell( int x, int y, bool create_flag ) {
    SDL_Point center = { screen_width / 2, screen_height / 2 };
    SDL_Point mouse = { x, y };
    if ( sqr( x - center.x ) + sqr( y - center. y ) < sqr( R ) ) {
        vec3s point = screen_to_surf( view_direction, R, mouse, center );
        if ( create_flag ) {
            create( f, point );
        } else {
            toggle( f, point );
        }
    }
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
                case SDLK_SPACE:
                    game_step = !game_step;
                    break;
                case SDLK_F1:
                    help_flag = !help_flag;
                    event->key.keysym.sym = 0;
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
                case SDLK_PERIOD:
                    if ( MAX_COUNT > 0 ) {
                        MAX_COUNT -= 1;
                    }
                    break;
                case SDLK_COMMA:
                    MAX_COUNT += 1;
                    break;
                case SDLK_f:
                    random_fill();
                    break;
                case SDLK_r:
                    for ( std::size_t i = 0; i < f.width; i++ ) {
                        for ( std::size_t j = 0; j < f.height; j++ ) {
                            f[j][i] = false;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEMOTION:
            if ( button_left_set ) {
                delta.phi = ( event->button.x - px ) / 100.0f;
                delta.theta = ( event->button.y - py ) / 100.0f;
                px = event->button.x;
                py = event->button.y;
                view_direction -= delta;
            }
            if ( button_right_set ) {
                set_cell( event->button.x, event->button.y, true );
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( event->button.button ) {
                case SDL_BUTTON_LEFT:
                    if ( button_left_set == false ) {
                        px = event->button.x;
                        py = event->button.y;
                    }
                    button_left_set = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    if ( button_right_set == false ) {
                        set_cell( event->button.x, event->button.y, false );
                    }
                    button_right_set = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            button_left_set = button_right_set = false;
            break;
        default:
            break;
    }
}

void game_loop( void ) {
    if ( game_step && game_counter >= MAX_COUNT ) {
        f.nextGeneration();
        game_counter = 0;
    } else {
        game_counter++;
    }
}

void game_render( void ) {
    const std::size_t BUFFER_SIZE = 128;
    wchar_t buffer[BUFFER_SIZE];

    SDL_RenderClear( render );
    set_coloru( COLOR_WHITE );
    draw_sphere( view_direction, light_source, {screen_width / 2, screen_height / 2}, R, f );
    swprintf( buffer, BUFFER_SIZE, tmp_str, game_status[int(game_step)], get_fps(),
              view_direction.theta, view_direction.phi, MAX_COUNT );
    font_draw( render, ft, buffer, 5, screen_height - 16 );
    if ( help_flag ) {
        set_color4u( 0x00, 0x00, 0xff, 0x96 );
        draw_rectangle_param( ( screen_width - help_box_width ) / 2,
                              ( screen_height - help_box_height ) / 2,
                              help_box_width, help_box_height, true );
        font_draw( render, ft, help_info, ( screen_width - help_box_width ) / 2,
                   ( screen_height - help_box_height ) / 2 + 4 );
    }
    set_coloru( COLOR_BLACK );
    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    font_destroy( ft );
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void ) {
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( window == nullptr ) {
        game_send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, SDL_RENDER_DRIVER, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE );
    if ( render == nullptr ) {
        game_send_error( EXIT_FAILURE );
    }
    SDL_SetRenderDrawBlendMode( render, SDL_BLENDMODE_BLEND );
    draw_init( render );
    font_load( render, &ft, "./data/font.cfg" );
    srand( time( nullptr ) );
}

int main( int argc, char * argv[] ) {
    float current = 0.0f, last = 0.0f;

    game_init();
    while ( quit_flag == false ) {
        current = (float) SDL_GetTicks();
        if ( current > last + 1000.0f / 60.0f ) {
            game_event( &event );
            game_loop();
            game_render();
            last = current;
        }
    }
    game_destroy();
    return EXIT_SUCCESS;
}
