#include "AngleCaster.h"

static constexpr float M_PI = 3.14159265358979323846f;
static constexpr float M_TAU = M_PI * 2.0f;

const unsigned int AngleCaster::numRays = 64;

AngleCaster::AngleCaster(float x, float y): pos(x, y), vao(true) {
	rays.reserve(numRays);

	float positions[2 * (numRays + 1)];
	unsigned int indices[2 * numRays];

	positions[0] = pos.x;
	positions[1] = pos.y;

	const float slice = M_TAU / float(numRays);
	for (unsigned int i = 0; i < numRays; i++) {
		rays.emplace_back(pos.x, pos.y, float(i) * slice);

		positions[(i + 1) * 2 + 0] = 0.0f;
		positions[(i + 1) * 2 + 1] = 0.0f;

		indices[i * 2 + 0] = 0;
		indices[i * 2 + 1] = i + 1;
	}

	// Construct array buffer.
	vao.constructArrayBuffer(2 * (numRays + 1) * sizeof(float), positions, GL_DYNAMIC_DRAW);
	vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));

	// Construct index buffer.
	vao.constructIndexBuffer(2 * numRays * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
}

void AngleCaster::update(const float x, const float y) {
	pos.x = x;
	pos.y = y;

	float positions[2] = { x, y };

	for (unsigned int i = 0; i < numRays; i++) {
		Ray& ray = rays[i];
		ray.pos.x = x;
		ray.pos.y = y;
	}

	vao.setArrayData(0, 2 * sizeof(float), positions);
}

void AngleCaster::look(const std::vector<Boundary>& bounds) {
	float positions[2 * numRays];
	const unsigned int numBounds = bounds.size();

	std::vector<std::unique_ptr<Point>> intersections;
	intersections.reserve(numBounds);
	
	// Iterate over all rays to find where they intersect.
	for (unsigned int i = 0; i < numRays; i++) {
		const Ray& ray = rays[i];

		unsigned int boundCount = 0;
		for (const Boundary& bound : bounds) {
			auto intersection = ray.intersects(bound.line);
			if (intersection) {
				intersections.push_back(std::move(intersection));
			}
		}

		const unsigned int numIntersections = intersections.size();
		if (numIntersections) {
			std::unique_ptr<Point> shortestPath = nullptr;

			for (unsigned int j = 0; j < numIntersections; j++) {
				const std::unique_ptr<Point>& intersection = intersections[j];

				if (!j || ray.pos.distanceTo(*intersection) < ray.pos.distanceTo(*shortestPath)) {
					shortestPath = std::move(intersections[j]);
				}
			}

			//std::cout << shortestPath->x << ", " << shortestPath->y << std::endl;

			positions[i * 2 + 0] = shortestPath->x;
			positions[i * 2 + 1] = shortestPath->y;
		}

		else {
			positions[i * 2 + 0] = 50.0f * ray.dir.x + pos.x;
			positions[i * 2 + 1] = 50.0f * ray.dir.y + pos.y;
		}
		
		intersections.clear();
	}

	vao.setArrayData(2 * sizeof(float), 2 * numRays * sizeof(float), positions);
}

void AngleCaster::draw() const {
	vao.drawElements(GL_LINES, 2 * numRays, GL_UNSIGNED_INT);
}
