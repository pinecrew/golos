#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include <ft2build.h>
#include <ftglyph.h>
#include FT_FREETYPE_H
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
    const uint16_t FONT_LIST_SIZE = 256;
    void make_dlist( FT_Face face, int ch );
    GLuint * tex;
    GLuint list;
};