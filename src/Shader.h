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
	Shader(const Shader& other);

	static unsigned int compileShader(int mode, const std::string& source);

	[[nodiscard]] Location uniformLocation(const std::string& name) const;

	void uniformMatrix4fv(const Location& location, const float* data);

	void bind() const;
};

