#include "draw.hpp"

/*
vec3d field_to_screen( vec3s sp, vec3s n, SDL_Point center ) {
    // координаты ортов в плоскости экрана в декартовых координатах
    vec3s ex = { 1, ( float ) ( M_PI / 2 ), n.phi + ( float ) ( M_PI / 2 ) };
    vec3s ey = { 1, n.theta - ( float ) ( M_PI / 2 ), n.phi };

    // немного скалярных произведений
    return vec3d( center.x + ( int ) ( sp * ex ), center.y - ( int ) ( sp * ey ), 0 );
}

bool visible( vec3s n, vec3s sp ) {
    return ( n * sp >= 0 ); // хак для увеличения области видимости
}

vec3s screen_to_field( SDL_Point p, vec3s n, SDL_Point c, vec3s camera ) {
   float b1 = ( p.x - c.x ) / camera.r,
         b2 = ( c.y - p.y ) / camera.r,
         a1 = cos( n.theta ),
         a2 = sin( n.theta );
   float d = a1 * sqrt( 1 - b1 * b1 - b2 * b2 );
   float ct1 = ( b2 * a2 + d ), ct2 = ( b2 * a2 - d );
   float st1 = sqrt( 1 - ct1 * ct1 ), st2 = ( 1 - ct2 * ct2 );
   float sdp1 = b1 / st1, sdp2 = b1 / st2;
   float cdp1 = ( a2 * ct1 - b2 ) / a1 / st1, cdp2 = ( a2 * ct2 - b2 ) / a1 / st2;
   float t1 = atan2f(st1, ct1),
         t2 = atan2f(st1, ct1),
         p1 = _fmod( n.phi + atan2f(sdp1, cdp1), 2 * M_PI)  ,
         p2 = _fmod( n.phi + atan2f(sdp2, cdp2), 2 * M_PI );
   vec3s s1 = { 1, t1, p1 };
   vec3s s2 = { 1, t2, p2 };
   vec3s s;
   if ( visible(n, s1) )
       s = s1;
   else
       s = s2;
   return s;
}
*/

void gLoadImage( const char * filename, GLuint & tex ) {
    SDL_Surface* Surface = IMG_Load( filename );
    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_2D, tex );
    int Mode = GL_RGB;
    if ( Surface->format->BytesPerPixel == 4 ) {
        Mode = GL_RGBA;
    }
    glTexImage2D( GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels );
    SDL_FreeSurface( Surface );
}

gSphere::gSphere( const gSphere & sphere ) {
    max_count = sphere.max_count;
    size_t copy_count = max_count * coords_count * vertex_count;
    vertex = new float [ copy_count ];
    std::memcpy( (void *) vertex, (void *) sphere.vertex, copy_count );
}

gSphere::gSphere( std::size_t UResolution, std::size_t VResolution ) {
    const float radius = 1.0f;
    const float startU = 0.0f;
    const float startV = 0.0f;
    const float endU = M_PI;
    const float endV = M_2PI;
    const float stepU = ( endU - startU ) / (float) UResolution;
    const float stepV = ( endV - startV ) / (float) VResolution;
    max_count = UResolution * VResolution * vertex_count;
    vertex = new float [ max_count * coords_count ];
    std::size_t count = 0;
    for ( std::size_t i = 0; i < UResolution; i++ ) {
        for ( std::size_t j = 0; j < VResolution; j++ ) {
            float u = (float) i * stepU + startU;
            float v = (float) j * stepV + startV;
            float un = (float) ( i + 1 ) * stepU + startU;
            float vn = (float) ( j + 1 ) * stepV + startV;
            insert_vec3d( count + 0, vec3d( radius, un, v, true ) );
            insert_vec3d( count + 3, vec3d( radius, un, vn, true ) );
            insert_vec3d( count + 6, vec3d( radius, u, v, true ) );
            insert_vec3d( count + 9, vec3d( radius, u, vn, true ) );
            count += vertex_count * coords_count;
        }
    }
}

void gSphere::draw( float radius, const vec3d & pos ) {
    glPushMatrix();
    if ( pos.x != 0.0f && pos.y != 0.0f && pos.z != 0.0f ) {
        glTranslatef( pos.x, pos.y, pos.z );
    }
    std::size_t copy_count = max_count * coords_count * vertex_count;
    auto tmp = new float [ copy_count ];
    // осторожно, хак, иначе тени неправильно считаются
    for (std::size_t i = 0; i < copy_count; ++i) {
        tmp[i] = radius * vertex[i];
    }
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, tmp );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, max_count );
    glDisableClientState( GL_VERTEX_ARRAY );
    glPopMatrix();
    delete[] tmp;
}

gSphere::~gSphere() {
    delete[] vertex;
}

void gSphere::insert_vec3d( std::size_t index, vec3d v ) {
    vertex[index+0] = v.x;
    vertex[index+1] = v.y;
    vertex[index+2] = v.z;
}
