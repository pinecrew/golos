#pragma once
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdarg>
#include <cstdlib>
#include <cstdint>
#include <wchar.h>
#include "window.hpp"

class gFont {
public:
    void load( std::string ttf_file, uint16_t height );
    void draw( float x, float y, const char * fmt, ... );
    ~gFont();
private:
    const uint16_t FONT_LIST_SIZE = 128;
    void make_dlist( uint16_t ch );
    TTF_Font * font = nullptr;
    GLuint * tex = nullptr;
    GLuint list = 0;
};