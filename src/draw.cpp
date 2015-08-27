#include "draw.hpp"

// функция загрузки изображения и создание текстуры
// входные параметры:
//     filename -- имя загружаемого изображения
//     texture  -- ссылка на индентификатор текстуры
// выходные параметры:
//     нет
void gLoadImage( const char * filename, GLuint & texture ) {
    SDL_Surface * Surface = IMG_Load( filename );
    if ( Surface == nullptr ) {
        std::string tmp = "Couldn't load ";
        tmp += filename;
        tmp += " image!";
        Info( tmp.c_str() );
        return;
    }
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    int Mode = GL_RGB;
    if ( Surface->format->BytesPerPixel == 4 ) {
        Mode = GL_RGBA;
    }
    glTexImage2D( GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode,
                  GL_UNSIGNED_BYTE, Surface->pixels );
    SDL_FreeSurface( Surface );
}

// функция копирования вершин на основе другой сферы
// входные параметры:
//     sphere -- ссылка на объект gSphere
// выходные параметры:
//     нет
gSphere::gSphere( const gSphere & sphere ) {
    max_count = sphere.max_count;
    size_t copy_count = max_count * coords_count * vertex_count;
    vertex = new float[ copy_count ];
    std::memcpy( (void *) vertex, (void *) sphere.vertex, copy_count );
}

// функция инициализации сферы
// входные параметры:
//     UResolution -- количество параллелей
//     VResolution -- количество вертикалей
// выводные параметры:
//     нет
gSphere::gSphere( std::size_t UResolution, std::size_t VResolution ) {
    // стандартный радиус сферы
    const float radius = 1.0f;
    // начальная параллель
    const float startU = 0.0f;
    // начальная вертикаль
    const float startV = 0.0f;
    // соответственно конечная параллель
    const float endU = M_PI;
    // и вертикаль
    const float endV = M_2PI;
    // шаг рисование параллели
    const float stepU = ( endU - startU ) / (float) UResolution;
    // и вертикали
    const float stepV = ( endV - startV ) / (float) VResolution;
    // количество необходимых вершин
    max_count = UResolution * VResolution * vertex_count;
    vertex = new float[ max_count * coords_count ];
    std::size_t count = 0;
    for ( std::size_t i = 0; i < UResolution; i++ ) {
        for ( std::size_t j = 0; j < VResolution; j++ ) {
            // надеюсь картинка вершин без косяков
            //  (u, v)  --  (u, vn)
            //    |    \       |
            //    |      \     |
            //    |        \   |
            // (un, v)  -- (un, vn)
            // находим необходимые вершины
            float u = (float) i * stepU + startU;
            float v = (float) j * stepV + startV;
            float un = (float) ( i + 1 ) * stepU + startU;
            float vn = (float) ( j + 1 ) * stepV + startV;
            // и добавляем их в массив
            insert_vec3d( count + 0, vec3d( radius, un, v, true ) );
            insert_vec3d( count + 3, vec3d( radius, un, vn, true ) );
            insert_vec3d( count + 6, vec3d( radius, u, v, true ) );
            insert_vec3d( count + 9, vec3d( radius, u, vn, true ) );
            // и увеличиваем вершинный счётчик
            count += vertex_count * coords_count;
        }
    }
}

// функция рисования сферы
// входные параметры:
//     radius   -- радиус сферы, стд. 1.0f
//     position -- положение сферы, стд. (0.0f, 0.0f, 0.0f)
// выходные параметры:
//     нет
void gSphere::draw( float radius, const vec3d & position ) {
    // !!! В случае нормальной работы теней      !!!
    // !!! протестировать код под комментариями  !!!
    glPushMatrix();
    // если position не задан, то не передвигаем сферу
    if ( position.x != 0.0f && position.y != 0.0f && position.z != 0.0f ) {
        glTranslatef( position.x, position.y, position.z );
    }
    // аналогично и для масштабирования сферы
    if ( radius != 1.0f ) {
        glScalef( radius, radius, radius );
    }
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertex );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, max_count );
    glDisableClientState( GL_VERTEX_ARRAY );
    glPopMatrix();
}

// функция деструктор
// входные параметры: нет
// выходные параметры: нет
gSphere::~gSphere() { delete[] vertex; }

// функция добавления вершин в массив
// входные параметры:
//     index -- номер добавляемой вершины
//     coord -- координаты вершины
// выходные параметры:
//     нет
void gSphere::insert_vec3d( std::size_t index, vec3d coord ) {
    vertex[ index + 0 ] = coord.x;
    vertex[ index + 1 ] = coord.y;
    vertex[ index + 2 ] = coord.z;
}
