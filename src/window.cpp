#include "window.hpp"

void WindowManager::mainLoop( void ) {
    // init SDL subsystems
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) ) {
        Panic( SDL_GetError() );
    }
    // init double buffer
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    // color configuration
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    // create SDL window
    window = SDL_CreateWindow( windowName.c_str(), SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
    if ( window == nullptr ) {
        Panic( SDL_GetError() );
    }
    // init OpenGL context
    context = SDL_GL_CreateContext( window );
    if ( initCallback ) {
        initCallback();
    }
    SDL_GL_SetSwapInterval( 1 );
    uint32_t last = 0;
    while ( !quitFlag ) {
        uint32_t current = SDL_GetTicks();
        if ( current > last + 10 ) {
            if ( eventCallback ) {
                eventCallback( &event );
            }
            if ( renderCallback ) {
                renderCallback();
            }
            SDL_GL_SwapWindow( window );
            last = current;
        }
        usleep( ( current - last ) * 1000 );
    }
}

uint32_t WindowManager::getFPS( void ) {
    static uint32_t NewCount = 0, LastCount = 0, FpsRate = 0;
    static int FrameCount = 0;

    NewCount = (float) SDL_GetTicks();
    if ( ( NewCount - LastCount ) > 1000 ) {
        FpsRate = ( FrameCount * 1000 ) / ( NewCount - LastCount );
        LastCount = NewCount;
        FrameCount = 0;
    }
    FrameCount++;
    return FpsRate;
}

void WindowManager::killWindow( void ) { quitFlag = true; }

WindowManager::~WindowManager() {
    SDL_DestroyWindow( window );
    SDL_Quit();
}
