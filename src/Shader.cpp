#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <GL/glew.h>
#include "Shader.h"


Location::operator int() const { return data; }
Location::operator bool() const { return data != -1; }
Location::Location(const int uniformLocation): data(uniformLocation) {}

Location::Location(Location& other): data(other.data) {
	std::cout << "Copied location" << std::endl;
}

std::string read_file(const std::string& filepath) {
	std::ifstream file(filepath);
	std::stringstream output_stream;

	std::string line;
	while (getline(file, line)) {
		output_stream << line << '\n';
	}

	return output_stream.str();
}

Shader::Shader(): m_id(0) {}
Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile): m_id(0) {
	m_id = glCreateProgram();

	const std::string vertex_source = read_file(vertexFile);
	const std::string fragment_source = read_file(fragmentFile);

	const unsigned int vs = compileShader(GL_VERTEX_SHADER, vertex_source);
	const unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragment_source);

	//GLCall(glAttachShader(m_id, vs));
	glAttachShader(m_id, vs);
	//GLCall(glAttachShader(m_id, fs));
	glAttachShader(m_id, fs);

	//GLCall(glLinkProgram(m_id));
	glLinkProgram(m_id);
	//GLCall(glValidateProgram(m_id));
	glValidateProgram(m_id);

	//GLCall(glDeleteShader(vs));
	glDeleteShader(vs);
	//GLCall(glDeleteShader(fs));
	glDeleteShader(fs);
}

unsigned int Shader::compileShader(const int mode, const std::string& source) {
	const unsigned int id = glCreateShader(mode);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
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

	return id;
}

Location Shader::uniformLocation(const std::string& name) const {
	if (!m_id > 0) throw std::exception("Invalid program");
	const int location = glGetUniformLocation(m_id, name.c_str());
	return { location };
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Shader::uniformMatrix4fv(const Location& location, const float* data) {
	glUseProgram(m_id);
	glUniformMatrix4fv(location, 1, GL_FALSE, data);
}

void Shader::bind() const {
	glUseProgram(m_id);
}
