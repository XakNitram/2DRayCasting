#include "pch.hpp"
#include "Caster.hpp"

Point closestIntersection(const Ray& ray, std::vector<Point> intersections) {
	const unsigned int numIntersections = intersections.size();
	Point shortestPath = intersections[0];

	for (unsigned int i = 1; i < numIntersections; i++) {
		const Point& intersection = intersections[i];

		if (ray.pos.distanceTo(intersection) < ray.pos.distanceTo(shortestPath)) {
			shortestPath = intersection;
		}
	}

	return shortestPath;
}


void pushIntersections(
	const Ray& ray, const std::vector<LineSegment>& bounds,
	std::vector<Point>& intersections
) {
	for (const LineSegment& bound : bounds) {
		if (auto intersection = ray.intersects(bound)) {
			intersections.push_back(intersection.value());
		}
	}
}
