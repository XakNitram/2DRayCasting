#pragma once
#include "rcpch.h"
#include <glad/glad.h>
#include "GLUtils.h"


class Uniform {
	int m_location;

	static void checkValidProgram();

public:
	Uniform();
	Uniform(int location);
	Uniform(const Uniform& other) = default;
	Uniform(Uniform&& other) = default;

	Uniform& operator=(const Uniform& other) = default;
	Uniform& operator=(Uniform&& other) = default;

	void set1i(const int i0);
	void set1f(const float f0);
	void set1u(const unsigned int u0);

	void set2i(const int i0, const int i1);
	void set2f(const float f0, const float f1);
	void set2u(const unsigned int u0, const unsigned int u1);

	void set3i(const int i0, const int i1, const int i2);
	void set3f(const float f0, const float f1, const float f2);
	void set3u(const unsigned int u0, const unsigned int u1, const unsigned int u2);

	void set4i(const int i0, const int i1, const int i2, const int i3);
	void set4f(const float f0, const float f1, const float f2, const float f3);
	void set4u(const unsigned int u0, const unsigned int u1, const unsigned int u2, const unsigned int u3);

	void setMatrix4(const float* data);
	void setOrthographic(float top, float bottom, float right, float left, float far, float near);
	void set2DOrthographic(float top, float bottom, float right, float left);

	int location();
};


#ifndef NDEBUG
#define ValidProgram(x) Uniform::checkValidProgram(); GLCall(x)
#else
#define ValidProgram(x) x
#endif


class Shader {
	unsigned int m_id;

	int uniformLocation(const std::string& name) const;
	static unsigned int compileShader(int mode, const std::string& source);

public:
	Shader() = default;
	Shader(const std::string& vertexFile, const std::string& fragmentFile);
	Shader(const Shader& other) = delete;
	Shader(Shader&& other) noexcept;
	~Shader();

	Shader& operator=(const Shader& other) = delete;
	Shader& operator=(Shader&& other) noexcept;

	unsigned int id();
	Uniform uniform(const std::string& name);

	void bind() const;
	static void clear();
};

