#include <iostream>
#include <GL/glew.h>
#include "GLUtils.h"
#include "Shader.h"


Location::operator int() const { return data; }
Location::operator bool() const { return data != -1; }
Location::Location(const int uniformLocation): data(uniformLocation) {}

Location::Location(Location& other): data(other.data) {
	std::cout << "Copied location" << std::endl;
}

Shader::Shader(): m_id(0) {}
Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource): m_id(0) {
	GLClearErrors();
	m_id = glCreateProgram();
	ASSERT(GLLogCall("glCreateProgram", __FILE__, __LINE__));

	GLClearErrors();
	const unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource);
	const unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
	ASSERT(GLLogCall("compileShaders", __FILE__, __LINE__));

	GLCall(glAttachShader(m_id, vs));
	GLCall(glAttachShader(m_id, fs));

	GLCall(glLinkProgram(m_id));
	GLCall(glValidateProgram(m_id));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
}

Shader::~Shader() {
	GLCall(glDeleteProgram(m_id));
}

unsigned int Shader::compileShader(const int mode, const std::string& source) {
	GLClearErrors();
	const unsigned int id = glCreateShader(mode);
	ASSERT(GLLogCall("glCreateShader", __FILE__, __LINE__));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	
	GLClearErrors();
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = static_cast<char*>(_malloca(length * sizeof(char)));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (mode == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
	}
	GLLogErrors();

	return id;
}

Location Shader::uniformLocation(const std::string& name) const {
	if (!(m_id > 0)) throw std::exception("Invalid program");
	const int location = glGetUniformLocation(m_id, name.c_str());
	return { location };
}

void Shader::uniformMatrix4fv(const Location& location, const float* data) {
	GLCall(glUseProgram(m_id));
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, data));
}

void Shader::uniform1f(const Location& location, const float v0) {
	GLCall(glUseProgram(m_id));
	GLCall(glUniform1f(location, v0));
}

void Shader::uniform2f(const Location& location, const float v0, const float v1) {
	GLCall(glUseProgram(m_id));
	GLCall(glUniform2f(location, v0, v1));
}

void Shader::uniform1i(const Location& location, const int v0) {
	GLCall(glUseProgram(m_id));
	GLCall(glUniform1i(location, v0));
}

void Shader::setOrthographic(const Location& location, float top, float bottom, float right, float left, float far, float near) {
	GLCall(glUseProgram(m_id));

	float ortho[16] = {
		// top 3 rows
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f / (far - near), 0.0f,
		
		// bottom row
		-(right + left) / (right - left), 
		-(top + bottom) / (top - bottom), 
		-(far + near) / (far - near), 
		1.0f
	};

	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, ortho));
}

void Shader::setOrthographic2D(const Location& location, float top, float bottom, float right, float left) {
	GLCall(glUseProgram(m_id));

	float ortho[16] = {
		// top 3 rows
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
	};

	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, ortho));
}

void Shader::bind() const {
	GLCall(glUseProgram(m_id));
}
