varying vec3 light;
varying vec3 normal;
varying vec3 texCoords;
uniform sampler2D surface;
uniform sampler2D normals;

void main(void) {
    vec2 longitudeLatitude = vec2((atan(texCoords.y, texCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(texCoords.z) / 3.1415926 + 0.5));
        // processing of the texture coordinates;
        // this is unnecessary if correct texture coordinates are specified by the application

    vec4 color = texture2D(surface, longitudeLatitude);
        // look up the color of the texture image specified by the uniform "mytexture"
        // at the position specified by "longitudeLatitude.x" and
        // "longitudeLatitude.y" and return it in "gl_FragColor"

    vec3 ex = normalize(vec3( -texCoords.y,  texCoords.x, 0.0 ));
    vec3 ez = normalize( texCoords );
    vec3 ey = cross( ez, ex );
    vec4 tmp = texture2D( normals, longitudeLatitude ) * 2.0 - 1.0;
    vec3 n = gl_NormalMatrix * normalize( tmp.x * ex + tmp.y * ey + tmp.z * ez );
    n = normalize(n);
    vec3 l = normalize( light );
    float intensity = 0.3 + 0.7 * max ( dot( n, l ), 0.0 );
    gl_FragColor =  intensity * color;
}
