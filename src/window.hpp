#pragma once
#include <unistd.h>
#include <string>
#include "error.hpp"

class WindowManager {
  public:
    WindowManager( std::string title, int wSize = 640, int hSize = 640 )
        : windowName( title ), width( wSize ), height( hSize ){};
    void setFrameRate( uint32_t fps );
    void mainLoop( void );
    void setInitFunc( void ( *param )( void ) ) { initCallback = param; }
    void setRenderFunc( void ( *param )( void ) ) { renderCallback = param; }
    void setEventFunc( void ( *param )( SDL_Event * event ) ) {
        eventCallback = param;
    }
    uint32_t getFPS( void );
    void killWindow( void );
    int getWidth( void ) { return width; }
    int getHeight( void ) { return height; }
    ~WindowManager();

  private:
    SDL_Window * window = nullptr;
    SDL_Renderer * render = nullptr;
    SDL_GLContext context;
    SDL_Event event;
    std::string windowName;
    bool quitFlag = false;
    int width, height;
    uint32_t framerate = 1000 / 25;
    void ( *initCallback )( void ) = nullptr;
    void ( *renderCallback )( void ) = nullptr;
    void ( *eventCallback )( SDL_Event * event ) = nullptr;
};
