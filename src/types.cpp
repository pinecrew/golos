#include <cmath>
#include <iostream>
#include "types.hpp"

field::field(std::size_t h, std::size_t w) : f(h) {
    this->width = w;
    this->height = h;
    for (std::size_t i = 0; i < h; ++i)
        this->f[i].assign(w, false);
}

std::vector< bool > & field::operator[](std::size_t i){
    return this->f[i];
}

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

SDL_Point surf_to_screen( vec3s n, vec3s sp, SDL_Point center ) {
    // координаты ортов в плоскости экрана в декартовых координатах
    vec3s ex = { 1, ( float ) ( M_PI / 2 ), n.phi + ( float ) ( M_PI / 2 ) };
    vec3s ey = { 1, n.theta - ( float ) ( M_PI / 2 ), n.phi };

    // немного скалярных произведений
    return { center.x + ( int ) ( sp * ex ),
             center.y - ( int ) ( sp * ey ) };
}

bool visible ( vec3s n, vec3s sp ) {
    return ( n * sp >= 0 ); // хак для увеличения области видимости
}

