#pragma once

#include <vector>
#include <cstdlib>

#include "math.hpp"
#include "vectors.hpp"

class Field {
  private:
    std::vector<std::vector<bool>> f;
    std::pair<std::size_t, std::size_t> cellFromPoint( const vec3s & p );
    int count;

  public:
    const std::size_t width, height;
    std::vector<bool> & operator[]( std::size_t i );
    void randomFill();
    void clear();
    void nextGeneration();
    void create( const vec3s & p );
    void kill( const vec3s & p );
    void toggle( const vec3s & p );
    int population();
    Field( std::size_t h, std::size_t w );
};
