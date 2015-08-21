varying vec3 lightDir;
varying vec3 normal;
varying vec3 texCoord;
varying vec3 smCoord;

uniform sampler2D surfaceMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

void main(void) {
    vec2 longitudeLatitude = vec2((atan(texCoord.y, texCoord.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(texCoord.z) / 3.1415926 + 0.5));
        // processing of the texture coordinates;
        // this is unnecessary if correct texture coordinates are specified by the application

    vec4 color = texture2D(surfaceMap, longitudeLatitude);
        // look up the color of the texture image specified by the uniform "mytexture"
        // at the position specified by "longitudeLatitude.x" and
        // "longitudeLatitude.y" and return it in "gl_FragColor"

    vec3 ex = normalize(vec3( -texCoord.y,  texCoord.x, 0.0 ));
    vec3 ez = normalize( texCoord );
    vec3 ey = cross( ez, ex );

    vec4 tmp = texture2D( normalMap, longitudeLatitude ) * 2.0 - 1.0;
    vec3 n = gl_NormalMatrix * normalize( tmp.x * ex + tmp.y * ey + tmp.z * ez );
    n = normalize(n);
    vec3 l = normalize( lightDir );

    float ambient = 0.3;
    float intensity = ambient + (1.0 - ambient) * max ( dot( n, l ), 0.0 );

    if (smCoord.z > texture2D(shadowMap, smCoord.xy).x)
        intensity = ambient;

    gl_FragColor =  intensity * color;
}
