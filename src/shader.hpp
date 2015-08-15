#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

GLuint compileShader(const char* fileName, GLenum shaderType);
void printInfoLog(GLuint program);
