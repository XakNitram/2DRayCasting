#include <iostream>
#include "GLUtils.h"
#include "VertexArray.h"


VertexArray::VertexArray(): attributeCount(0) {
	GLCall(glGenVertexArrays(1, &id));
	GLCall(glBindVertexArray(id));
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
}

VertexArray::VertexArray(VertexArray&& other) noexcept : id(other.id), vbo(other.vbo), attributeCount(0) {
	other.id = 0;
	other.vbo = 0;
}

VertexArray::~VertexArray() {
	if (id) {
		GLCall(glDeleteBuffers(1, &vbo));
		GLCall(glDeleteVertexArrays(1, &id));
	}
}

void VertexArray::constructBuffer(GLsizei size, const void* data, GLenum usage) {
	GLCall(glBindVertexArray(id));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

void VertexArray::setData(GLintptr offset, GLsizeiptr size, const void* data) {
	GLCall(glBindVertexArray(id));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

void VertexArray::attachAttribute(GLuint dimensions, GLenum type, GLsizei size) {
	GLCall(glBindVertexArray(id));
	GLCall(glEnableVertexArrayAttrib(id, attributeCount));
	GLCall(glVertexAttribPointer(attributeCount, dimensions, type, GL_FALSE, size, nullptr));
	attributeCount++;
}

void VertexArray::draw(GLenum mode, GLuint count) const {
	GLCall(glBindVertexArray(id));
	GLCall(glDrawArrays(mode, 0, count));
}
