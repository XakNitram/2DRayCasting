#pragma once
#include <memory>
#include <vector>
#include "Caster.h"
#include "Geometrics.h"
#include "VertexArray.h"

class AngleCaster : public Caster {
	const static unsigned int numRays;

	Point pos;
	VertexArray vao;
	std::vector<Ray> rays;

public:
	AngleCaster(float x, float y);

	void update(const float x, const float y);

	void look(const std::vector<Boundary>& bounds);

	void draw() const;
};
