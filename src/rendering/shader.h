#pragma once

#include <glad/glad.h>
#include <string>

class Shader{
public:
    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
};
