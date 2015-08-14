#include "window.hpp"

void Panic( const char * error ) {
    std::string error_data = "[error] ";
    error_data += error;
    error_data += " exits failure!";
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", error_data.c_str(), nullptr );
    exit( EXIT_FAILURE );
}

void WindowManager::MainLoop( void ) {
    float current = 0.0f, last = 0.0f;
    // init SDL subsystems
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) ) {
        Panic();
    }
    // init double buffer
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    // color configuration
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    // create SDL window
    window = SDL_CreateWindow( windowname.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
    if ( window == nullptr ) {
        Panic();
    }
    // init OpenGL context
    context = SDL_GL_CreateContext( window );
    init_callback();
    while ( !quit_flag ) {
        current = (float) SDL_GetTicks();
        if ( current > last + 1000.0f / 60.0f ) {
            event_callback( &event );
            render_callback();
            SDL_GL_SwapWindow( window );
            last = current;
        }
    }
}

void WindowManager::KillWindow( void ) {
    quit_flag = true;
}

void WindowManager::Panic( void ) {
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr );
    exit( EXIT_FAILURE );
}

WindowManager::~WindowManager() {
    SDL_DestroyWindow( window );
    SDL_Quit();
}
