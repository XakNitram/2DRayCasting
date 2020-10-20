#include "rcpch.h"
#include "Shader.h"


/* ****** Uniform ****** */
void lwvl::Uniform::checkValidProgram() {
	int id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);

	// Some program must be bound to set a uniform value.
	assert(id != 0);
}

lwvl::Uniform::Uniform(int location): m_location(location) {}

void lwvl::Uniform::set1i(const int v0) { ValidProgram(glUniform1i(m_location, v0)); }
void lwvl::Uniform::set1f(const float v0) { ValidProgram(glUniform1f(m_location, v0)); }
void lwvl::Uniform::set1u(const unsigned int v0) { ValidProgram(glUniform1ui(m_location, v0)); }

void lwvl::Uniform::set2i(const int v0, const int v1) { ValidProgram(glUniform2i(m_location, v0, v1)); }
void lwvl::Uniform::set2f(const float v0, const float v1) { ValidProgram(glUniform2f(m_location, v0, v1)); }
void lwvl::Uniform::set2u(const unsigned int v0, const unsigned int v1) { ValidProgram(glUniform2ui(m_location, v0, v1)); }

void lwvl::Uniform::set3i(const int v0, const int v1, const int v2) { ValidProgram(glUniform3i(m_location, v0, v1, v2)); }
void lwvl::Uniform::set3f(const float v0, const float v1, const float v2) { ValidProgram(glUniform3f(m_location, v0, v1, v2)); }
void lwvl::Uniform::set3u(const unsigned int v0, const unsigned int v1, const unsigned int v2) { ValidProgram(glUniform3ui(m_location, v0, v1, v2)); }

void lwvl::Uniform::set4i(const int v0, const int v1, const int v2, const int v3) { ValidProgram(glUniform4i(m_location, v0, v1, v2, v3)); }
void lwvl::Uniform::set4f(const float v0, const float v1, const float v2, const float v3) { ValidProgram(glUniform4f(m_location, v0, v1, v2, v3)); }
void lwvl::Uniform::set4u(const unsigned int v0, const unsigned int v1, const unsigned int v2, const unsigned int v3) { ValidProgram(glUniform4ui(m_location, v0, v1, v2, v3)); }

void lwvl::Uniform::setMatrix4(const float* data) {
	GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, data));
}

void lwvl::Uniform::setOrthographic(float top, float bottom, float right, float left, float far, float near) {
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

void lwvl::Uniform::set2DOrthographic(float top, float bottom, float right, float left) {
	float ortho[16] = {
		// top 3 rows
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
	};

	GLCall(glUniformMatrix4fv(m_location, 1, GL_FALSE, ortho));
}

int lwvl::Uniform::location() {
	return m_location;
}


/* ****** Shader Compilation Error ****** */
lwvl::shader_compilation_failure::shader_compilation_failure(const std::string& msg) : std::exception(msg.c_str()) {}


/* ****** Shader ****** */
int lwvl::ShaderProgram::uniformLocation(const std::string& name) const {
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

unsigned int lwvl::ShaderProgram::compileShader(const int mode, const std::string& source) {
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

unsigned int lwvl::ShaderProgram::reserveProgram() {
	GLClearErrors();
	unsigned int id = glCreateProgram();
	ASSERT(GLLogCall("glCreateProgram", __FILE__, __LINE__));
	ASSERT(id != 0);
#ifdef _DEBUG
	std::cout << "Reserved program " << id << std::endl;
#endif // _DEBUG
	return id;
}

lwvl::ShaderProgram::ShaderProgram(): m_id(reserveProgram()) {}

lwvl::ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept: m_id(std::move(other.m_id)) {
	other.m_id = 0;
}

lwvl::ShaderProgram::~ShaderProgram() {
	// An id of 0 will be silently ignored.
	GLCall(glDeleteProgram(m_id));
}

lwvl::ShaderProgram& lwvl::ShaderProgram::operator=(ShaderProgram&& other) noexcept {
	m_id = std::move(other.m_id);
	other.m_id = 0;
	return *this;
}

unsigned int lwvl::ShaderProgram::id() {
	return m_id;
}

lwvl::Uniform lwvl::ShaderProgram::uniform(const std::string& name) {
	int location = uniformLocation(name);
	return { location };
}

void lwvl::ShaderProgram::link() {
	/* Links the program object.
	* https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLinkProgram.xhtml
	* 
	* If any shader objects of type GL_VERTEX_SHADER are attached to program,
	* they will be used to create an executable that will run on the programmable vertex processor.
	* 
	* If any shader objects of type GL_GEOMETRY_SHADER are attached to program,
	* they will be used to create an executable that will run on the programmable geometry processor.
	* 
	* If any shader objects of type GL_FRAGMENT_SHADER are attached to program,
	* they will be used to create an executable that will run on the programmable fragment processor.
	*/

	GLCall(glLinkProgram(m_id));
	// assert(glGetProgramiv(m_id, GL_LINK_STATUS) == GL_TRUE);  
	// assertion because the user can't change enough with shader files to break this.
	
	GLCall(glValidateProgram(m_id));
	// assert(glGetProgramiv(m_id, GL_VALIDATION_STATUS) == GL_TRUE);
	
	/* Checks to see whether the executables contained in program can execute given the current OpenGL state.
	* https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glValidateProgram.xhtml
	* 
	* The error GL_INVALID_OPERATION will be generated by any command that triggers the rendering of geometry if:
    * . Any two active samplers in the current program object are of different types, but refer to the same texture image unit
    * . The number of active samplers in the program exceeds the maximum number of texture image units allowed
	* 
	* It may be difficult or cause a performance degradation for applications to catch these errors when rendering commands are issued. 
	* Therefore, applications are advised to make calls to glValidateProgram to detect these issues during application development.
	*/
}

void lwvl::ShaderProgram::link(const VertexShader& vs, const FragmentShader& fs) {
	GLCall(glAttachShader(m_id, vs.m_id));
	GLCall(glAttachShader(m_id, fs.m_id));

	link();

	GLCall(glDetachShader(m_id, vs.m_id));
	GLCall(glDetachShader(m_id, fs.m_id));
}

void lwvl::ShaderProgram::bind() const {
	GLCall(glUseProgram(m_id));
}

void lwvl::ShaderProgram::clear() {
	GLCall(glUseProgram(0));
}
