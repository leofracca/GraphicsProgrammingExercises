#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexShader = readShaderFile(vertexPath);
	std::string fragmentShader = readShaderFile(fragmentPath);

	id = createShader(vertexShader, fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::use() const
{
	glUseProgram(id);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setMatrix4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

const std::string Shader::readShaderFile(const std::string& shaderPath)
{
	std::ifstream stream(shaderPath);

	std::stringstream ss;
	std::string line;
	while (std::getline(stream, line))
		ss << line << '\n';

	return ss.str();
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	checkCompileErrors(program, "PROGRAM");

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

unsigned int Shader::compileShader(const std::string& source, unsigned int shaderType)
{
	unsigned int id = glCreateShader(shaderType);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	// Error handling
	checkCompileErrors(id, (shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT"));

	return id;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
