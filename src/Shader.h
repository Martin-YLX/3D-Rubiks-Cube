#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>

std::string readShaderSource(const std::string &filepath);
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);

#endif // SHADER_H