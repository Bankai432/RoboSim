
#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexFile;
    std::ifstream fragmentFile;

    vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try{
        vertexFile.open(vertexPath);
        fragmentFile.open(fragmentPath);

        std::stringstream vertexStream;
        std::stringstream fragmentStream;

        vertexStream << vertexFile.rdbuf();
        fragmentStream << fragmentFile.rdbuf();

        vertexFile.close();
        fragmentFile.close();

        vertexCode = vertexStream.str();
        fragmentCode = fragmentStream.str();
    }
    catch(const std::ifstream::failure& e){
        std::cerr<<"ERROR: Failed to read shader files: "
                <<e.what()<<'\n';
        std::exit(EXIT_FAILURE);
    }

    const char* vertexSource=vertexCode.c_str();
    const char* fragmentSource=fragmentCode.c_str();

    GLuint vertex=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vertexSource,nullptr);
    glCompileShader(vertex);

    int success;
    char infoLog[512];

    glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(vertex,512,nullptr,infoLog);
        std::cerr<<"ERROR: Vertex shader compilation failed:\n"
                    <<infoLog<<'\n';
        std::exit(EXIT_FAILURE);
    }

    GLuint fragment=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fragmentSource,nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(fragment,512,nullptr,infoLog);
        std::cerr<<"ERROR: Fragment shader compilation failed:\n"
                <<infoLog<<'\n';
        std::exit(EXIT_FAILURE);
    }

    ID=glCreateProgram();

    glAttachShader(ID,vertex);
    glAttachShader(ID,fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID,GL_LINK_STATUS,&success);
    if(!success){
        glGetProgramInfoLog(ID,512,nullptr,infoLog);
        std::cerr<<"ERROR: Shader program linking failed:\n"
                  <<infoLog<<'\n';
        std::exit(EXIT_FAILURE);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use(){
    glUseProgram(ID);
}
