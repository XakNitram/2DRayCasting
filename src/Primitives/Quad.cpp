#include "pch.h"
#include "Quad.h"

Quad::Quad(float left, float bottom, float width, float height): vao(2 * sizeof(float), true) {
    float positions[8] {
        left,         bottom,
        left + width, bottom,
        left + width, bottom + height,
        left,         bottom + height
    };

    vao.constructArrayBuffer(sizeof(positions), positions, GL_STATIC_DRAW);
    vao.attachAttribute(2, GL_FLOAT, 0);

    unsigned int indices[6] {
        0, 1, 2,
        2, 3, 0
    };

    vao.constructIndexBuffer(sizeof(indices), indices, GL_STATIC_DRAW);
}

Quad::Quad(Quad&& other) noexcept : vao(std::move(other.vao)) {}

Quad& Quad::operator=(Quad&& other) noexcept {
    vao = std::move(other.vao);
    return *this;
}

void Quad::draw() const {
    vao.drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT);
}
