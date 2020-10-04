#pragma once
#include "rcpch.h"
#include <glad/glad.h>
#include "GLUtils.h"

class VertexArray {
	unsigned int id, vbo, ebo;
	unsigned int attributeCount;

	void genIndexBuffer();

public:
	VertexArray(bool useElementsBuffer);
	VertexArray(VertexArray&& other) noexcept;
	~VertexArray();

	void constructArrayBuffer(GLsizei size, const void* data, GLenum usage);
	void attachAttribute(GLuint dimensions, GLenum type, GLsizei size);
	void setArrayData(GLintptr offset, GLsizeiptr size, const void* data);
	
	void constructIndexBuffer(GLsizei size, const void* data, GLenum usage);
	void setIndexData(GLintptr offset, GLsizeiptr size, const void* data);

	void drawArrays(GLenum mode, GLuint count) const;
	void drawElements(GLenum mode, GLuint count, GLenum type) const;
};
