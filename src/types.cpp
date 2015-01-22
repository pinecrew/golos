#include "types.hpp"

field::field(unsigned int w, unsigned int h){
    this->width = w;
    this->height = h;
    this->f = std::vector< std::vector < bool > >(h);
    for (unsigned int i = 0; i < h; ++i)
        this->f[i].assign(w, false);
}

std::vector< bool > field::operator[](unsigned int i){
    return this->f[i];
}
