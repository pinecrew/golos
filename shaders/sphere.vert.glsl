uniform mat4 lightMVP;

varying vec3 lightDir;
varying vec3 normal;
varying vec3 texCoord;
varying vec3 smCoord;

void main(void) {
    vec4 v = gl_ModelViewMatrix * gl_Vertex;
    lightDir = normalize( vec3( gl_LightSource[0].position - v ));
    normal = normalize( gl_NormalMatrix * vec3( gl_Vertex ));

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord = normalize( vec3( gl_Vertex ));
    vec4 smCoord4 = lightMVP * gl_Vertex;
    smCoord = 0.5 * smCoord4.xyz / smCoord4.w + vec3(0.5, 0.5, 0.5);
}
