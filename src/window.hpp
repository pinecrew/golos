#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <cstdlib>

void Panic( const char * error );

class WindowManager {
public:
    WindowManager( std::string title, int w_size = 640, int h_size = 640 ) :
        windowname( title ), width( w_size ), height( h_size ) {};
    void MainLoop( void );
    void SetInitFunc( void ( *param )( void ) ) { init_callback = param; }
    void SetRenderFunc( void ( *param )( void ) ) { render_callback = param; }
    void SetEventFunc( void ( *param )( SDL_Event * event ) ) { event_callback = param; }
    float GetFPS( void );
    void KillWindow( void );
    void Panic( void );
    int GetWidth( void ) { return width; }
    int GetHeight( void ) { return height; }
    ~WindowManager();
private:
    SDL_Window * window = nullptr;
    SDL_Renderer * render = nullptr;
    SDL_GLContext context;
    SDL_Event event;
    std::string windowname;
    bool quit_flag = false;
    int width, height;
    void ( *init_callback )( void );
    void ( *render_callback )( void );
    void ( *event_callback )( SDL_Event * event );
};
