#include "types.hpp"

field::field(std::size_t h, std::size_t w){
    this->width = w;
    this->height = h;
    this->f = std::vector< std::vector < bool > >(h);
    for (std::size_t i = 0; i < h; ++i)
        this->f[i].assign(w, false);
}

std::vector< bool > field::operator[](std::size_t i){
    return this->f[i];
}
