#include <cmath>
#include <iostream>
#include "types.hpp"

field::field(std::size_t h, std::size_t w) : f(h), width(w), height(h) {
    for (std::size_t i = 0; i < h; ++i)
        this->f[i].assign(w, false);
}

std::vector< bool > & field::operator[](std::size_t i){
    return this->f[i];
}

void field::nextGeneration() {
    // массив для подсчёта соседей
    auto tmp = new std::vector<char>[height];
    for (std::size_t i = 0; i < height; ++i)
        tmp[i].assign(width, 0);

    // 1. Обрабатываем первую строку
    //    в ней все соседи друг другу, так как есть общая вершина (полюс)
    //    поэтому проще всего посчитать сумму в этой строке
    std::size_t s = 0;
    for (std::size_t i = 0; i < width; ++i)
        s += f[0][i];

    for (std::size_t i = 0; i < width; ++i)
        if ( f[0][i] ){
            tmp[0][i] = s - 1;
            tmp[1][( i + width - 1 ) % width] += 1;
            tmp[1][i] += 1;
            tmp[1][( i + width + 1 ) % width] += 1;
        } else {
            tmp[0][i] = s;
        }

    // 2. Между первой и последней
    for (std::size_t j = 1; j < height - 1; ++j)
        for (std::size_t i = 0; i < width; ++i)
            if ( f[j][i] ){
                tmp[j - 1][( i + width - 1 ) % width] += 1;
                tmp[j - 1][i]                             += 1;
                tmp[j - 1][( i + width + 1 ) % width] += 1;

                tmp[j][( i + width - 1 ) % width] += 1;
                tmp[j][( i + width + 1 ) % width] += 1;

                tmp[j + 1][( i + width - 1 ) % width] += 1;
                tmp[j + 1][i]                             += 1;
                tmp[j + 1][( i + width + 1 ) % width] += 1;
            }

    // 3. Последняя аналогично первой
    std::size_t p = height - 1;
    s = 0;
    for (std::size_t i = 0; i < width; ++i)
        s += f[p][i];

    for (std::size_t i = 0; i < width; ++i)
        if ( f[p][i] ){
            tmp[p][i] += s - 1;

            tmp[p - 1][( i + width - 1 ) % width] += 1;
            tmp[p - 1][i] += 1;
            tmp[p - 1][( i + width + 1 ) % width] += 1;
        } else {
            tmp[p][i] += s;
        }

    // 4. Установка нового состояния
    for (std::size_t j = 0; j < height; ++j)
        for (std::size_t i = 0; i < width; ++i)
            f[j][i] = (tmp[j][i] == 2 && f[j][i]) || (tmp[j][i] == 3);

    delete[] tmp;
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

cell::cell( field & f, int _i, int _j ) : i(_i), j(_j) {
    int w = f.width, h = f.height;
    n = { 1,
          ( float ) M_PI * ( i + 0.5f ) / h,
          2.0f * ( float ) M_PI * ( j + 0.5f ) / w };
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

float _fmod( float x, float y ) {
    return fmod( fmod( x, y ) + y, y );
}

vec3s screen_to_surf( vec3s n, float r, SDL_Point p, SDL_Point c ) {
   float b1 = ( p.x - c.x ) / r,
         b2 = ( c.y - p.y ) / r,
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
