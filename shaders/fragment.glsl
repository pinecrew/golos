varying vec3 light;
varying vec3 normal;
varying vec3 texCoords;
uniform sampler2D mytexture;

void main(void) {
    vec2 longitudeLatitude = vec2((atan(texCoords.y, texCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(texCoords.z) / 3.1415926 + 0.5));
        // processing of the texture coordinates;
        // this is unnecessary if correct texture coordinates are specified by the application

    vec4 color = texture2D(mytexture, longitudeLatitude);
        // look up the color of the texture image specified by the uniform "mytexture"
        // at the position specified by "longitudeLatitude.x" and
        // "longitudeLatitude.y" and return it in "gl_FragColor"

    vec4 alive = vec4(0.4, 1.0, 0.4, 1.0);
    vec4 dead = vec4(0.4, 0.4, 1.0, 1.0);
    vec3 n = normalize( normal );
    vec3 l = normalize( light );
    float intensity = 0.1 + 0.9 * max ( dot( n, l ), 0.0 );
    gl_FragColor =  intensity * ((color.r < 0.5) ? dead : alive);
}
