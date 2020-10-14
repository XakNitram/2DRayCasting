#include "rcpch.h"
#include "Floor.h"

Floor::Floor(float left, float bottom, float width, float height): vao(4 * sizeof(float)) {
    float positions[16] = {
        // positions                 | texture coordinates
        left,         bottom,          0.0f, 0.0f,
        left + width, bottom,          1.0f, 0.0f,
        left + width, bottom + height, 1.0f, 1.0f,
        left,         bottom + height, 0.0f, 1.0f
    };

    vao.constructArrayBuffer(sizeof(positions), positions, GL_STATIC_DRAW);
    vao.attachAttribute(2, GL_FLOAT, 0);
    vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));

    unsigned int indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    vao.constructIndexBuffer(sizeof(indices), indices, GL_STATIC_DRAW);
}

Floor::Floor(Floor&& other) noexcept : vao(std::move(other.vao)) {}

Floor& Floor::operator=(Floor&& other) noexcept {
    vao = std::move(other.vao);
    return *this;
}

void Floor::draw() const {
    vao.drawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT);
}
