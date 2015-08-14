#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

GLuint compileShader(const char* fileName, GLenum shaderType);
void addShader(GLuint program, GLuint shader);
