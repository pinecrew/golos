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

vec3s camera = {30, M_PI/4, 0}; // положение камеры в сферических координатах
vec3s earth_pos = {18, M_PI/2, M_PI/2};
vec3s venus_pos = {12, M_PI/2, M_PI/2};

float dtheta = 0.01;
float dphi = 0.01;

GLuint program;
gSphere earth( 1.0f, 30, 60 );
gSphere venus( 1.0f, 30, 60 );
gSphere sun( 2.0f, 30, 60 );
gFont font;

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


    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    program = glCreateProgram();
    auto shader = compileShader( "./shaders/fragment.glsl", GL_FRAGMENT_SHADER );
    glAttachShader( program, shader );
    shader = compileShader( "./shaders/vertex.glsl", GL_VERTEX_SHADER );
    glAttachShader( program, shader );
    glLinkProgram( program );
    printInfoLog( program );

    f = new field( rows, cols );
    // рандомная инициализация
    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            (*f)[i][j] = rand() % 2;
    cells = new GLubyte[rows * cols];

    font.load( "./data/OpenSansLight.ttf", 16 );
}

void golos_event( SDL_Event * event ) {
    static uint8_t button_left_set = false;
    static uint8_t button_right_set = false;
    static uint16_t mouse_x = 0, mouse_y = 0;
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
        case SDL_MOUSEMOTION:
            if ( button_left_set ) {
                camera.rotate( -( event->button.y - mouse_y ) / 100.0f,
                               -( event->button.x - mouse_x ) / 100.0f );
                mouse_x = event->button.x;
                mouse_y = event->button.y;
            }
            break;
        case SDL_MOUSEWHEEL:
                camera.r += event->wheel.y * camera.r / 60.0;
                event->wheel.y = 0;
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch ( event->button.button ) {
                case SDL_BUTTON_LEFT:
                    if ( !button_left_set ) {
                        mouse_x = event->button.x;
                        mouse_y = event->button.y;
                    }
                    button_left_set = true;
                    break;
                case SDL_BUTTON_RIGHT:
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

void golos_render( void ) {
    vec3d rect_camera = vec3d(camera); // положение камеры в прямоугольных координатах
    vec3d rect_earth = vec3d(earth_pos);
    vec3d rect_venus = vec3d(venus_pos);
    earth_pos.rotate(0, dphi / 3);
    venus_pos.rotate(0, 1.8 * dphi / 3);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();

    float up = (camera.theta < 0) ? -1.0 : 1.0; // фикс для gluLookAt
    gluLookAt( rect_earth.x + rect_camera.x,
               rect_earth.y + rect_camera.y,
               rect_earth.z + rect_camera.z,
               rect_earth.x,
               rect_earth.y,
               rect_earth.z,
               0, 0, up );

    // рисуем солнышко (потом добавлю шейдер)
    glColor3f(1.0, 1.0, 1.0);
    sun.draw();
    float light_position[4] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // рисуем венеру
    glPushMatrix();
        glTranslatef(rect_venus.x, rect_venus.y, rect_venus.z);
        glColor3f(0.2, 0.3, 0.9); // не работает !!!
        venus.draw();
    glPopMatrix();


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

    // врубаем шейдеры
    glUseProgram( program );
    // рисуем Землю
    glPushMatrix();
        glTranslatef(rect_earth.x, rect_earth.y, rect_earth.z);
        earth.draw();
    glPopMatrix();
    glUseProgram( 0 );
    // для нормального отображения текста
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glPushMatrix();
    glLoadIdentity();
    glColor3f( 1.0f, 1.0f, 1.0f );
    font.draw( 10, 10, "FPS: %.2f", window.GetFPS() );
    glPopMatrix();

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
