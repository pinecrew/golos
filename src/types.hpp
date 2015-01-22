#pragma once
#include <vector>

// Описание структур для точек на экране
// точек на поверхности и ячеек

// viewport point
//struct viewPoint {
    //int x, y;
    //viewPoint( surfPoint );
//};

// surface point
//struct surfPoint {
    //int theta, phi;
    //surfPoint( viewPoint );
//};

// cell
//struct cell {
    //int i, j;
    //bool alive;
    //cell( surfPoint );
//};

struct field {
    unsigned int width, height;
    std::vector< std::vector< bool > > f;
    std::vector< bool > operator[](unsigned int i);
    field(unsigned int w, unsigned int h);
};
