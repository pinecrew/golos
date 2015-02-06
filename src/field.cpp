#include "field.hpp"

field::field(std::size_t h, std::size_t w, float r) :
    f(h), width(w), height(h), r(r) {
    for (std::size_t i = 0; i < h; ++i)
        this->f[i].assign(w, false);
}

std::vector< bool > & field::operator[](std::size_t i) {
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
    for (std::size_t j = 0; j < width; ++j)
        s += f[0][j];

    for (std::size_t j = 0; j < width; ++j)
        if ( f[0][j] ) {
            tmp[0][j] = s - 1;
            ++tmp[1][( j + width - 1 ) % width];
            ++tmp[1][j];
            ++tmp[1][( j + width + 1 ) % width];
        } else {
            tmp[0][j] = s;
        }

    // 2. Между первой и последней
    for (std::size_t i = 1; i < height - 1; ++i)
        for (std::size_t j = 0; j < width; ++j)
            if ( f[i][j] ){
                ++tmp[i - 1][( j + width - 1 ) % width];
                ++tmp[i - 1][j];
                ++tmp[i - 1][( j + width + 1 ) % width];

                ++tmp[i][( j + width - 1 ) % width];
                ++tmp[i][( j + width + 1 ) % width];

                ++tmp[i + 1][( j + width - 1 ) % width];
                ++tmp[i + 1][j];
                ++tmp[i + 1][( j + width + 1 ) % width];
            }

    // 3. Последняя аналогично первой
    std::size_t p = height - 1;
    s = 0;
    for (std::size_t j = 0; j < width; ++j)
        s += f[p][j];

    for (std::size_t j = 0; j < width; ++j)
        if ( f[p][j] ){
            tmp[p][j] += s - 1;

            tmp[p - 1][( j + width - 1 ) % width] += 1;
            tmp[p - 1][j] += 1;
            tmp[p - 1][( j + width + 1 ) % width] += 1;
        } else {
            tmp[p][j] += s;
        }

    // 4. Установка нового состояния
    for (std::size_t i = 0; i < height; ++i)
        for (std::size_t j = 0; j < width; ++j)
            f[i][j] = (tmp[i][j] == 2 && f[i][j]) || (tmp[i][j] == 3);

    delete[] tmp;
}

vec3s field::norm( std::size_t i, std::size_t j ) {
    return { 1,
             ( float ) M_PI * ( i + 0.5f ) / height,
             2.0f * ( float ) M_PI * ( j + 0.5f ) / width };
}

// получаем контур на поверхности сферы
std::vector<vec3s> field::cell_contour( std::size_t i, std::size_t j,
                                        std::size_t npoints ) {
    // пока не обрабатываем ячейки у полюсов, хотя там одна из сторон просто
    // вырождается в точку и все точки на этой стороне совпадают
    std::vector<vec3s> contour( npoints, { r, 0, 0 } );
    std::size_t pos = npoints / 4; // points on side
    std::size_t c = 0;
    // обход из правого нижнего угла против часовой стрелки
    // по меридиану
    for ( ; c < pos; ++c ){
        contour[c].theta = ( i + 1 - ( float ) c / pos ) * M_PI / height;
        contour[c].phi = ( j + 1 ) * 2 * M_PI / width;
    }
    // по широте
    for ( ; c < 2 * pos; ++c ){
        contour[c].theta = i * M_PI / height;
        contour[c].phi = ( j + 1 - ( float ) ( c - pos ) / pos )
                         * 2 * M_PI / width;
    }
    // снова по меридиану
    for ( ; c < 3 * pos; ++c ){
        contour[c].theta = ( i + ( float ) ( c - 2 * pos ) / pos )
                           * M_PI / height;
        contour[c].phi = j * 2 * M_PI / width;
    }
    // и ещё раз по широте
    for ( ; c < npoints; ++c) {
        auto last = npoints - 3 * pos;
        contour[c].theta = ( i + 1 ) * M_PI / height;
        contour[c].phi = ( j + ( float ) ( c - 3 * pos ) / last )
                         * 2 * M_PI / width;
    }
    return contour;
}

std::pair<std::size_t, std::size_t> field::cell_from_point( vec3s & p ) {
    return { p.theta / M_PI   * height,
             p.phi / 2 / M_PI * width };
}

void field::create( vec3s & p ) {
    auto c = cell_from_point( p );
    f[c.first][c.second] = true;
}

void field::kill( vec3s & p ) {
    auto c = cell_from_point( p );
    f[c.first][c.second] = false;
}

void field::toggle( vec3s & p ) {
    auto c = cell_from_point( p );
    f[c.first][c.second] = !f[c.first][c.second];
}

