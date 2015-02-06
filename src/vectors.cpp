#include <cmath>
#include "vectors.hpp"

vec3d::vec3d( const vec3s & v ) {
    float st, ct, sp, cp;
    sincosf(v.theta, &st, &ct);
    sincosf(v.phi, &sp, &cp);
    this->x = v.r * st * cp;
    this->y = v.r * st * sp;
    this->z = v.r * ct;
}

float vec3d::operator*( const vec3d & rhs ) {
    return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
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
