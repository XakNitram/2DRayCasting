#include "Boundary.h"

Boundary::Boundary(float x1, float y1, float x2, float y2): line(x1, y1, x2, y2), vao(true) {
	float positions[4] = { x1, y1, x2, y2 };

	// upload vertex position data
	vao.constructArrayBuffer(sizeof(positions), positions, GL_STATIC_DRAW);
	vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));
}

Boundary::Boundary(Boundary&& other) noexcept: line(other.line), vao(std::move(other.vao)) {}

void Boundary::draw() const {
	vao.drawArrays(GL_LINES, 2);
}
