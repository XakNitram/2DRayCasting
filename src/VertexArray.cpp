#include "VertexArray.h"

void VertexArray::genIndexBuffer() {
#ifdef _DEBUG
	std::cout << "Initializing element buffer on vertex array " << id << " post-initialization." << std::endl;
#endif // _DEBUG

	glBindVertexArray(id);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

VertexArray::VertexArray(bool useElementsBuffer = false): ebo(0), attributeCount(0) {
	GLCall(glGenVertexArrays(1, &id));
	GLCall(glBindVertexArray(id));
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	
	if (useElementsBuffer) {
		GLCall(glGenBuffers(1, &ebo));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	}
}

VertexArray::VertexArray(VertexArray&& other) noexcept : id(other.id), vbo(other.vbo), ebo(other.ebo), attributeCount(other.attributeCount) {
#ifdef _DEBUG
	std::cout << "Moving vertex array " << id << '.' << std::endl;
#endif // _DEBUG


	other.id = 0;
	other.vbo = 0;
	other.ebo = 0;
}

VertexArray::~VertexArray() {
	if (id) {
		if (ebo) {
			GLCall(glDeleteBuffers(1, &ebo));
		}
		GLCall(glDeleteBuffers(1, &vbo));
		GLCall(glDeleteVertexArrays(1, &id));

#ifdef _DEBUG
		std::cout << "Destroying vertex array " << id << " and associated buffers." << std::endl;
#endif // DEBUG
	}

#ifdef _DEBUG
	else {
		std::cout << "Vertex array lifetime ended with an id of 0." << std::endl;
	}
#endif // DEBUG
}

void VertexArray::constructArrayBuffer(GLsizei size, const void* data, GLenum usage) {
#ifdef _DEBUG
	std::cout << "Constructing array buffer of " << size << " bytes on vertex array " << id << '.' << std::endl;
#endif // DEBUG
	GLCall(glBindVertexArray(id));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

void VertexArray::setArrayData(GLintptr offset, GLsizeiptr size, const void* data) {
#ifdef _DEBUG
	std::cout << "Updating array buffer with " << size << " bytes of data." << std::endl;
#endif // _DEBUG

	GLCall(glBindVertexArray(id));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

void VertexArray::constructIndexBuffer(GLsizei size, const void* data, GLenum usage) {
#ifdef _DEBUG
	std::cout << "Constructing element buffer of " << size << " bytes on vertex array " << id << '.' << std::endl;
#endif // _DEBUG
	if (!ebo) {
		genIndexBuffer();
	}
	
	else {
		GLCall(glBindVertexArray(id));
	}
	
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage));
}

void VertexArray::setIndexData(GLintptr offset, GLsizeiptr size, const void* data) {
#ifdef _DEBUG
	std::cout << "Updating element buffer with " << size << " bytes of data." << std::endl;
#endif // _DEBUG
	GLCall(glBindVertexArray(id));
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void VertexArray::attachAttribute(GLuint dimensions, GLenum type, GLsizei size) {
#ifdef _DEBUG
	std::cout << "Attaching " << dimensions << "d attribute"
		<< ", " << "location=" << attributeCount
		<< " (" << size << " bytes)"
		<< " on vertex array " << id << '.'
		<< std::endl;
#endif // DEBUG
	GLCall(glBindVertexArray(id));
	GLCall(glEnableVertexArrayAttrib(id, attributeCount));
	GLCall(glVertexAttribPointer(attributeCount, dimensions, type, GL_FALSE, size, nullptr));
	attributeCount++;
}

void VertexArray::drawArrays(GLenum mode, GLuint count) const {
	GLCall(glBindVertexArray(id));
	GLCall(glDrawArrays(mode, 0, count));
}

void VertexArray::drawElements(GLenum mode, GLuint count, GLenum type) const {
	ASSERT(ebo);
	GLCall(glBindVertexArray(id));
	GLCall(glDrawElements(mode, count, type, nullptr));
}
