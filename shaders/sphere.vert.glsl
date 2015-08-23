#version 120

varying vec3 lightDir;   // направление на источник света в мировых координатах

varying vec3 normal;     // нормаль к сфере в мировых координатах

varying vec3 normCoord;  // нормированные координаты на сфере для натягивания текстуры

varying mat3 localBasis; // матрица для перевода из локального базиса на пов-ти сферы
                         // в мировые координаты для normal mapping

void main(void) {
    vec4 position = gl_ModelViewMatrix * gl_Vertex; // место вершины в мировых координатах
    lightDir = normalize( vec3( gl_LightSource[0].position - position ));

    normal = normalize( gl_NormalMatrix * vec3( gl_Vertex ));

    normCoord = normalize( vec3( gl_Vertex ));

    // локальный базис на пов-ти сферы в коордиинатах модели
    vec3 ex = normalize( vec3( -normCoord.y,  normCoord.x, 0.0 ));
    vec3 ez = normCoord;
    vec3 ey = cross( ez, ex );

    // матрица перевода локального базиса в координаты модели
    // требуется для normal map, так как она задаётся в локальном базисе
    localBasis = mat3( ex, ey, ez );


    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
