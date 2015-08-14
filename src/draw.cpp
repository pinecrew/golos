#include <algorithm>
#include "draw.hpp"

// vec3s screen_to_field( ) {
    // TODO
// }

gSphere::gSphere( float radius, std::size_t UResolution, std::size_t VResolution ) {
    const float startU = 0.0f;
    const float startV = 0.0f;
    const float endU = M_PI;
    const float endV = M_2PI;
    const float stepU = ( endU - startU ) / (float) UResolution;
    const float stepV = ( endV - startV ) / (float) VResolution;
    max_count = UResolution * VResolution * 6;
    vertex = new float [ max_count * 3 ];
    std::size_t count = 0;
    for ( std::size_t i = 0; i < UResolution; i++ ) {
        for ( std::size_t j = 0; j < VResolution; j++ ) {
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

void gSphere::insert_vec3d( std::size_t index, vec3d v ) {
    vertex[index+0] = v.x;
    vertex[index+1] = v.y;
    vertex[index+2] = v.z;
}
