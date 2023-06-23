#pragma once

#include <glad/glad.h>

#include <string>

class Shader
{
private:
    unsigned int id;

public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    void use() const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

private:
    const std::string readShaderFile(const std::string& shaderPath);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int compileShader(const std::string& source, unsigned int shaderType);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};