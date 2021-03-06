#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "error.hpp"

class ShaderProgram {
  public:
    ShaderProgram();
    ~ShaderProgram();
    void addShader( const char * fileName, GLenum shaderType );
    void link();

    void uniform( const char * varName, int value );

    void run();
    void stop();

  private:
    GLuint compileShader( const char * fileName, GLenum shaderType );
    GLuint program;
};
