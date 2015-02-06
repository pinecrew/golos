#include <cstdio>
#include <ctime>
#include <SDL2/SDL.h>
#include "draw.hpp"
#include "font.hpp"
#include "field.hpp"

const wchar_t tmp_str[] = L"(%s) FPS: %.2f; theta: %.2f; phi: %.2f; delay %d";
const wchar_t * game_status[] = {
    (const wchar_t *) "pause",
    (const wchar_t *) "play"
};
const wchar_t help_info[] =
    L"\t\t   help menu\n"
    L"-------------------------------\n"
    L"   F1 -- this menu\n"
    L"  ESC -- quit\n"
    L"SPACE -- play/pause\n"
    L" RBLK -- set/clear\n"
    L" LBLK -- rotation (mouse move)\n"
    L"    F -- random fill\n"
    L"    R -- clear field\n"
    L"    > -- speed up\n"
    L"    < -- speed down\n"
    L" []{} -- rotate\n"
    L"-------------------------------";
const int help_box_width = 260;
const int help_box_height = 110;
int MAX_COUNT = 5;
vec3s view_direction = { 1, M_PI / 4, 0 };
vec3s light_source = { 1, M_PI / 4, 0 };
field f( 10, 20, 200 );

struct Window {
    const char * name = (const char *) "Game Of Life On fieldace";
    int SDL_RENDER_DRIVER = 3; // use software renderer
    SDL_Window * window = nullptr;
    SDL_Renderer * render = nullptr;
    font_table_t * ft = nullptr;
    SDL_Event event;
    bool quit_flag = false;
    bool button_left_set = false;
    bool button_right_set = false;
    bool game_step = false;
    bool help_flag = false;
    int width = 640;
    int height = 640;
    int counter = 0;
    int mouse_x, mouse_y;
} gw;

template< typename T > inline T sqr( const T & i ) { return ( i ) * ( i ); } 

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
    for ( std::size_t i = 0; i < ( f.width * f.height ) / 3; i++ ) {
        f[rand()%f.height][rand()%f.width] = true;
    }
}

void set_cell( int x, int y, bool create_flag ) {
    SDL_Point center = { gw.width / 2, gw.height / 2 };
    SDL_Point mouse = { x, y };
    if ( sqr( x - center.x ) + sqr( y - center. y ) < sqr( f.r ) ) {
        vec3s point = screen_to_field( mouse, view_direction, center, f );
        if ( create_flag ) {
            f.create( point );
        } else {
            f.toggle( point );
        }
    }
}

void game_event( void ) {
    SDL_PollEvent( &( gw.event ) );
    switch ( gw.event.type ) {
        case SDL_QUIT:
            gw.quit_flag = true;
            break;
        case SDL_WINDOWEVENT:
            if ( gw.event.window.event == SDL_WINDOWEVENT_RESIZED ) {
                gw.width  = gw.event.window.data1;
                gw.height = gw.event.window.data2;
            }
            break;
        case SDL_KEYDOWN:
            switch ( gw.event.key.keysym.sym ) {
                case SDLK_ESCAPE:
                    gw.quit_flag = true;
                    break;
                case SDLK_SPACE:
                    gw.game_step = !gw.game_step;
                    break;
                case SDLK_F1:
                    gw.help_flag = !gw.help_flag;
                    gw.event.key.keysym.sym = 0;
                    break;
                case SDLK_UP:
                    view_direction.rotate( -0.01f, 0.0f );
                    break;
                case SDLK_DOWN:
                    view_direction.rotate( 0.01f, 0.0f );
                    break;
                case SDLK_LEFT:
                    view_direction.rotate( 0.0f, -0.01f );
                    break;
                case SDLK_RIGHT:
                    view_direction.rotate( 0.0f, 0.01f );
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
                    for ( std::size_t i = 0; i < f.height; i++ ) {
                        for ( std::size_t j = 0; j < f.width; j++ ) {
                            f[i][j] = false;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEMOTION:
            if ( gw.button_left_set ) {
                view_direction.rotate( -( gw.event.button.y - gw.mouse_y ) / 100.0f,
                                       -( gw.event.button.x - gw.mouse_x ) / 100.0f );
                gw.mouse_x = gw.event.button.x;
                gw.mouse_y = gw.event.button.y;
            }
            if ( gw.button_right_set ) {
                set_cell( gw.event.button.x, gw.event.button.y, true );
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( gw.event.button.button ) {
                case SDL_BUTTON_LEFT:
                    if ( !gw.button_left_set ) {
                        gw.mouse_x = gw.event.button.x;
                        gw.mouse_y = gw.event.button.y;
                    }
                    gw.button_left_set = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    if ( !gw.button_right_set ) {
                        set_cell( gw.event.button.x, gw.event.button.y, false );
                    }
                    gw.button_right_set = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            gw.button_left_set = gw.button_right_set = false;
            break;
        default:
            break;
    }
}

void game_loop( void ) {
    if ( gw.game_step && gw.counter >= MAX_COUNT ) {
        f.nextGeneration();
        gw.counter = 0;
    } else {
        gw.counter++;
    }
}

void game_render( void ) {
    const std::size_t BUFFER_SIZE = 128;
    wchar_t buffer[BUFFER_SIZE];

    SDL_RenderClear( gw.render );
    set_coloru( COLOR_WHITE );
    draw_sphere( f, { gw.width / 2, gw.height / 2 },
                 view_direction, light_source );
    swprintf( buffer, BUFFER_SIZE, tmp_str, game_status[int(gw.game_step)], get_fps(),
              view_direction.theta, view_direction.phi, MAX_COUNT );
    font_draw( gw.render, gw.ft, buffer, 5, gw.height - 16 );
    if ( gw.help_flag ) {
        set_color4u( 0x00, 0x00, 0xff, 0x96 );
        draw_rectangle_param( ( gw.width - help_box_width ) / 2,
                              ( gw.height - help_box_height ) / 2,
                              help_box_width, help_box_height, true );
        font_draw( gw.render, gw.ft, help_info, 
                   ( gw.width - help_box_width ) / 2,
                   ( gw.height - help_box_height ) / 2 + 4 );
    }
    set_coloru( COLOR_BLACK );
    SDL_RenderPresent( gw.render );
}

void game_destroy( void ) {
    font_destroy( gw.ft );
    SDL_DestroyRenderer( gw.render );
    SDL_DestroyWindow( gw.window );
    SDL_Quit();
}

void game_init( void ) {
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    gw.window = SDL_CreateWindow( gw.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           gw.width, gw.height, 
                                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( gw.window == nullptr ) {
        game_send_error( EXIT_FAILURE );
    }
    gw.render = SDL_CreateRenderer( gw.window, gw.SDL_RENDER_DRIVER, 
                                             SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC | 
                                             SDL_RENDERER_TARGETTEXTURE );
    if ( gw.render == nullptr ) {
        game_send_error( EXIT_FAILURE );
    }
    SDL_SetRenderDrawBlendMode( gw.render, SDL_BLENDMODE_BLEND );
    draw_init( gw.render );
    font_load( gw.render, &( gw.ft ), "./data/font.cfg" );
    srand( time( nullptr ) );
}

int main( int argc, char * argv[] ) {
    float current = 0.0f, last = 0.0f;

    game_init();
    while ( !gw.quit_flag ) {
        current = (float) SDL_GetTicks();
        if ( current > last + 1000.0f / 60.0f ) {
            game_event();
            game_loop();
            game_render();
            last = current;
        }
    }
    game_destroy();
    return EXIT_SUCCESS;
}
