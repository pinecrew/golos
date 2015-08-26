#version 120

#define PI 3.14159265359

varying vec3 lightDir;
varying vec3 normal;
varying vec3 normCoord;
varying mat3 localBasis;

uniform sampler2D surfaceMap;
uniform sampler2D normalMap;

void main(void) {
    vec2 texCoord = vec2(( atan( normCoord.y, normCoord.x ) / PI + 1.0 ) * 0.5,
                                  ( acos( normCoord.z ) / PI ));

    vec4 color = texture2D( surfaceMap, texCoord );

    // нормаль в локальном базисе
    vec3 localNormal = vec3( texture2D( normalMap, texCoord )) * 2.0 - 1.0;
    // нормаль в мировых координатах
    vec3 n = normalize( gl_NormalMatrix * localBasis * localNormal );
    vec3 l = normalize( lightDir );

    float ambient = 0.3; // фоновая подсветка
    float intensity = ambient + ( 1.0 - ambient ) * max( dot( n, l ), 0.0 );

    gl_FragColor =  intensity * color;
}
