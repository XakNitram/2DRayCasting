#include "Caster.h"

std::unique_ptr<Point> closestIntersection(const Ray& ray, std::vector<std::unique_ptr<Point>>& intersections) {
	const unsigned int numIntersections = intersections.size();
	std::unique_ptr<Point> shortestPath = nullptr;

	for (unsigned int i = 0; i < numIntersections; i++) {
		const std::unique_ptr<Point>& intersection = intersections[i];

		if (!i || ray.pos.distanceTo(*intersection) < ray.pos.distanceTo(*shortestPath)) {
			shortestPath = std::move(intersections[i]);
		}
	}

	return std::move(shortestPath);
}


void pushIntersections(
	const Ray& ray, const std::vector<Boundary>& bounds, 
	std::vector<std::unique_ptr<Point>>& intersections
) {
	for (const Boundary& bound : bounds) {
		auto intersection = ray.intersects(bound.line);
		if (intersection) {
			intersections.push_back(std::move(intersection));
		}
	}
}
