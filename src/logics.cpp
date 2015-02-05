#include "logics.hpp"

// получаем контур на поверхности сферы
std::vector<vec3s> cell_contour( cell c, field f, std::size_t npoints ){
    // пока не обрабатываем ячейки у полюсов, хотя там одна из сторон просто
    // вырождается в точку и все точки на этой стороне совпадают
    std::vector<vec3s> contour(npoints);
    std::size_t pos = npoints / 4; // points on side
    std::size_t i = 0;
    // обход из правого нижнего угла против часовой стрелки
    // по меридиану
    for ( ; i < pos; ++i ){
        contour[i].theta = ( c.i + 1 - ( float ) i / pos ) * M_PI / f.height;
        contour[i].phi = ( c.j + 1 ) * 2 * M_PI / f.width;
    }
    // по широте
    for ( ; i < 2 * pos; ++i ){
        contour[i].theta = c.i * M_PI / f.height;
        contour[i].phi = ( c.j + 1 - ( float ) ( i - pos ) / pos )
                         * 2 * M_PI / f.width;
    }
    // снова по меридиану
    for ( ; i < 3 * pos; ++i ){
        contour[i].theta = ( c.i + ( float ) ( i - 2 * pos ) / pos )
                           * M_PI / f.height;
        contour[i].phi = c.j * 2 * M_PI / f.width;
    }
    // и ещё раз по широте
    for ( ; i < npoints; ++i) {
        auto last = npoints - 3 * pos;
        contour[i].theta = ( c.i + 1 ) * M_PI / f.height;
        contour[i].phi = ( c.j + ( float ) ( i - 3 * pos ) / last )
                         * 2 * M_PI / f.width;
    }
    return contour;
}

cell cell_from_point( field & f, vec3s & p ){
    return { f,
             ( int )( p.theta / M_PI   * f.height ),
             ( int )( p.phi / 2 / M_PI * f.width  )};
}

void create( field & f, vec3s & p ){
    auto c = cell_from_point(f, p);
    f[c.i][c.j] = true;
}

void kill( field & f, vec3s & p ){
    auto c = cell_from_point(f, p);
    f[c.i][c.j] = false;
}

void toggle( field & f, vec3s & p ){
    auto c = cell_from_point(f, p);
    f[c.i][c.j] = !f[c.i][c.j];
}


