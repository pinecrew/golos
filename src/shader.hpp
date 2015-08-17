#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <fstream>
#include <iostream>
#include "error.hpp"

GLuint compileShader(const char* fileName, GLenum shaderType);
void printInfoLog(GLuint program);