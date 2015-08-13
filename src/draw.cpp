#include <algorithm>
#include "draw.hpp"
#include "field.hpp"

SDL_Point field_to_screen( vec3s sp, vec3s n, SDL_Point center ) {
    // координаты ортов в плоскости экрана в декартовых координатах
    vec3s ex = { 1, ( float ) ( M_PI / 2 ), n.phi + ( float ) ( M_PI / 2 ) };
    vec3s ey = { 1, n.theta - ( float ) ( M_PI / 2 ), n.phi };

    // немного скалярных произведений
    return { center.x + ( int ) ( sp * ex ),
             center.y - ( int ) ( sp * ey ) };
}

bool visible( vec3s n, vec3s sp ) {
    return ( n * sp >= 0 ); // хак для увеличения области видимости
}

vec3s screen_to_field( SDL_Point p, vec3s n, SDL_Point c, field & f ) {
   float b1 = ( p.x - c.x ) / f.r,
         b2 = ( c.y - p.y ) / f.r,
         a1 = cos( n.theta ),
         a2 = sin( n.theta );
   float d = a1 * sqrt( 1 - b1 * b1 - b2 * b2 );
   float ct1 = ( b2 * a2 + d ), ct2 = ( b2 * a2 - d );
   float st1 = sqrt( 1 - ct1 * ct1 ), st2 = ( 1 - ct2 * ct2 );
   float sdp1 = b1 / st1, sdp2 = b1 / st2;
   float cdp1 = ( a2 * ct1 - b2 ) / a1 / st1, cdp2 = ( a2 * ct2 - b2 ) / a1 / st2;
   float t1 = atan2f(st1, ct1),
         t2 = atan2f(st1, ct1),
         p1 = _fmod( n.phi + atan2f(sdp1, cdp1), M_2PI ),
         p2 = _fmod( n.phi + atan2f(sdp2, cdp2), M_2PI );
   vec3s s1 = { 1, t1, p1 };
   vec3s s2 = { 1, t2, p2 };
   vec3s s;
   if ( visible(n, s1) )
       s = s1;
   else
       s = s2;
   return s;
}

gSphere::gSphere( float radius, size_t UResolution, size_t VResolution ) {
    const float startU = 0.0f;
    const float startV = 0.0f;
    const float endU = M_PI;
    const float endV = M_2PI;
    const float stepU = ( endU - startU ) / (float) UResolution;
    const float stepV = ( endV - startV ) / (float) VResolution;
    max_count = UResolution * VResolution * 6;
    vertex = new float [ max_count * 3 ];
    size_t count = 0;
    for ( size_t i = 0; i < UResolution; i++ ) {
        for ( size_t j = 0; j < VResolution; j++ ) {
            float u = (float) i * stepU + startU;
            float v = (float) j * stepV + startV;
            float un = (float) ( i + 1 ) * stepU + startU;
            float vn = (float) ( j + 1 ) * stepV + startV;
            vec3d p0 = vec3d( radius, u, v, true );
            vec3d p1 = vec3d( radius, un, vn, true );
            insert_vec3d( count +  0, p0 );
            insert_vec3d( count +  3, vec3d( radius, un, v, true ) );
            insert_vec3d( count +  6, p1 );
            insert_vec3d( count +  9, p0 );
            insert_vec3d( count + 12, p1 );
            insert_vec3d( count + 15, vec3d( radius, u, vn, true ) );
            count += 18;
        }
    }
}

void gSphere::draw() {
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertex );
    glDrawArrays( GL_TRIANGLES, 0, max_count );
    glDisableClientState( GL_VERTEX_ARRAY ); 
}

gSphere::~gSphere() {
    delete[] vertex;
}

void gSphere::insert_vec3d( size_t index, vec3d v ) {
    vertex[index+0] = v.x;
    vertex[index+1] = v.y;
    vertex[index+2] = v.z;
}