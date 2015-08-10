#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>

struct Window {
    const char * name = (const char *) "Game Of Life On fieldace";
    SDL_Window * window = nullptr;
    SDL_Renderer * render = nullptr;
    SDL_GLContext context;
    SDL_Event event;
    bool quit_flag = false;
    // bool button_left_set = false;
    // bool button_right_set = false;
    // bool game_step = false;
    // bool help_flag = false;
    int width = 640;
    int height = 640;
    // int counter = 0;
    // int mouse_x, mouse_y;
} gw;

GLUquadricObj * sphere;

void golos_panic( void ) {
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), nullptr );
    exit( EXIT_FAILURE );
}

void golos_init( void ) {
    // init SDL subsystems
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) ) {
        golos_panic();
    }
    // init double buffer
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    // color configuration
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    // create SDL window
    gw.window =  SDL_CreateWindow( gw.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        gw.width, gw.height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
    if ( gw.window == nullptr ) {
        golos_panic();
    }
    // init OpenGL context
    gw.context = SDL_GL_CreateContext( gw.window );
    // init OpenGL params
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0 );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glShadeModel( GL_SMOOTH );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0f, (float) gw.width / (float) gw.height, 0.1f, 100.0f );
    glMatrixMode( GL_MODELVIEW );
    // init sphere
    sphere = gluNewQuadric();
}

void golos_event( void ) {
    SDL_PollEvent( &( gw.event ) );
    switch ( gw.event.type ) {
        case SDL_QUIT:
            gw.quit_flag = true;
            break;
        default:
            break;
    }
}

void golos_loop( void ) {
    // input code
}

void golos_render( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor3f( 0.0f, 1.0f, 0.0f );
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -4.0f );
    glRotatef( -45.0f, 1.0f, 0.0f, 0.0f );
    glPolygonMode( GL_BACK, GL_POINT );
    glPolygonMode( GL_FRONT, GL_LINE );
    glColor3f( 0.0f, 1.0f, 0.0f );
    gluSphere( sphere, 1.0f, 16, 16 );
    glFlush();
    SDL_GL_SwapWindow( gw.window );
}

void golos_destroy( void ) {
    gluDeleteQuadric( sphere );
    SDL_DestroyWindow( gw.window );
    SDL_Quit();
}

int main( int argc, char ** argv ) {
    float current = 0.0f, last = 0.0f;

    golos_init();
    while ( !gw.quit_flag ) {
        current = (float) SDL_GetTicks();
        if ( current > last + 1000.0f / 60.0f ) {
            golos_event();
            golos_loop();
            golos_render();
            last = current;
        }
    }
    golos_destroy();
    return EXIT_SUCCESS;
}