
#include <fstream>
#include <iostream>

#include "shader.hpp"

GLuint compileShader(const char* fileName, GLenum shaderType) {
    GLuint handler;

    // get a shader handler
    handler = glCreateShader(shaderType);
    // read the shader source from a file
    std::ifstream is(fileName, std::ios::in|std::ios::binary|std::ios::ate);
	if (!is.is_open()) {
		std::cerr << "Unable to open file " << fileName << std::endl;
		exit(1);
	}

	long size = is.tellg();

    auto buffer = new char[size+1];
	is.seekg(0, std::ios::beg);
	is.read (buffer, size);
	is.close();
	buffer[size] = 0;

    // conversions to fit the next function
    const char* const_buffer = buffer;
    // pass the source text to GL
    glShaderSource(handler, 1, &const_buffer, 0);
    // free the memory from the source text
    delete[] buffer;
    // finally compile the shader
    glCompileShader(handler);
    return handler;
}

void addShader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);
    glLinkProgram(program);
};
