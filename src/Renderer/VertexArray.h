#pragma once
#include "../pch.h"
#include <glad/glad.h>
#include "GLUtils.h"

class VertexArray {
	uint32_t id = 0;
	uint32_t vbo = 0;
	uint32_t ebo = 0;
	unsigned int attributeCount;
	unsigned int stride;

	void genIndexBuffer();

public:
	explicit VertexArray(unsigned int stride, bool useElementsBuffer = false);
	VertexArray(const VertexArray& other) = delete;
	VertexArray(VertexArray&& other) noexcept;
	~VertexArray();

	VertexArray& operator=(const VertexArray& other) = delete;
	VertexArray& operator=(VertexArray&& other) noexcept;

	void constructArrayBuffer(GLsizei size, const void* data, GLenum usage);
	void attachAttribute(GLuint dimensions, GLenum type, unsigned int offset);
	void setArrayData(GLintptr offset, GLsizeiptr size, const void* data);
	
	void constructIndexBuffer(GLsizei size, const void* data, GLenum usage);
	void setIndexData(GLintptr offset, GLsizeiptr size, const void* data);

	void drawArrays(GLenum mode, GLuint count) const;
	void drawElements(GLenum mode, GLuint count, GLenum type) const;
};
