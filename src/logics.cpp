#include "logics.hpp"

// получаем контур на поверхности сферы
void cell_contour( cell c, field f, std::size_t npoints, surfPoint* contour ){
    // пока не обрабатываем ячейки у полюсов
    std::size_t pos = npoints / 4; // points on side
    std::size_t i = 0;
    // обход из правого нижнего угла против часовой стрелки
    // по меридиану
    for ( ; i < pos; ++i ){
        contour[i].theta = ( c.i + 1 - ( float ) i / pos ) * M_PI / f.height;
        contour[i].phi = ( c.j + 1 ) * M_PI_2 / f.width;
    }
    // по широте
    for ( ; i < 2 * pos; ++i ){
        contour[i].theta = c.i * M_PI / f.height;
        contour[i].phi = ( c.j + 1 - ( float ) ( i - pos ) / pos )
                         * M_PI_2 / f.width;
    }
    // снова по меридиану
    for ( ; i < 3 * pos; ++i ){
        contour[i].theta = ( c.i + ( float ) ( i - 2 * pos ) / pos )
                           * M_PI / f.height;
        contour[i].phi = c.j * M_PI_2 / f.width;
    }
    // и ещё раз по широте
    for ( ; i < npoints; ++i) {
        contour[i].theta = ( c.i + 1 ) * M_PI / f.height;
        contour[i].phi = ( c.j + ( float ) ( i - 3 * pos ) / pos )
                         * M_PI_2 / f.width;
    }
}

cell cell_from_point( field & f, surfPoint & p ){
    return { ( int )( p.theta / M_PI   * f.height ),
             ( int )( p.phi   / M_PI_2 * f.width  )};
}

void create( field & f, surfPoint & p ){
    auto c = cell_from_point(f, p);
    f[c.i][c.j] = true;
}

void kill( field & f, surfPoint & p ){
    auto c = cell_from_point(f, p);
    f[c.i][c.j] = false;
}

void toggle( field & f, surfPoint & p ){
    auto c = cell_from_point(f, p);
    f[c.i][c.j] = !f[c.i][c.j];
}

void nextStep(field & f){
    // массив для подсчёта соседей
    auto tmp = new char*[f.height];
    for (std::size_t i = 0; i < f.height; ++i)
        tmp[i] = new char[f.width];

    // 1. Обрабатываем первую строку
    //    в ней все соседи друг другу, так как есть общая вершина (полюс)
    //    поэтому проще всего посчитать сумму в этой строке
    std::size_t s = 0;
    for (std::size_t i = 0; i < f.width; ++i)
        s += f[0][i];

    for (std::size_t i = 0; i < f.width; ++i)
        if ( f[0][i] ){
            tmp[0][i] = s - 1;
            tmp[1][( i + f.width - 1 ) % f.width] += 1;
            tmp[1][i] += 1;
            tmp[1][( i + f.width + 1 ) % f.width] += 1;
        } else {
            tmp[0][i] = s;
        }

    // 2. Между первой и последней
    for (std::size_t j = 1; j < f.height - 1; ++j)
        for (std::size_t i = 0; i < f.width; ++i)
            if ( f[j][i] ){
                tmp[j - 1][( i + f.width - 1 ) % f.width] += 1;
                tmp[j - 1][i]                             += 1;
                tmp[j - 1][( i + f.width + 1 ) % f.width] += 1;

                tmp[j][( i + f.width - 1 ) % f.width] += 1;
                tmp[j][i]                             += 1;
                tmp[j][( i + f.width + 1 ) % f.width] += 1;

                tmp[j + 1][( i + f.width - 1 ) % f.width] += 1;
                tmp[j + 1][i]                             += 1;
                tmp[j + 1][( i + f.width + 1 ) % f.width] += 1;
            }

    // 3. Последняя аналогично первой
    std::size_t p = f.height - 1;
    s = 0;
    for (std::size_t i = 0; i < f.width; ++i)
        s += f[p][i];

    for (std::size_t i = 0; i < f.width; ++i)
        if ( f[0][i] ){
            tmp[0][i] += s - 1;

            tmp[p - 1][( i + f.width - 1 ) % f.width] += 1;
            tmp[p - 1][i] += 1;
            tmp[p - 1][( i + f.width + 1 ) % f.width] += 1;
        } else {
            tmp[p][i] += s;
        }

    // 4. Установка нового состояния
    for (std::size_t j = 0; j < f.height; ++j)
        for (std::size_t i = 0; i < f.width; ++i)
            f[j][i] = (tmp[j][i] == 2 && f[j][i]) || (tmp[j][i] == 3);
    delete[] tmp;
}
