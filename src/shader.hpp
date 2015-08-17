#pragma once
#include <GL/glew.h>
#include <glm/vec2.hpp>

class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();
        void addShader(const char* fileName, GLenum shaderType);
        void link();

        void uniform1i(const char* varName, int value);
        void uniform2f(const char* varName, glm::vec2 value);

        void run();
        void stop();
    private:
        GLuint compileShader(const char* fileName, GLenum shaderType);
        GLuint program;
};
