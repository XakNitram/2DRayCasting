#include "pch.hpp"
#include "Floor.hpp"

Floor::Floor(float left, float bottom, float width, float height) {
    float positions[16] = {
        // positions                 | texture coordinates
        left,         bottom,          0.0f, 0.0f,
        left + width, bottom,          1.0f, 0.0f,
        left + width, bottom + height, 1.0f, 1.0f,
        left,         bottom + height, 0.0f, 1.0f
    };

    vao.bind();

    vbo.bind();
    vbo.usage(lwvl::Usage::Static);
    vbo.construct(positions, 16);

    vao.attribute(2, GL_FLOAT, 4 * sizeof(float), 0);
    vao.attribute(2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float));

    uint8_t indices[6] = {
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

Floor::Floor(Floor&& other) noexcept : vao(other.vao), vbo(other.vbo), ebo(other.ebo) {}

Floor& Floor::operator=(Floor&& other) noexcept {
    vao = other.vao;
    vbo = other.vbo;
    ebo = other.ebo;
    return *this;
}

void Floor::draw() {
    vao.bind();
    vao.drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE);
}
