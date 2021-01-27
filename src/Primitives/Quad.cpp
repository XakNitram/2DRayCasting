#include "pch.hpp"
#include "Quad.hpp"

Quad::Quad(float left, float bottom, float width, float height) {
    float positions[8] {
        left,         bottom,
        left + width, bottom,
        left + width, bottom + height,
        left,         bottom + height
    };

    vao.bind();
    vbo.bind();
    vbo.usage(lwvl::Usage::Static);
    vbo.construct(positions, 8);
    vao.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    uint8_t indices[6] {
        0, 1, 2,
        2, 3, 0
    };

    ebo.bind();
    ebo.usage(lwvl::Usage::Static);
    ebo.construct(indices, 6);

    lwvl::VertexArray::clear();
    lwvl::ArrayBuffer::clear();
    lwvl::ElementBuffer::clear();
}

Quad::Quad(Quad&& other) noexcept : vao(other.vao), vbo(other.vbo), ebo(other.ebo) {}

Quad& Quad::operator=(Quad&& other) noexcept {
    vao = other.vao;
    vbo = other.vbo;
    ebo = other.ebo;
    return *this;
}

void Quad::draw() {
    vao.bind();
    vao.drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE);
}
