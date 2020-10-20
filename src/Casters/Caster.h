#pragma once
#include "rcpch.h"
#include "Math/Geometrics.h"
#include "Math/Boundary.h"

class __declspec(novtable) Caster {
public:
	virtual void update(const float x, const float y) = 0;
	virtual void look(const std::vector<Boundary>& bounds) = 0;
	virtual void draw() const = 0;
};

Point closestIntersection(const Ray& ray, std::vector<Point> intersections);

void pushIntersections(const Ray& ray, const std::vector<Boundary>& bounds, std::vector<Point>& intersections);
