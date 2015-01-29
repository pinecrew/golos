#include <cmath>
#include "types.hpp"

field::field(std::size_t h, std::size_t w){
    this->width = w;
    this->height = h;
    this->f = std::vector< std::vector < bool > >(h);
    for (std::size_t i = 0; i < h; ++i)
        this->f[i].assign(w, false);
}

std::vector< bool > field::operator[](std::size_t i){
    return this->f[i];
}

SDL_Point surf_to_screen( svec2 n, surfPoint sp, SDL_Point center, float R ) {
    // координаты нормали к плоскости проекции в декартовых координатах
    //float nx = sin( n.theta ) * cos( n.phi );
    //float ny = sin( n.theta ) * sin( n.phi );
    //float nz = cos( n.theta );

    // координаты проекции точки на плоскость проекции в декартовых координатах
    float px = R * sin( sp.theta ) * cos( sp.phi );
    float py = R * sin( sp.theta ) * sin( sp.phi );
    float pz = R * cos( sp.theta );

    // координаты ортов в плоскости экрана в декартовых координатах
    float yx = cos( n.theta ) * cos( n.phi );
    float yy = cos( n.theta ) * sin( n.phi );
    float yz = -sin( n.theta );

    float xx = -sin( n.phi );
    float xy = cos( n.phi );
    float xz = 0;

    // немного скалярных произведений
    return { center.x + ( int ) ( xx * px + xy * py + xz * pz ),
             center.y + ( int ) ( yx * px + yy * py + yz * pz ) };
}
