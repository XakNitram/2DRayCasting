#include "rcpch.h"
#include "Shader.h"


/* ****** Uniform ****** */
void Uniform::checkValidProgram() {
	int id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);

	// Some program must be bound to set a uniform value.
	assert(id != 0);
}

Uniform::Uniform(): m_location(-1) {}

Uniform::Uniform(int location): m_location(location) {}

void Uniform::set1i(const int v0) { ValidProgram(glUniform1i(m_location, v0)); }
void Uniform::set1f(const float v0) { ValidProgram(glUniform1f(m_location, v0)); }
void Uniform::set1u(const unsigned int v0) { ValidProgram(glUniform1ui(m_location, v0)); }

void Uniform::set2i(const int v0, const int v1) { ValidProgram(glUniform2i(m_location, v0, v1)); }
void Uniform::set2f(const float v0, const float v1) { ValidProgram(glUniform2f(m_location, v0, v1)); }
void Uniform::set2u(const unsigned int v0, const unsigned int v1) { ValidProgram(glUniform2ui(m_location, v0, v1)); }

void Uniform::set3i(const int v0, const int v1, const int v2) { ValidProgram(glUniform3i(m_location, v0, v1, v2)); }
void Uniform::set3f(const float v0, const float v1, const float v2) { ValidProgram(glUniform3f(m_location, v0, v1, v2)); }
void Uniform::set3u(const unsigned int v0, const unsigned int v1, const unsigned int v2) { ValidProgram(glUniform3ui(m_location, v0, v1, v2)); }

void Uniform::set4i(const int v0, const int v1, const int v2, const int v3) { ValidProgram(glUniform4i(m_location, v0, v1, v2, v3)); }
void Uniform::set4f(const float v0, const float v1, const float v2, const float v3) { ValidProgram(glUniform4f(m_location, v0, v1, v2, v3)); }
void Uniform::set4u(const unsigned int v0, const unsigned int v1, const unsigned int v2, const unsigned int v3) { ValidProgram(glUniform4ui(m_location, v0, v1, v2, v3)); }

void Uniform::setMatrix4(const float* data) {
	GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, data));
}

void Uniform::setOrthographic(float top, float bottom, float right, float left, float far, float near) {
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

	GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, ortho));
}

void Uniform::set2DOrthographic(float top, float bottom, float right, float left) {
	float ortho[16] = {
		// top 3 rows
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
	};

	GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, ortho));
}

int Uniform::location() {
	return m_location;
}


/* ****** Shader ****** */
int Shader::uniformLocation(const std::string& name) const {
	GLCall(const int location = glGetUniformLocation(m_id, name.c_str()));
	if (location == -1) {
		std::stringstream msg;
		msg << "Uniform " << name << " not found.";
		throw std::exception(msg.str().c_str());
	}
	
	else {
		return location;
	}
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


Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource): m_id(0) {
	if (!vertexSource.size()) {
		throw std::invalid_argument("Empty vertex source code.");
	}

	else if (!fragmentSource.size()) {
		throw std::invalid_argument("Empty fragment source code.");
	}

	GLClearErrors();
	m_id = glCreateProgram();
	ASSERT(GLLogCall("glCreateProgram", __FILE__, __LINE__));
	ASSERT(m_id != 0);

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

Shader::Shader(Shader&& other) noexcept: m_id(std::move(other.m_id)) {
	other.m_id = 0;
}

Shader::~Shader() {
	GLCall(glDeleteProgram(m_id));
}

Shader& Shader::operator=(Shader&& other) noexcept {
	m_id = std::move(other.m_id);
	other.m_id = 0;
	return *this;
}

unsigned int Shader::id() {
	return m_id;
}

Uniform Shader::uniform(const std::string& name) {
	int location = uniformLocation(name);
	return { location };
}

void Shader::bind() const {
	GLCall(glUseProgram(m_id));
}

void Shader::clear() {
	GLCall(glUseProgram(0));
}
