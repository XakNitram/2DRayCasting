#pragma once
#include <string>

struct Location {
	int data = -1;
	operator int() const;
	operator bool() const;

	Location(const int uniformLocation);
	Location(Location& other);
};


class Shader {
	unsigned int m_id;
	
public:
	Shader();
	Shader(const std::string& vertexFile, const std::string& fragmentFile);
	~Shader();

	static unsigned int compileShader(int mode, const std::string& source);

	[[nodiscard]] Location uniformLocation(const std::string& name) const;

	void uniformMatrix4fv(const Location& location, const float* data);

	void uniform1f(const Location& location, const float v0);
	void uniform2f(const Location& location, const float v0, const float v1);
	void uniform3f(const Location& location, const float v0, const float v1, const float v2);
	void uniform4f(const Location& location, const float v0, const float v1, const float v2, const float v3);

	bool iUniform1f(const Location& location, const float v0);
	bool iUniform2f(const Location& location, const float v0, const float v1);
	bool iUniform3f(const Location& location, const float v0, const float v1, const float v2);
	bool iUniform4f(const Location& location, const float v0, const float v1, const float v2, const float v3);

	void uniform1i(const Location& location, const int v0);

	void setOrthographic(const Location& location, float top, float bottom, float right, float left, float far, float near);
	void setOrthographic2D(const Location& location, float top, float bottom, float right, float left);

	void bind() const;
};

