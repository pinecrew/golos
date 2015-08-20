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

vec3s camera = {10, M_PI/4, 0}; // положение камеры в сферических координатах
vec3s sun_pos = {30, M_PI/2, M_PI/2};
vec3s moon_pos = {9, M_PI/2, M_PI/2};

float smRatio = 2.0;
float dtheta = 0.01;
float dphi = 0.01;

GLuint moonTextureId;
GLuint moonNormalsId;
GLuint earthTextureId;
GLuint fboId;
GLuint depthTextureId;

ShaderProgram *sunShader, *earthShader, *moonShader;

gSphere sphere( 30, 60 );
gFont font;

int rows = 30;
int cols = 60;
GLubyte* cells;
field* f;

WindowManager window( "Game Of Life On fieldace" );
const char output_str[] ="[%s] fps: %.2f; theta: %.2f; phi: %.2f; delay %d";
const char * game_status[] = {
    (const char *) "pause",
    (const char *) "play"
};
bool game_step = false;
uint8_t MAX_COUNT = 5;

template< typename T > inline T sqr( const T & i ) { return ( i ) * ( i ); } 

void set_cell( int x, int y, bool create_flag ) {
    SDL_Point center = { window.GetWidth() / 2, window.GetHeight() / 2 };
    SDL_Point mouse = { x, y };
    // if ( sqr( x - center.x ) + sqr( y - center.y ) < sqr( 1.0f ) ) {
    //     vec3s point = screen_to_field( mouse, view_direction, center );
    //     if ( create_flag ) {
    //         f->create( point );
    //     } else {
    //         f->toggle( point );
    //     }
    // }
}

void generateShadowFBO() {
  int shadowMapWidth = window.GetWidth() * smRatio;
  int shadowMapHeight = window.GetHeight() * smRatio;

  GLenum FBOstatus;

  // Try to use a texture depth component
  glGenTextures(1, &depthTextureId);
  glBindTexture(GL_TEXTURE_2D, depthTextureId);

  // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Remove artifact on the edges of the shadowmap
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

  // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // create a framebuffer object
  glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // Instruct openGL that we won't bind a color texture with the currently bound FBO
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  // attach the texture to FBO depth attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthTextureId, 0);

  // check FBO status
  FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(FBOstatus != GL_FRAMEBUFFER_COMPLETE)
      printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");

  // switch back to window-system-provided framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setTextureMatrix(void) {
    static double modelView[16];
    static double projection[16];

    // Moving from unit cube [-1,1] to [0,1]
    const GLdouble bias[16] = {
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0};

    // Grab modelview and transformation matrices
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);


    glMatrixMode(GL_TEXTURE);
    glActiveTextureARB(GL_TEXTURE7);

    glLoadIdentity();
    glLoadMatrixd(bias);

    // concatating all matrices into one.
    glMultMatrixd (projection);
    glMultMatrixd (modelView);

    // Go back to normal matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void setupMatrices(vec3s position, vec3s lookAt) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective( 60.0f, (float) window.GetWidth() / (float) window.GetHeight(), 0.1f, 100.0f );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    auto rPos = vec3d(position);
    auto rLook = vec3d(lookAt);
    float up = (camera.theta < 0) ? -1.0 : 1.0; // фикс для gluLookAt
	gluLookAt(rPos.x,rPos.y,rPos.z,rLook.x,rLook.y,rLook.z,0,0,up);
}



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
    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            (*f)[i][j] = rand() % 2;
    cells = new GLubyte[rows * cols];

    font.load( "./data/FiraSans-Medium.ttf", 16 );

    gLoadImage( "./data/moon.png", moonTextureId );
    gLoadImage( "./data/moon_normalmap.jpg", moonNormalsId );
    glGenTextures(1, &earthTextureId);

    generateShadowFBO();
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
    vec3d rect_sun = vec3d(sun_pos);
    vec3d rect_moon = vec3d(moon_pos);
    sun_pos.rotate(0, dphi / 3);
    moon_pos.rotate(0, 5 * dphi / 3);

    glEnable( GL_CULL_FACE );
	//First step: Render from the light POV to a FBO, story depth values only
    glBindFramebuffer(GL_FRAMEBUFFER,fboId);	//Rendering offscreen
	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
    glViewport(0,0, window.GetWidth() * smRatio, window.GetHeight() * smRatio);
    // Clear previous frame values
    glClear( GL_DEPTH_BUFFER_BIT);
    //Disable color rendering, we only want to write to the Z-Buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    setupMatrices(sun_pos, {0,0,0});
    // Culling switching, rendering only backface, this is done to avoid self-shadowing
    glCullFace( GL_FRONT );
    sphere.draw( 0.3f, rect_moon );
    sphere.draw( 1.0f );
    //Save modelview/projection matrice into texture7, also add a biais
    setTextureMatrix();
    // Now rendering from the camera POV, using the FBO to generate shadows
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,window.GetWidth(), window.GetHeight());

    //Enabling color write (previously disabled for light POV z-buffer rendering)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Clear previous frame values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace( GL_BACK );
    setupMatrices(camera, {0, 0, 0});

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);

    //// нужно выключать использование текстур для обычной отрисовки
    //glDisable( GL_TEXTURE_2D );

    float light_position[4] = {rect_sun.x, rect_sun.y, rect_sun.z, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    // рисуем солнышко (потом добавлю шейдер)
    sunShader->run();
    sphere.draw( 2.0f, rect_sun );
    sunShader->stop();

    // рисуем венеру
    glEnable( GL_TEXTURE_2D );
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, moonTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, moonNormalsId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );



    moonShader->run();
    moonShader->uniform1i("surface", 0);
    moonShader->uniform1i("normals", 1);
    moonShader->uniform1i("shadowMap", 7);
    sphere.draw( 0.3f, rect_moon );
    moonShader->stop();

    // формируем текстуру

    for ( int i = 0; i < rows; ++i )
        for ( int j = 0; j < cols; ++j )
            cells[i * cols + j] = ( (*f)[i][j] ) ? 0xff : 0x00;

    // отдаём текстуру
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, earthTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, cols, rows, 0, GL_RED , GL_UNSIGNED_BYTE, cells );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    // врубаем шейдеры
    earthShader->run();
    // рисуем Землю
    earthShader->uniform1i("shadowMap", 7);
    sphere.draw( 1.0f );
    earthShader->stop();

    // для нормального отображения текста
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
    glDisable( GL_CULL_FACE );
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
