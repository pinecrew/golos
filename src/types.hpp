#pragma once
#include <vector>

// Описание структур для точек на экране
// точек на поверхности и ячеек

// screen point
struct screenPoint {
    int x, y;
    //viewPoint( surfPoint );
};

// surface point
struct surfPoint {
    int theta, phi;
    //surfPoint( viewPoint );
};

// cell
struct cell {
    int i, j;
    //bool alive;
    //cell( surfPoint );
};

struct field {
    std::size_t width, height;
    std::vector< std::vector< bool > > f;
    std::vector< bool > operator[](std::size_t i);
    field(std::size_t h, std::size_t w);
};
