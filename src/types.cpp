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

screenPoint surf_to_screen( svec2 n, surfPoint sp, screenPoint center, float R ) {
    // координаты нормали к плоскости проекции в декартовых координатах
    float nx = sin( n.theta ) * cos( n.phi );
    float ny = sin( n.theta ) * sin( n.phi );
    float nz = cos( n.theta );

    // координаты проекции точки на плоскость проекции в декартовых координатах
    float px = R * sin( sp.theta ) * cos( sp.phi ) * ( 1 - nx );
    float py = R * sin( sp.theta ) * sin( sp.phi ) * ( 1 - ny );
    float pz = R * cos( sp.theta ) * ( 1 - nz );

    // координаты ортов в плоскости экрана в декартовых координатах
    float xx = cos( n.theta ) * cos( n.phi );
    float xy = cos( n.theta ) * sin( n.phi );
    float xz = -sin( n.theta );

    float yx = -sin( n.phi );
    float yy = cos( n.phi );
    float yz = 0;

    // немного скалярных произведений
    return { center.x + ( int ) ( xx * px + xy * py + xz * pz ),
             center.y + ( int ) ( yx * px + yy * py + yz * pz ) };
}
