#include "font.hpp"

static inline int nextP2( int a ) {
    int rval = 1;

    while ( rval < a ) {
        rval <<= 1;
    }
    return rval;
}

inline void pushOpenglParams() {
    GLint viewport[ 4 ];
    glPushAttrib( GL_TRANSFORM_BIT );
    glGetIntegerv( GL_VIEWPORT, viewport );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( viewport[ 0 ], viewport[ 2 ], viewport[ 1 ], viewport[ 3 ] );
    glPopAttrib();
    glPushAttrib( GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT |
                  GL_TRANSFORM_BIT | GL_TEXTURE_BIT );
    glMatrixMode( GL_MODELVIEW );
    glDisable( GL_LIGHTING );
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

inline void popOpenglParams() {
    glPopAttrib();
    glPushAttrib( GL_TRANSFORM_BIT );
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glPopAttrib();
}

void gFont::makeDlist( uint16_t ch ) {
    SDL_Color colorFg = {255, 255, 255, 255};
    SDL_Surface * surface = TTF_RenderGlyph_Blended( font, ch, colorFg );
    if ( surface == nullptr ) {
        Panic( "TTF_RenderText_Blended" );
    }
    uint16_t width = nextP2( surface->w );
    uint16_t height = nextP2( surface->h );
    SDL_Surface * s = SDL_CreateRGBSurface(
        0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );
    SDL_BlitSurface( surface, nullptr, s, NULL );
    glBindTexture( GL_TEXTURE_2D, tex[ ch ] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, s->pixels );
    glNewList( list + ch, GL_COMPILE );
    glBindTexture( GL_TEXTURE_2D, tex[ ch ] );
    glPushMatrix();
    float x = (float) surface->w / (float) width;
    float y = (float) surface->h / (float) height;
    glBegin( GL_TRIANGLE_FAN );
    glTexCoord2d( x, 0 );
    glVertex2f( surface->w, surface->h );
    glTexCoord2d( 0, 0 );
    glVertex2f( 0, surface->h );
    glTexCoord2d( 0, y );
    glVertex2f( 0, 0 );
    glTexCoord2d( x, y );
    glVertex2f( surface->w, 0 );
    glEnd();
    glPopMatrix();
    glTranslatef( surface->w, 0, 0 );
    glEndList();
    SDL_FreeSurface( surface );
    SDL_FreeSurface( s );
}

void gFont::load( std::string fontName, uint16_t height ) {
    tex = new GLuint[ FONT_LIST_SIZE ];
    if ( TTF_Init() == -1 ) {
        Panic( TTF_GetError() );
    }
    font = TTF_OpenFont( fontName.c_str(), height );
    if ( font == nullptr ) {
        Panic( TTF_GetError() );
    }
    list = glGenLists( FONT_LIST_SIZE );
    glGenTextures( FONT_LIST_SIZE, tex );
    for ( uint16_t i = 32; i < FONT_LIST_SIZE; i++ ) {
        makeDlist( i );
    }
}

void gFont::draw( float x, float y, const char * fmt, ... ) {
    int size = strlen( fmt ) * 2 + 50;

    if ( size > 50 ) {
        std::string text;
        va_list ap;
        while ( true ) {
            text.resize( size );
            va_start( ap, fmt );
            int n = vsnprintf( (char *) text.data(), size, fmt, ap );
            va_end( ap );
            if ( n > -1 && n < size ) {
                text.resize( n );
                break;
            }
            if ( n > -1 ) {
                size = n + 1;
            } else {
                size *= 2;
            }
        }
        pushOpenglParams();
        glListBase( list );
        float modelviewMatrix[ 16 ];
        glGetFloatv( GL_MODELVIEW_MATRIX, modelviewMatrix );
        glPushMatrix();
        glLoadIdentity();
        glMultMatrixf( modelviewMatrix );
        glTranslatef( x, y, 0 );
        glCallLists( text.length(), GL_UNSIGNED_BYTE, text.c_str() );
        glPopMatrix();
        popOpenglParams();
    }
}

void gFont::drawUTF( float x, float y, const char * fmt, ... ) {
    int size = strlen( fmt ) * 2 + 50;

    if ( size > 50 ) {
        std::string text;
        va_list ap;
        while ( true ) {
            text.resize( size );
            va_start( ap, fmt );
            int n = vsnprintf( (char *) text.data(), size, fmt, ap );
            va_end( ap );
            if ( n > -1 && n < size ) {
                text.resize( n );
                break;
            }
            if ( n > -1 ) {
                size = n + 1;
            } else {
                size *= 2;
            }
        }
        pushOpenglParams();
        float modelviewMatrix[ 16 ];
        glGetFloatv( GL_MODELVIEW_MATRIX, modelviewMatrix );
        glPushMatrix();
        glLoadIdentity();
        glMultMatrixf( modelviewMatrix );
        glTranslatef( x, y, 0 );
        SDL_Color colorFg = {255, 255, 255, 255};
        SDL_Surface * surface =
            TTF_RenderUTF8_Blended( font, text.c_str(), colorFg );
        if ( surface == nullptr ) {
            Panic( "TTF_RenderText_Blended" );
        }
        uint16_t width = nextP2( surface->w );
        uint16_t height = nextP2( surface->h );
        SDL_Surface * s =
            SDL_CreateRGBSurface( 0, width, height, 32, 0x00ff0000, 0x0000ff00,
                                  0x000000ff, 0xff000000 );
        SDL_BlitSurface( surface, nullptr, s, NULL );
        glBindTexture( GL_TEXTURE_2D, texUtf );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, s->pixels );
        glBindTexture( GL_TEXTURE_2D, texUtf );
        float x = (float) surface->w / (float) width;
        float y = (float) surface->h / (float) height;
        glBegin( GL_TRIANGLE_FAN );
        glTexCoord2d( x, 0 );
        glVertex2f( surface->w, surface->h );
        glTexCoord2d( 0, 0 );
        glVertex2f( 0, surface->h );
        glTexCoord2d( 0, y );
        glVertex2f( 0, 0 );
        glTexCoord2d( x, y );
        glVertex2f( surface->w, 0 );
        glEnd();
        glTranslatef( surface->w, 0, 0 );
        SDL_FreeSurface( surface );
        SDL_FreeSurface( s );
        glEnd();
        glPopMatrix();
        popOpenglParams();
    }
}

gFont::~gFont() {
    TTF_CloseFont( font );
    TTF_Quit();
    glDeleteTextures( FONT_LIST_SIZE, tex );
    delete[] tex;
}
