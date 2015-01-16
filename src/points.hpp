#pragma once

// Описание структур для точек на экране
// точек на поверхности и ячеек

// viewport point
struct viewPoint {
    int x, y;
    viewPoint( surfPoint );
};

// surface point
struct surfPoint {
    int theta, phi;
    surfPoint( viewPoint );
};

// cell
struct cell {
    int i, j;
    bool alive;
    cell( surfPoint );
};
