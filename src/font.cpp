#include "font.hpp"

static inline int next_p2( int a ) {
    int rval = 1;
    
    while ( rval < a ) {
        rval <<= 1;
    }
    return rval;
}

void gFont::make_dlist( FT_Face face, int ch, GLuint list, GLuint * tex ) {
    FT_BitmapGlyph bitmap_glyph;
    int width, height, i, j;
    FT_Bitmap bitmap;
    FT_Glyph glyph;
    GLubyte * data;
    float x, y;

    if ( FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_RENDER ) ) {
        Panic( "FT_Load_Glyph" );
    }
    if ( FT_Get_Glyph( face->glyph, &glyph ) ) {
        Panic( "FT_Get_Glyph" );
    }
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    bitmap_glyph = (FT_BitmapGlyph) glyph;
    bitmap = bitmap_glyph->bitmap;
    width = next_p2( bitmap.width );
    height = next_p2( bitmap.rows );
    data = new GLubyte [ 2 * width * height ];
    for ( j = 0; j < height; j++) {
        for ( i = 0; i < width; i++ ) {
            data[2 * (i + j * width)] = 255;
            data[2 * (i + j * width) + 1] = (i >= bitmap.width || j >= bitmap.rows) ? 
                                            0 : bitmap.buffer[i + bitmap.width * j];
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
    x = (float) bitmap.width / (float) width;
    y = (float) bitmap.rows / (float) height;
    glBegin( GL_QUADS );
        glTexCoord2d( 0, 0 ); glVertex2f( 0, bitmap.rows );
        glTexCoord2d( 0, y ); glVertex2f( 0, 0 );
        glTexCoord2d( x, y ); glVertex2f( bitmap.width, 0 );
        glTexCoord2d( x, 0 ); glVertex2f( bitmap.width, bitmap.rows );
    glEnd();
    glPopMatrix();
    glTranslatef( face->glyph->advance.x >> 6, 0, 0 );
    glEndList();
    FT_Done_Glyph( glyph );
}

gFont::gFont( std::string fontname, uint16_t height ) {
    FT_Library lib;
    FT_Face face;

    tex = new GLuint[ UI_FONT_LIST ];
    if ( FT_Init_FreeType( &lib ) ) {
        Panic( "FT_Init_FreeType" );
    }
    if ( FT_New_Face( lib, fontname.c_str(), 0, &face ) ) {
        Panic( "FT_New_Face exits" );
    }
    FT_Set_Char_Size( face, height << 6, height << 6, 96, 96 );
    FT_Select_Charmap( face, FT_ENCODING_UNICODE );
    list = glGenLists( UI_FONT_LIST );
    size_t char_table_size = strlen( char_table );
    glGenTextures( char_table_size, tex );
    for ( size_t i = 0; i < char_table_size; i++ ) {
        make_dlist( face, char_table[i], list, tex );
    }
    FT_Done_Face( face );
    FT_Done_FreeType( lib );
}

void gFont::draw( float x, float y, const char * fmt, ... ) {
    int size = strlen( fmt ) * 2 + 50;
    float modelview_matrix[16];
    std::string text;
    va_list ap;

    if ( fmt != nullptr ) {
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
    }
    glPushAttrib( GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT );
    glMatrixMode( GL_MODELVIEW );
    glDisable( GL_LIGHTING ); 
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST ); 
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glListBase( list );
    glGetFloatv( GL_MODELVIEW_MATRIX, modelview_matrix );
    // для вывода нескольких строк
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf( modelview_matrix );
    glTranslatef( x, y, 0 );
    glRotatef( 180, 1.0f, 0.0f, 0.0f );
    glCallLists( size, GL_UNSIGNED_SHORT, text.c_str() );
    glPopMatrix();
    /* для вывода нескольких строк */
    glPopAttrib();
}

gFont::~gFont() {
    glDeleteTextures( UI_FONT_LIST, tex );
    delete[] tex;
}