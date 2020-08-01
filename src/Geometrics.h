#pragma once
#include <memory>
#include <cmath>

struct Point {
	float x, y;

	Point(float x, float y);
	Point();

	float distanceTo(const Point& other) const;
};

struct Vector {
	float x, y;

	Vector();
	Vector(float x, float y);
	Vector(const Vector& other);

	float angle() const;
};

struct LineSegment {
	Point a, b;

	LineSegment(float x1, float y1, float x2, float y2);
};

struct Ray {
	Point pos;
	Vector dir;
	
	Ray(float x, float y, float angle);
	Ray();

	std::unique_ptr<Point> intersects(const LineSegment& bound) const;
};
