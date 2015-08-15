#include "font.hpp"

static inline int next_p2( int a ) {
    int rval = 1;
    
    while ( rval < a ) {
        rval <<= 1;
    }
    return rval;
}

inline void push_opengl_params() {
    GLint viewport[4];
    glPushAttrib( GL_TRANSFORM_BIT );
    glGetIntegerv( GL_VIEWPORT, viewport );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( viewport[0], viewport[2], viewport[1], viewport[3] );
    glPopAttrib();
    glPushAttrib( GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT );
    glMatrixMode( GL_MODELVIEW );
    glDisable( GL_LIGHTING ); 
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST ); 
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

inline void pop_opengl_params() {
    glPopAttrib();
    glPushAttrib( GL_TRANSFORM_BIT );
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glPopAttrib();
}

void gFont::make_dlist( FT_Face face, uint16_t ch ) {
    if ( FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_RENDER ) ) {
        Panic( "FT_Load_Glyph" );
    }
    FT_Glyph glyph;
    if ( FT_Get_Glyph( face->glyph, &glyph ) ) {
        Panic( "FT_Get_Glyph" );
    }
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph) glyph;
    FT_Bitmap bitmap = bitmap_glyph->bitmap;
    uint16_t width = next_p2( bitmap.width );
    uint16_t height = next_p2( bitmap.rows );
    GLubyte * data = new GLubyte [ 2 * width * height ];
    for ( uint16_t j = 0; j < height; j++) {
        for ( uint16_t i = 0; i < width; i++ ) {
            data[2 * (i + j * width) + 0] = 255;
            data[2 * (i + j * width) + 1] = 
                ( i >= bitmap.width || j >= bitmap.rows ) ? 0 : bitmap.buffer[i + bitmap.width * j];
        }
    }
    glBindTexture( GL_TEXTURE_2D, tex[ch] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data );
    delete[] data;
    glNewList( list + ch, GL_COMPILE );
    glBindTexture( GL_TEXTURE_2D, tex[ch] );
    glTranslatef( bitmap_glyph->left, 0, 0 );
    glPushMatrix();
    glTranslatef( 0, bitmap_glyph->top-bitmap.rows, 0 );
    float x = (float) bitmap.width / (float) width;
    float y = (float) bitmap.rows / (float) height;
    glBegin( GL_TRIANGLE_FAN );
        glTexCoord2d( x, 0 ); glVertex2f( bitmap.width, bitmap.rows );
        glTexCoord2d( 0, 0 ); glVertex2f( 0, bitmap.rows );
        glTexCoord2d( 0, y ); glVertex2f( 0, 0 );
        glTexCoord2d( x, y ); glVertex2f( bitmap.width, 0 );
    glEnd();
    glPopMatrix();
    glTranslatef( face->glyph->advance.x >> 6, 0, 0 );
    glEndList();
}

void gFont::load( std::string fontname, uint16_t height ) {
    tex = new GLuint[ FONT_LIST_SIZE ];
    FT_Library lib;
    if ( FT_Init_FreeType( &lib ) ) {
        Panic( "FT_Init_FreeType" );
    }
    FT_Face face;
    if ( FT_New_Face( lib, fontname.c_str(), 0, &face ) ) {
        Panic( "FT_New_Face exits" );
    }
    FT_Set_Char_Size( face, height << 6, height << 6, 96, 96 );
    list = glGenLists( FONT_LIST_SIZE );
    glGenTextures( FONT_LIST_SIZE, tex );
    for ( uint16_t i = 0; i < FONT_LIST_SIZE; i++ ) {
        make_dlist( face, i );
    }
    FT_Done_Face( face );
    FT_Done_FreeType( lib );
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
        push_opengl_params();
        glListBase( list );
        float modelview_matrix[16];
        glGetFloatv( GL_MODELVIEW_MATRIX, modelview_matrix );
        glPushMatrix();
        glLoadIdentity();
        glMultMatrixf( modelview_matrix );
        glTranslatef( x, y, 0 );
        glCallLists( text.length(), GL_UNSIGNED_BYTE, text.c_str() );
        glPopMatrix();
        pop_opengl_params();
    }
}

gFont::~gFont() {
    glDeleteTextures( FONT_LIST_SIZE, tex );
    delete[] tex;
}