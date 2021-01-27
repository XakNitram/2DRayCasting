#include "pch.hpp"
#include "VertexArray.hpp"

lwvl::VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_id);
}

lwvl::VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void lwvl::VertexArray::bind() {
    glBindVertexArray(m_id);
}

void lwvl::VertexArray::clear() {
    glBindVertexArray(0);
}

uint32_t lwvl::VertexArray::instances() const { return m_instances; }
void lwvl::VertexArray::instances(uint32_t count) { m_instances = count; }

void lwvl::VertexArray::attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset, uint32_t divisor) {
    glEnableVertexAttribArray(m_attributes);
    glVertexAttribPointer(m_attributes, dimensions, type, GL_FALSE, stride, reinterpret_cast<void*>(offset));
    glVertexAttribDivisor(m_attributes, divisor);
    m_attributes++;
}

void lwvl::VertexArray::drawArrays(GLenum mode, int count) const {
    glDrawArraysInstanced(mode, 0, count, m_instances);
}

void lwvl::VertexArray::drawElements(GLenum mode, int count, GLenum type) const {
    glDrawElementsInstanced(mode, count, type, nullptr, m_instances);
}
