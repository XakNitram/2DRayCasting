#pragma once
#include "GL/glew.h"

class VertexArray {
	unsigned int id, vbo;
	unsigned int attributeCount;

public:
	VertexArray();
	VertexArray(VertexArray&& other) noexcept;
	~VertexArray();

	void constructBuffer(GLsizei size, const void* data, GLenum usage);
	void setData(GLintptr offset, GLsizeiptr size, const void* data);
	void attachAttribute(GLuint dimensions, GLenum type, GLsizei size);

	void draw(GLenum mode, GLuint count) const;
};
