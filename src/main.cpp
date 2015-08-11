#include <GL/gl.h>
#include <GL/glu.h>
#include "window.hpp"

float camera[] = {3,0,0}; // положение камеры в сферических координатах
float dtheta = 0.012;
float dphi = 0.01;
GLUquadricObj * sphere;
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
    gluPerspective( 45.0f, (float) window.GetWidth() / (float) window.GetHeight(), 0.1f, 100.0f );
    glMatrixMode( GL_MODELVIEW );
    // init sphere
    sphere = gluNewQuadric();
}

void golos_event( SDL_Event * event ) {
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            window.KillWindow();
            break;
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_ESCAPE:
                    window.KillWindow();
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
    float rect_camera[3]; // положение камеры в прямоугольных координатах
	rect_camera[0] = camera[0] * sin(camera[1]) * cos(camera[2]);
	rect_camera[1] = camera[0] * sin(camera[1]) * sin(camera[2]);
	rect_camera[2] = camera[0] * cos(camera[1]);
	camera[1] += dtheta;
	camera[2] += dphi;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor3f( 0.0f, 1.0f, 0.0f );
    glLoadIdentity();
	gluLookAt(rect_camera[0], rect_camera[1], rect_camera[2], 0,  0, 0, 0, 1, 0);
    glTranslatef( 0.0f, 0.0f, 0.0f );
    glRotatef( -45.0f, 1.0f, 0.0f, 0.0f );
    glPolygonMode( GL_BACK, GL_POINT );
    glPolygonMode( GL_FRONT, GL_LINE );
    glColor3f( 0.0f, 1.0f, 0.0f );
    gluQuadricTexture(sphere, true);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere( sphere, 1.0f, 16, 16 );
    glFlush();
}

void golos_destroy( void ) {
    gluDeleteQuadric( sphere );
}

int main( int argc, char ** argv ) {
    window.SetInitFunc( golos_init );
    window.SetRenderFunc( golos_render );
    window.SetEventFunc( golos_event );
    window.MainLoop();
    golos_destroy();
    return EXIT_SUCCESS;
}
