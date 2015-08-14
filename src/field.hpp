#pragma once

#include <vector>

#include "math.hpp"
#include "vectors.hpp"

class field {
    private:
        std::vector< std::vector< bool > > f;
        std::pair< std::size_t, std::size_t > cell_from_point( const vec3s & p );
    public:
        const std::size_t width, height;
        std::vector< bool > & operator[]( std::size_t i );
        void nextGeneration();
        void create( const vec3s & p );
        void kill( const vec3s & p );
        void toggle( const vec3s & p );
        field(std::size_t h, std::size_t w);
};



