#include <iostream>
#include <fstream>
#include "shader.hpp"

ShaderProgram::ShaderProgram() { program = glCreateProgram(); }

ShaderProgram::~ShaderProgram() { glDeleteProgram( program ); }

char * readFile( const char * fileName ) {
    std::ifstream is( fileName,
                      std::ios::in | std::ios::binary | std::ios::ate );
    if ( !is.is_open() ) {
        std::string tmp = "Unable to open file ";
        tmp += fileName;
        Panic( tmp.c_str() );
    }

    long size = is.tellg();

    auto buffer = new char[ size + 1 ];
    is.seekg( 0, std::ios::beg );
    is.read( buffer, size );
    is.close();
    buffer[ size ] = 0;
    return buffer;
}

GLuint ShaderProgram::compileShader( const char * fileName,
                                     GLenum shaderType ) {
    GLuint handler;

    // get a shader handler
    handler = glCreateShader( shaderType );
    // read the shader source from a file
    // conversions to fit the next function
    const char * buffer = readFile( fileName );
    // pass the source text to GL
    glShaderSource( handler, 1, &buffer, 0 );
    // finally compile the shader
    glCompileShader( handler );
    return handler;
}

void ShaderProgram::addShader( const char * fileName, GLenum shaderType ) {
    auto shader = compileShader( fileName, shaderType );
    glAttachShader( program, shader );
};

void ShaderProgram::link() { glLinkProgram( program ); }

void ShaderProgram::run() { glUseProgram( program ); }

void ShaderProgram::stop() { glUseProgram( 0 ); }

void ShaderProgram::uniform( const char * varName, int value ) {
    glUniform1i( glGetUniformLocation( program, varName ), value );
}

