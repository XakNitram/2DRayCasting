#include "Boundary.h"

Boundary::Boundary(float x1, float y1, float x2, float y2): line(x1, y1, x2, y1), vao(true) {
	float positions[4] = { x1, y1, x2, y2 };

	// upload vertex position data
	vao.constructArrayBuffer(sizeof(positions), positions, GL_STATIC_DRAW);
	vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));
}

void Boundary::draw() const {
	vao.drawArrays(GL_LINES, 2);
}
