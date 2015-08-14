#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include "draw.hpp"
#include "math.hpp"
#include "vectors.hpp"
#include "window.hpp"
#include "shader.hpp"
#include "font.hpp"

vec3s camera = {3,M_PI/4,0}; // положение камеры в сферических координатах
float dtheta = 0.01;
float dphi = 0.01;
GLuint program;
gSphere sphere( 1.0f, 30, 60 );
gFont font( "./data/OpenSansLight.ttf", 24 );

int rows = 30;
int cols = 60;
GLubyte* cells;
field* f;

WindowManager window( "Game Of Life On fieldace" );

void golos_init( void ) {
    // init OpenGL params
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0 );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glShadeModel( GL_SMOOTH );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.0f, (float) window.GetWidth() / (float) window.GetHeight(), 0.1f, 100.0f );
    glMatrixMode( GL_MODELVIEW );
    glewInit();
    program = glCreateProgram();
    auto shader = compileShader( "./shaders/fragment.glsl", GL_FRAGMENT_SHADER );
    addShader( program, shader );
    shader = compileShader("./shaders/vertex.glsl", GL_VERTEX_SHADER );
    addShader( program, shader );

    f = new field( rows, cols );
    // рандомная инициализация
    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            (*f)[i][j] = rand() % 2;
    cells = new GLubyte[rows * cols];
}

void golos_event( SDL_Event * event ) {
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            window.KillWindow();
            break;
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_SPACE:
                    f->nextGeneration();
                    break;
                case SDLK_ESCAPE:
                case SDLK_q:
                    window.KillWindow();
                    break;
                case SDLK_DOWN:
                    camera.rotate(dtheta, 0);
                    break;
                case SDLK_UP:
                    camera.rotate(-dtheta, 0);
                    break;
                case SDLK_RIGHT:
                    camera.rotate(0, dphi);
                    break;
                case SDLK_LEFT:
                    camera.rotate(0, -dphi);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void golos_render( void ) {
    vec3d rect_camera = vec3d(camera); // положение камеры в прямоугольных координатах

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    float up = (camera.theta < 0) ? -1.0 : 1.0; // фикс для gluLookAt
    gluLookAt( rect_camera.x, rect_camera.y, rect_camera.z, 0, 0, 0, 0, 0, up );

    // врубаем шейдеры
    glUseProgram( program );

    // формируем текстуру

    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            cells[i * cols + j] = ( (*f)[i][j] ) ? 0xff : 0x00;

    // отдаём текстуру
    glBindTexture( GL_TEXTURE_2D, 1 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, cols, rows, 0, GL_RED , GL_UNSIGNED_BYTE, cells );
    glEnable( GL_TEXTURE_2D );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    // рисуем сферу
    sphere.draw();

    // вырубаем шейдеры
    glUseProgram(0);

    glFlush();
}

void golos_destroy( void ) {
    delete[] cells;
}

int main( int argc, char ** argv ) {
    window.SetInitFunc( golos_init );
    window.SetRenderFunc( golos_render );
    window.SetEventFunc( golos_event );
    window.MainLoop();
    golos_destroy();
    return EXIT_SUCCESS;
}
