varying vec3 light;
varying vec3 normal;
varying vec3 texCoords;

void main(void) {
    vec4 v = gl_ModelViewMatrix * gl_Vertex;
    light = normalize( vec3( gl_LightSource[0].position - v ));
    normal = normalize( gl_NormalMatrix * vec3( gl_Vertex ));

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoords = normalize( vec3( gl_Vertex ));
}
