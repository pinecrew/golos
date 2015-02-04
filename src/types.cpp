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

vec3s screen_to_surf( vec3s n, float r, SDL_Point p, SDL_Point c ) {
   float b1 = ( p.x - c.x ) / r,
         b2 = ( c.y - p.y ) / r,
         a1 = cos( n.theta ),
         a2 = sin( n.theta );
   float d = a1 * sqrt( 1 - b1 * b1 - b2 * b2 );
   float ct1 = ( b2 * a2 + d ), ct2 = ( b2 * a2 - d );
   float st1 = sqrt( 1 - ct1 * ct1 ), st2 = ( 1 - ct2 * ct2 );
   float sp1 = b1 / st1, sp2 = b1 / st2;
   float cp1 = ( a2 * ct1 - b2 ) / a1 / st1, cp2 = ( a2 * ct2 - b2 ) / a1 / st2;
   vec3s s1 = {1, atan2f(st1, ct1), atan2f(sp1, cp1)};
   vec3s s2 = {1, atan2f(st2, ct2), atan2f(sp2, cp2)};
   vec3s s;
   if ( visible(n, s1) )
       s = s1;
   else
       s = s2;
   return s;
}
