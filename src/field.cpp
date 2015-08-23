#include "field.hpp"

field::field( std::size_t h, std::size_t w ) : f( h ), width( w ), height( h ) {
    for ( std::size_t i = 0; i < h; ++i )
        this->f[ i ].assign( w, false );
}

std::vector<bool> & field::operator[]( std::size_t i ) { return this->f[ i ]; }

void field::nextGeneration() {
    // массив для подсчёта соседей
    auto tmp = new std::vector<char>[ height ];
    for ( std::size_t i = 0; i < height; ++i )
        tmp[ i ].assign( width, 0 );

    // 1. Обрабатываем первую строку
    //    в ней все соседи друг другу, так как есть общая вершина (полюс)
    //    поэтому проще всего посчитать сумму в этой строке
    std::size_t s = 0;
    for ( std::size_t j = 0; j < width; ++j )
        s += f[ 0 ][ j ];

    for ( std::size_t j = 0; j < width; ++j )
        if ( f[ 0 ][ j ] ) {
            tmp[ 0 ][ j ] = s - 1;
            ++tmp[ 1 ][ ( j + width - 1 ) % width ];
            ++tmp[ 1 ][ j ];
            ++tmp[ 1 ][ ( j + width + 1 ) % width ];
        } else {
            tmp[ 0 ][ j ] = s;
        }

    // 2. Между первой и последней
    for ( std::size_t i = 1; i < height - 1; ++i )
        for ( std::size_t j = 0; j < width; ++j )
            if ( f[ i ][ j ] ) {
                ++tmp[ i - 1 ][ ( j + width - 1 ) % width ];
                ++tmp[ i - 1 ][ j ];
                ++tmp[ i - 1 ][ ( j + width + 1 ) % width ];

                ++tmp[ i ][ ( j + width - 1 ) % width ];
                ++tmp[ i ][ ( j + width + 1 ) % width ];

                ++tmp[ i + 1 ][ ( j + width - 1 ) % width ];
                ++tmp[ i + 1 ][ j ];
                ++tmp[ i + 1 ][ ( j + width + 1 ) % width ];
            }

    // 3. Последняя аналогично первой
    std::size_t p = height - 1;
    s = 0;
    for ( std::size_t j = 0; j < width; ++j )
        s += f[ p ][ j ];

    for ( std::size_t j = 0; j < width; ++j )
        if ( f[ p ][ j ] ) {
            tmp[ p ][ j ] += s - 1;

            tmp[ p - 1 ][ ( j + width - 1 ) % width ] += 1;
            tmp[ p - 1 ][ j ] += 1;
            tmp[ p - 1 ][ ( j + width + 1 ) % width ] += 1;
        } else {
            tmp[ p ][ j ] += s;
        }

    // 4. Установка нового состояния
    for ( std::size_t i = 0; i < height; ++i )
        for ( std::size_t j = 0; j < width; ++j )
            f[ i ][ j ] =
                ( tmp[ i ][ j ] == 2 && f[ i ][ j ] ) || ( tmp[ i ][ j ] == 3 );

    delete[] tmp;
}

std::pair<std::size_t, std::size_t> field::cell_from_point( const vec3s & p ) {
    return {p.theta / M_PI * height, p.phi / 2 / M_PI * width};
}

void field::random_fill() {
    for ( std::size_t i = 0; i < ( width * height ) / 3; i++ )
        f[ rand() % height ][ rand() % width ] = true;
}

void field::clear() {
    for ( auto & row : f )
        std::fill( row.begin(), row.end(), false );
}

void field::create( const vec3s & p ) {
    auto c = cell_from_point( p );
    f[ c.first ][ c.second ] = true;
}

void field::kill( const vec3s & p ) {
    auto c = cell_from_point( p );
    f[ c.first ][ c.second ] = false;
}

void field::toggle( const vec3s & p ) {
    auto c = cell_from_point( p );
    f[ c.first ][ c.second ] = !f[ c.first ][ c.second ];
}
