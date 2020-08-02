#pragma once
#include <vector>
#include <memory>
#include "Boundary.h"

class __declspec(novtable) Caster {
public:
	virtual void update(const float x, const float y) = 0;
	virtual void look(const std::vector<Boundary>& bounds) = 0;
	virtual void draw() const = 0;
};

std::unique_ptr<Point> closestIntersection(const Ray& ray, std::vector<std::unique_ptr<Point>>& intersections);

void pushIntersections(const Ray& ray, const std::vector<Boundary>& bounds, std::vector<std::unique_ptr<Point>>& intersections);
