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
vec3s sunPos = {20, M_PI / 2, M_PI / 2};
vec3s moonPos = {6, M_PI / 2, M_PI / 3};

float smRatio = 2.0;
float dtheta = 0.01;
float dphi = 0.01;

ShaderProgram * sunShader, *earthShader, *moonShader;

gSphere sphere( 30, 60 );
gFont font;

int rows = 30;
int cols = 60;
GLubyte * cells;
Field * f;

WindowManager window( "Game Of Life On Sphere" );
const char outputStr[] = "[%s] fps: %.2f; ϑ: %.2f; φ: %.2f; задержка %d";
const char * gameStatus[] = {(const char *) "пауза",
                             ( const char * ) "симуляция"};
bool gameStep = false;
uint8_t MAX_COUNT = 5;

template <typename T> inline T sqr( const T & i ) { return ( i ) * ( i ); }

glm::vec3 GetOGLPos( int x, int y ) {
    GLint viewport[ 4 ];
    GLdouble modelview[ 16 ];
    GLdouble projection[ 16 ];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float) x;
    winY = (float) viewport[ 3 ] - (float) y;
    glReadPixels( x, int( winY ), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX,
                  &posY, &posZ );

    return glm::vec3( posX, posY, posZ );
}

void setCell( int x, int y, bool createFlag ) {
    auto p = GetOGLPos( x, y );
    if ( length( p ) <= 1.01f ) { // это радиус Земли
        p = normalize( p );
        vec3s point;
        point.r = 1.0;
        point.theta = acos( p.z );
        point.phi = atan2( p.y, p.x );

        if ( createFlag ) {
            f->create( point );
        } else {
            f->toggle( point );
        }
    }
}

glm::mat4 setProjection( float fieldOfView, float nearCut, float farCut ) {
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( fieldOfView,
                    (float) window.getWidth() / (float) window.getHeight(),
                    nearCut, farCut );
    glMatrixMode( GL_MODELVIEW );
    return glm::perspective( fieldOfView, (float) window.getWidth() /
                                              (float) window.getHeight(),
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

void golosInit( void ) {
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

    f = new Field( rows, cols );
    // рандомная инициализация
    f->randomFill();
    cells = new GLubyte[ rows * cols ];

    font.load( "./data/FiraSans-Medium.ttf", 16 );

    GLuint moonTextureId;
    GLuint moonNormalsId;
    GLuint earthTextureId;

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

void golosEvent( SDL_Event * event ) {
    static uint8_t buttonLeftSet = false;
    static uint8_t buttonRightSet = false;
    static uint16_t mouseX = 0, mouseY = 0;
    SDL_PollEvent( event );
    switch ( event->type ) {
    case SDL_QUIT:
        window.killWindow();
        break;
    case SDL_KEYDOWN:
        switch ( event->key.keysym.sym ) {
        case SDLK_SPACE:
            gameStep = !gameStep;
            break;
        case SDLK_ESCAPE:
        case SDLK_q:
            window.killWindow();
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
            f->clear();
            f->randomFill();
            break;
        case SDLK_r:
            f->clear();
            break;
        default:
            break;
        }
        break;
    case SDL_MOUSEMOTION:
        if ( buttonLeftSet ) {
            camera.rotate( -( event->button.y - mouseY ) / 100.0f,
                           -( event->button.x - mouseX ) / 100.0f );
            mouseX = event->button.x;
            mouseY = event->button.y;
        }
        if ( buttonRightSet ) {
            setCell( event->button.x, event->button.y, true );
        }
        break;
    case SDL_MOUSEWHEEL:
        camera.r -= event->wheel.y * camera.r / 60.0;
        event->wheel.y = 0;
        break;
    case SDL_MOUSEBUTTONDOWN:
        switch ( event->button.button ) {
        case SDL_BUTTON_LEFT:
            if ( !buttonLeftSet ) {
                mouseX = event->button.x;
                mouseY = event->button.y;
            }
            buttonLeftSet = true;
            break;
        case SDL_BUTTON_RIGHT:
            if ( !buttonRightSet ) {
                setCell( event->button.x, event->button.y, false );
            }
            buttonRightSet = true;
            break;
        default:
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        buttonLeftSet = buttonRightSet = false;
        break;
    default:
        break;
    }
}

void golosLoop( void ) {
    static int counter = 0;
    // блок управления автоматической генерации нового поколения
    // со скоростью MAX_COUNT
    if ( gameStep && counter >= MAX_COUNT ) {
        f->nextGeneration();
        counter = 0;
    } else {
        counter++;
    }

    sunPos.rotate( 0, dphi / 3 );
    moonPos.rotate( 0, dphi / 2 );

    vec3d rectSun = vec3d( sunPos );
    float lightPosition[ 4 ] = {rectSun.x, rectSun.y, rectSun.z, 1};

    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    // формируем текстуру
    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            cells[ i * cols + j ] = ( ( *f )[ i ][ j ] ) ? 0xff : 0x00;

    glActiveTexture( GL_TEXTURE0 );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, cols, rows, 0, GL_RED,
                  GL_UNSIGNED_BYTE, cells );
}

void golosRender( void ) {
    golosLoop();

    vec3d rectSun = vec3d( sunPos );
    vec3d rectMoon = vec3d( moonPos );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setView( camera, {0, 0, 0} );
    setProjection( 60.0f, 1.0f, 100.0f );

    sunShader->run();
    sphere.draw( 2.0f, rectSun );
    sunShader->stop();

    moonShader->run();
    moonShader->uniform( "surfaceMap", 1 );
    moonShader->uniform( "normalMap", 2 );
    sphere.draw( 0.3f, rectMoon );
    moonShader->stop();

    earthShader->run();
    sphere.draw( 1.0f );
    earthShader->stop();

    glPushMatrix();
    glLoadIdentity();
    glColor3f( 1.0f, 1.0f, 1.0f );
    font.drawUTF( 10, 10, outputStr, gameStatus[ int( gameStep ) ],
                  window.getFPS(), camera.theta, camera.phi, MAX_COUNT );
    glPopMatrix();

    glFlush();
}

void golosDestroy( void ) { delete[] cells; }

int main( int argc, char ** argv ) {
    window.setInitFunc( golosInit );
    window.setRenderFunc( golosRender );
    window.setEventFunc( golosEvent );
    window.mainLoop();
    golosDestroy();
    return EXIT_SUCCESS;
}
