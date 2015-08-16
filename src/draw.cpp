#include "draw.hpp"

// vec3s screen_to_field( ) {
    // TODO
// }

gSphere::gSphere( const gSphere & sphere ) {
    max_count = sphere.max_count;
    size_t copy_count = max_count * coords_count * vertex_count;
    vertex = new float [ copy_count ];
    std::memcpy( (void *) vertex, (void *) sphere.vertex, copy_count );
}

gSphere::gSphere( float radius, std::size_t UResolution, std::size_t VResolution ) {
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

void gSphere::draw() {
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertex );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, max_count );
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
