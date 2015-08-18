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

GLuint venusTextureId;

ShaderProgram *sunShader, *earthShader, *venusShader;

gSphere sphere( 30, 60 );
gFont font;

int rows = 30;
int cols = 60;
GLubyte* cells;
field* f;

WindowManager window( "Game Of Life On fieldace" );
const char output_str[] ="[%s] fps: %.2f; theta: %.2f; phi: %.2f; delay %d";
const wchar_t * game_status[] = {
    (const wchar_t *) "pause",
    (const wchar_t *) "play"
};
bool game_step = false;
uint8_t MAX_COUNT = 5;

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

    // включаем отсечение лишнего
    glEnable( GL_CULL_FACE );
    // отрисовка против часовой стрелки
    glFrontFace( GL_CCW );
    // рисуем только переднюю нормаль
    glCullFace( GL_BACK );

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    earthShader = new ShaderProgram();
    earthShader->addShader( "./shaders/sphere.vert.glsl", GL_VERTEX_SHADER );
    earthShader->addShader( "./shaders/earth.frag.glsl", GL_FRAGMENT_SHADER );
    earthShader->link();

    venusShader = new ShaderProgram();
    venusShader->addShader( "./shaders/sphere.vert.glsl", GL_VERTEX_SHADER );
    venusShader->addShader( "./shaders/venus.frag.glsl", GL_FRAGMENT_SHADER );
    venusShader->link();

    sunShader = new ShaderProgram();
    sunShader->addShader( "./shaders/sun.frag.glsl", GL_FRAGMENT_SHADER );
    sunShader->link();

    f = new field( rows, cols );
    // рандомная инициализация
    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            (*f)[i][j] = rand() % 2;
    cells = new GLubyte[rows * cols];

    font.load( "./data/FiraSans-Medium.ttf", 16 );

    gLoadImage( "./data/venus.jpg", venusTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void random_fill( void ) {
    for ( std::size_t i = 0; i < ( f->width * f->height ) / 3; i++ ) {
        (*f)[rand()%f->height][rand()%f->width] = true;
    }
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
                    game_step = !game_step;
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
                    for ( std::size_t i = 0; i < f->height; i++ ) {
                        for ( std::size_t j = 0; j < f->width; j++ ) {
                            (*f)[i][j] = false;
                        }
                    }
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
                camera.r -= event->wheel.y * camera.r / 60.0;
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

void golos_loop( void ) {
    static int counter = 0;
    if ( game_step && counter >= MAX_COUNT ) {
        f->nextGeneration();
        counter = 0;
    } else {
        counter++;
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

    // нужно выключать использование текстур для обычной отрисовки
    glDisable( GL_TEXTURE_2D );

    float light_position[4] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    // рисуем солнышко (потом добавлю шейдер)
    sunShader->run();
    sphere.draw( 2.0f );
    sunShader->stop();

    // рисуем венеру
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, venusTextureId );

    venusShader->run();
    sphere.draw( 1.0f, rect_venus );
    venusShader->stop();

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
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    // врубаем шейдеры
    earthShader->run();
    // рисуем Землю
    sphere.draw( 1.0f, rect_earth );
    earthShader->stop();

    // для нормального отображения текста
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
    glPushMatrix();
    glLoadIdentity();
    glColor3f( 1.0f, 1.0f, 1.0f );
    font.draw( 10, 10, output_str, game_status[int(game_step)], window.GetFPS(),
        camera.theta, camera.phi, MAX_COUNT );
    glPopMatrix();

    golos_loop();

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
