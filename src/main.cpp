#include <GL/gl.h>
#include <GL/glu.h>
#include "math.hpp"
#include "window.hpp"
#include "vectors.hpp"

vec3s camera = {3,0,0}; // положение камеры в сферических координатах
float dtheta = 0.012;
float dphi = 0.01;
WindowManager window( "Game Of Life On fieldace" );

vec3d F( float u, float v, float r ) {
    return { cosf( u ) * sinf ( v ) * r, cosf( v ) * r, sinf( u ) * sinf( v ) * r };
}

void draw_quad( vec3d p0, vec3d p1, vec3d p2, vec3d p3 ) {
    glVertex3f( p0.x, p0.y, p0.z );
    glVertex3f( p1.x, p1.y, p1.z );
    glVertex3f( p2.x, p2.y, p2.z );
    glVertex3f( p3.x, p3.y, p3.z );
}

void draw_sphere( int UResolution, int VResolution, float radius ) {
    const float startU = 0.0f;
    const float startV = 0.0f;
    const float endU = M_2PI;
    const float endV = M_PI;
    const float stepU = ( endU - startU ) / (float) UResolution;
    const float stepV = ( endV - startV ) / (float) VResolution;
    glBegin( GL_QUADS );
    for ( int i = 0; i < UResolution; i++ ) {
        for ( int j = 0; j < VResolution; j++ ) {
            float u = (float) i * stepU + startU;
            float v = (float) j * stepV + startV;
            float un = (float) ( i + 1 ) * stepU + startU;
            float vn = (float) ( j + 1 ) * stepV + startV;
            vec3d p0 = F( u, v, radius );
            vec3d p1 = F( u, vn, radius );
            vec3d p2 = F( un, v, radius );
            vec3d p3 = F( un, vn, radius );
            draw_quad( p0, p2, p3, p1 );
        }
    }
    glEnd();
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
    gluPerspective( 45.0f, (float) window.GetWidth() / (float) window.GetHeight(), 0.1f, 100.0f );
    glMatrixMode( GL_MODELVIEW );
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
    vec3d rect_camera = vec3d(camera); // положение камеры в прямоугольных координатах
	camera.theta += dtheta;
	camera.phi += dphi;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor3f( 0.0f, 1.0f, 0.0f );
    glLoadIdentity();
	gluLookAt( rect_camera.x, rect_camera.y, rect_camera.z, 0,  0, 0, 0, 1, 0 );
    glRotatef( -45.0f, 1.0f, 0.0f, 0.0f );
    glPolygonMode( GL_BACK, GL_POINT );
    glPolygonMode( GL_FRONT, GL_LINE );
    glColor3f( 0.0f, 1.0f, 0.0f );
    draw_sphere( 16, 16, 1.0f );
    glFlush();
}

void golos_destroy( void ) {
    // insert code here
}

int main( int argc, char ** argv ) {
    window.SetInitFunc( golos_init );
    window.SetRenderFunc( golos_render );
    window.SetEventFunc( golos_event );
    window.MainLoop();
    golos_destroy();
    return EXIT_SUCCESS;
}
