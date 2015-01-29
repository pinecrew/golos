#pragma once
#include <SDL2/SDL.h>
#include <vector>

// Описание структур для точек на экране
// точек на поверхности и ячеек

// surface point
struct surfPoint {
    float theta, phi;
    //surfPoint( viewPoint );
};

// вектор в сферических координатах
struct svec2 {
    float theta, phi;
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

SDL_Point surf_to_screen( svec2 n, surfPoint sp, SDL_Point center, float R );
