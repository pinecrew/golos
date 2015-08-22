#version 120

varying vec3 lightDir;
varying vec3 normal;
varying vec3 texCoord;
uniform sampler2D surfaceMap;

void main(void) {
    vec2 longitudeLatitude = vec2((atan(texCoord.y, texCoord.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(texCoord.z) / 3.1415926 + 0.5));
        // processing of the texture coordinates;
        // this is unnecessary if correct texture coordinates are specified by the application

    vec4 color = texture2D(surfaceMap, longitudeLatitude);
        // look up the color of the texture image specified by the uniform "mytexture"
        // at the position specified by "longitudeLatitude.x" and
        // "longitudeLatitude.y" and return it in "gl_FragColor"

    vec4 alive = vec4(0.4, 1.0, 0.4, 1.0);
    vec4 dead = vec4(0.4, 0.4, 1.0, 1.0);
    vec3 n = normalize( normal );
    vec3 l = normalize( lightDir );
    float ambient = 0.3;
    float intensity = ambient + (1.0 - ambient) * max ( dot( n, l ), 0.0 );

    gl_FragColor =  intensity * ((color.r < 0.5) ? dead : alive);
}
