#include "pch.hpp"
#include "Floor.hpp"

Floor::Floor() {
    float positions[16] = {
        // positions | texture coordinates
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
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

Floor::Floor(float left, float bottom, float width, float height) :
    start(left, bottom), end(left + width, bottom + height) {
    float positions[16] = {
        // positions                 | texture coordinates
        left, bottom, 0.0f, 0.0f,
        left + width, bottom, 1.0f, 0.0f,
        left + width, bottom + height, 1.0f, 1.0f,
        left, bottom + height, 0.0f, 1.0f
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

Floor::Floor(Floor &&other) noexcept: vao(other.vao), vbo(other.vbo), ebo(other.ebo) {}

Floor &Floor::operator=(Floor &&other) noexcept {
    vao = other.vao;
    vbo = other.vbo;
    ebo = other.ebo;
    return *this;
}

void Floor::draw() {
    vao.bind();
    vao.drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE);
}

void Floor::update(float left, float bottom, float width, float height) {
    start = Point(left, bottom);
    end = Point(left + width, bottom + height);

    float positions[8] = {
        // positions
        left, bottom,
        left + width, bottom,
        left + width, bottom + height,
        left, bottom + height,
    };

    vbo.bind();

    // Update positions and skip updating texture coordinates.
    vbo.update(positions + 0, 2, 0);
    vbo.update(positions + 2, 2, 4);
    vbo.update(positions + 4, 2, 8);
    vbo.update(positions + 6, 2, 12);
}
