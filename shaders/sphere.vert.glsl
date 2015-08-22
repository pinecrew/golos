#version 120

varying vec3 lightDir;
varying vec3 normal;
varying vec3 texCoord;

void main(void) {
    vec4 v = gl_ModelViewMatrix * gl_Vertex;
    lightDir = normalize( vec3( gl_LightSource[0].position - v ));
    normal = normalize( gl_NormalMatrix * vec3( gl_Vertex ));

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord = normalize( vec3( gl_Vertex ));
}
