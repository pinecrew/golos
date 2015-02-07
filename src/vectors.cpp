#include "math.hpp"
#include "vectors.hpp"

vec3d::vec3d( const vec3s & v ) {
    float st, ct, sp, cp;
    sincosf(v.theta, &st, &ct);
    sincosf(v.phi, &sp, &cp);
    x = v.r * st * cp;
    y = v.r * st * sp;
    z = v.r * ct;
}

float vec3d::operator*( const vec3d & rhs ) {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

float vec3s::operator*( const vec3s & rhs ) {
    return vec3d( *this ) * vec3d ( rhs );
}

vec3s & operator += ( vec3s & lhs, const vec3s & rhs ) {
    lhs.r += rhs.r;
    lhs.theta += rhs.theta;
    lhs.phi += rhs.phi;
    return lhs;
}

vec3s & operator -= ( vec3s & lhs, const vec3s & rhs ) {
    lhs.r -= rhs.r;
    lhs.theta -= rhs.theta;
    lhs.phi -= rhs.phi;
    return lhs;
}

void vec3s::rotate( float dtheta, float dphi ) {
    theta = _fmod( theta + dtheta + M_PI, 2 * M_PI ) - M_PI;
    phi = _fmod( phi + dphi, 2 * M_PI );
}
