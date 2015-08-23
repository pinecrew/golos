#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "draw.hpp"
#include "math.hpp"
#include "vectors.hpp"
#include "window.hpp"
#include "shader.hpp"
#include "font.hpp"

vec3s camera = {8, M_PI / 4, 0}; // положение камеры в сферических координатах
vec3s sun_pos = {20, M_PI / 2, M_PI / 2};
vec3s moon_pos = {6, M_PI / 2, M_PI / 3};

float smRatio = 2.0;
float dtheta = 0.01;
float dphi = 0.01;

GLuint moonTextureId;
GLuint moonNormalsId;
GLuint earthTextureId;

ShaderProgram * sunShader, *earthShader, *moonShader;

gSphere sphere( 30, 60 );
gFont font;

int rows = 30;
int cols = 60;
GLubyte * cells;
field * f;

WindowManager window( "Game Of Life On fieldace" );
const char output_str[] = "[%s] fps: %.2f; theta: %.2f; phi: %.2f; delay %d";
const char * game_status[] = {(const char *) "pause", ( const char * ) "play"};
bool game_step = false;
uint8_t MAX_COUNT = 5;

template <typename T> inline T sqr( const T & i ) { return ( i ) * ( i ); }

void set_cell( int x, int y, bool create_flag ) {
    // todo: написать рабочий код установки ячейки на сфере
    // SDL_Point center = { window.GetWidth() / 2, window.GetHeight() / 2 };
    // SDL_Point mouse = { x, y };
    // if ( sqr( x - center.x ) + sqr( y - center.y ) < sqr( 1.0f ) ) {
    //     vec3s point = screen_to_field( mouse, view_direction, center );
    //     if ( create_flag ) {
    //         f->create( point );
    //     } else {
    //         f->toggle( point );
    //     }
    // }
}

glm::mat4 setProjection( float fieldOfView, float nearCut, float farCut ) {
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( fieldOfView,
                    (float) window.GetWidth() / (float) window.GetHeight(),
                    nearCut, farCut );
    glMatrixMode( GL_MODELVIEW );
    return glm::perspective( fieldOfView, (float) window.GetWidth() /
                                              (float) window.GetHeight(),
                             nearCut, farCut );
}

glm::mat4 setView( vec3s position, vec3s lookAt ) {
    glLoadIdentity();
    auto rPos = vec3d( position );
    auto rLook = vec3d( lookAt );
    float up = ( position.theta < 0 ) ? -1.0 : 1.0; // фикс для gluLookAt
    gluLookAt( rPos.x, rPos.y, rPos.z, rLook.x, rLook.y, rLook.z, 0, 0, up );
    return glm::lookAt( glm::vec3( rPos.x, rPos.y, rPos.z ),
                        glm::vec3( rLook.x, rLook.y, rLook.z ),
                        glm::vec3( 0, 0, up ) );
}

void random_fill( void ) {
    for ( std::size_t i = 0; i < ( f->width * f->height ) / 3; i++ ) {
        ( *f )[ rand() % f->height ][ rand() % f->width ] = true;
    }
}

void golos_init( void ) {
    // init OpenGL params
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0 );
    glDepthFunc( GL_LESS );
    glEnable( GL_DEPTH_TEST );
    glShadeModel( GL_SMOOTH );
    glewInit();

    glEnable( GL_COLOR_MATERIAL );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    earthShader = new ShaderProgram();
    earthShader->addShader( "./shaders/sphere.vert.glsl", GL_VERTEX_SHADER );
    earthShader->addShader( "./shaders/earth.frag.glsl", GL_FRAGMENT_SHADER );
    earthShader->link();

    moonShader = new ShaderProgram();
    moonShader->addShader( "./shaders/sphere.vert.glsl", GL_VERTEX_SHADER );
    moonShader->addShader( "./shaders/moon.frag.glsl", GL_FRAGMENT_SHADER );
    moonShader->link();

    sunShader = new ShaderProgram();
    sunShader->addShader( "./shaders/sun.frag.glsl", GL_FRAGMENT_SHADER );
    sunShader->link();

    f = new field( rows, cols );
    // рандомная инициализация
    random_fill();
    cells = new GLubyte[ rows * cols ];

    font.load( "./data/FiraSans-Medium.ttf", 16 );

    gLoadImage( "./data/moon.png", moonTextureId );
    gLoadImage( "./data/moon_normalmap.png", moonNormalsId );
    glGenTextures( 1, &earthTextureId );

    glEnable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, moonTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, moonNormalsId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, earthTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
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
            camera.rotate( dtheta, 0 );
            break;
        case SDLK_UP:
            camera.rotate( -dtheta, 0 );
            break;
        case SDLK_RIGHT:
            camera.rotate( 0, dphi );
            break;
        case SDLK_LEFT:
            camera.rotate( 0, -dphi );
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
                    ( *f )[ i ][ j ] = false;
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
        if ( button_right_set ) {
            set_cell( event->button.x, event->button.y, true );
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
            if ( button_right_set ) {
                set_cell( event->button.x, event->button.y, true );
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

void update() {
    sun_pos.rotate( 0, dphi / 3 );
    moon_pos.rotate( 0, dphi / 2 );

    vec3d rect_sun = vec3d( sun_pos );
    float light_position[ 4 ] = {rect_sun.x, rect_sun.y, rect_sun.z, 1};

    glLightfv( GL_LIGHT0, GL_POSITION, light_position );

    // формируем текстуру
    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            cells[ i * cols + j ] = ( ( *f )[ i ][ j ] ) ? 0xff : 0x00;

    glActiveTexture( GL_TEXTURE0 );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, cols, rows, 0, GL_RED,
                  GL_UNSIGNED_BYTE, cells );
};

void golos_loop( void ) {
    static int counter = 0;
    // блок управления автоматической генерации нового поколения
    // со скоростью MAX_COUNT
    if ( game_step && counter >= MAX_COUNT ) {
        f->nextGeneration();
        counter = 0;
    } else {
        counter++;
    }
}

void golos_render( void ) {

    update();

    vec3d rect_sun = vec3d( sun_pos );
    vec3d rect_moon = vec3d( moon_pos );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setView( camera, {0, 0, 0} );
    setProjection( 60.0f, 1.0f, 100.0f );

    sunShader->run();
    sphere.draw( 2.0f, rect_sun );
    sunShader->stop();

    moonShader->run();
    moonShader->uniform1i( "surfaceMap", 1 );
    moonShader->uniform1i( "normalMap", 2 );
    sphere.draw( 0.3f, rect_moon );
    moonShader->stop();

    earthShader->run();
    sphere.draw( 1.0f );
    earthShader->stop();

    glPushMatrix();
    glLoadIdentity();
    glColor3f( 1.0f, 1.0f, 1.0f );
    font.draw( 10, 10, output_str, game_status[ int( game_step ) ],
               window.GetFPS(), camera.theta, camera.phi, MAX_COUNT );
    glPopMatrix();

    golos_loop();

    glFlush();
}

void golos_destroy( void ) { delete[] cells; }

int main( int argc, char ** argv ) {
    window.SetInitFunc( golos_init );
    window.SetRenderFunc( golos_render );
    window.SetEventFunc( golos_event );
    window.MainLoop();
    golos_destroy();
    return EXIT_SUCCESS;
}
