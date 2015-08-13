#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include "draw.hpp"
#include "math.hpp"
#include "vectors.hpp"
#include "window.hpp"

vec3s camera = {3,0,0}; // положение камеры в сферических координатах
float dtheta = 0.01;
float dphi = 0.01;
gSphere sphere( 1.0f, 16, 32 );
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
                case SDLK_q:
                    window.KillWindow();
                    break;
                case SDLK_DOWN:
                    camera.theta += dtheta;
                    if (camera.theta > M_PI) {
                        camera.phi += M_PI;
                        camera.theta -= M_PI;
                    }
                    break;
                case SDLK_UP:
                    camera.theta -= dtheta;
                    if (camera.theta < 0) {
                        camera.phi -= M_PI;
                        camera.theta += M_PI;
                    }
                    break;
                case SDLK_RIGHT:
                    camera.phi += dphi;
                    break;
                case SDLK_LEFT:
                    camera.phi -= dphi;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    //auto rc = vec3d(camera);
    //std::cout << "( " << camera.theta << ", " << camera.phi << " ) -> (" <<
    //"( " << rc.x << ", " << rc.y << ", " << rc.z << " )" << std::endl;
}

void golos_render( void ) {
    vec3d rect_camera = vec3d(camera); // положение камеры в прямоугольных координатах

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor3f( 0.0f, 1.0f, 0.0f );
    glLoadIdentity();
    gluLookAt( rect_camera.x, rect_camera.y, rect_camera.z, 0,  0, 0, 0, 0, 1 );
    glPolygonMode( GL_BACK, GL_POINT );
    glPolygonMode( GL_FRONT, GL_LINE );
    glColor3f( 0.0f, 1.0f, 0.0f );
    sphere.draw();
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
