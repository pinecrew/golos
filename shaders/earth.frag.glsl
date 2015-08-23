#version 120

#define PI 3.14159265359

varying vec3 lightDir;
varying vec3 normal;
varying vec3 normCoord;
uniform sampler2D surfaceMap;

void main(void) {
    vec2 texCoord = vec2(( atan( normCoord.y, normCoord.x ) / PI + 1.0 ) * 0.5,
                                  ( asin( normCoord.z ) / PI + 0.5 ));
    vec4 color = texture2D( surfaceMap, texCoord );
    vec4 alive = vec4( 0.4, 1.0, 0.4, 1.0 );
    vec4 dead = vec4( 0.4, 0.4, 1.0, 1.0 );
    vec3 n = normalize( normal );
    vec3 l = normalize( lightDir );
    float ambient = 0.3; // фоновая подсветка
    float intensity = ambient + ( 1.0 - ambient ) * max( dot( n, l ), 0.0 );

    gl_FragColor =  intensity * (( color.r < 0.5 ) ? dead : alive );
}
