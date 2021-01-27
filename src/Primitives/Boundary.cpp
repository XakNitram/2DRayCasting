#include "pch.hpp"
#include "Boundary.hpp"

Boundary::Boundary(float x1, float y1, float x2, float y2): line(x1, y1, x2, y2) {
	float positions[4] = { x1, y1, x2, y2 };
    vao.bind();
    vbo.bind();
    vbo.usage(lwvl::Usage::Static);
    vbo.construct(positions, 4);
    vao.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    lwvl::ArrayBuffer::clear();
    lwvl::VertexArray::clear();
}

Boundary::Boundary(Boundary&& other) noexcept: line(other.line), vao(other.vao) {}

void Boundary::draw() {
    vao.bind();
	vao.drawArrays(GL_LINES, 2);
}
