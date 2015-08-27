#pragma once
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdarg>
#include <cstdint>
#include <iostream>
#include <string>
#include <wchar.h>
#include "error.hpp"

class gFont {
  public:
    void load( std::string ttfFile, uint16_t height );
    void draw( float x, float y, const char * fmt, ... );
    void drawUTF( float x, float y, const char * fmt, ... );
    ~gFont();

  private:
    const uint16_t FONT_LIST_SIZE = 128;
    void makeDlist( uint16_t ch );
    TTF_Font * font = nullptr;
    GLuint * tex = nullptr;
    GLuint texUtf;
    GLuint list = 0;
};
