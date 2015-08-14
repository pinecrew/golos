#pragma once
#ifdef __WIN32__
    #include <windows.h>
#elif __linux__
    #include <GL/glx.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <ft2build.h> 
#include FT_FREETYPE_H
#include <ftglyph.h>
#include <ftoutln.h>
#include <fttrigon.h>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <cstdint>
#include "window.hpp"

class gFont {
public:
    gFont( std::string ttf_file, uint16_t height );
    void draw( float x, float y, const char * fmt, ... );
    ~gFont();
private:
    const uint16_t UI_FONT_LIST = 256;
    void make_dlist( FT_Face face, int ch, GLuint list, GLuint * tex );
    const char * char_table = " !\"#$%&'()*+,-./0123456789:;<=>?@[]\\^_`{}|~"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    GLuint * tex;
    GLuint list;
};