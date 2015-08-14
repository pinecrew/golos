varying vec4 texCoords;

void main(void) {
    vec3 v_coord = gl_Vertex.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(v_coord, 1.0);
    texCoords = vec4(v_coord, 1.0);
}
