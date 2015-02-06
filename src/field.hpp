#pragma once
#include <cmath>
#include <vector>
#include "vectors.hpp"
#include "types.hpp"

class field {
    private:
        std::vector< std::vector< bool > > f;
        std::pair< std::size_t, std::size_t > cell_from_point( vec3s & p );
    public:
        const std::size_t width, height;
        const float r;
        std::vector< bool > & operator[]( std::size_t i );
        vec3s norm( std::size_t i, std::size_t j );
        void nextGeneration();
        void create( vec3s & p );
        void kill( vec3s & p );
        void toggle( vec3s & p );
        std::vector<vec3s> cell_contour( std::size_t i, std::size_t j,
                                         std::size_t npoints );
        field(std::size_t h, std::size_t w, float r);
};



