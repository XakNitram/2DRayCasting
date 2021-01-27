#include "pch.hpp"
#include "Geometrics.hpp"

// Point
Point::Point(float x, float y): x(x), y(y) {}

Point::Point(): x(0.0f), y(0.0f) {}

float Point::distanceTo(const Point& other) const {
	const float dx = other.x - x;
	const float dy = other.y - y;

	return dx * dx + dy * dy;
}


// Vector
Vector::Vector() : x(0.0f), y(0.0f) {}

Vector::Vector(float x, float y) : x(x), y(y) {}

Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

float Vector::angle() const {
	return std::atan2f(y, x);
}


// Line
LineSegment::LineSegment(float x1, float y1, float x2, float y2) : a(x1, y1), b(x2, y2) {}


// Ray
Ray::Ray(float x, float y, float angle) : pos(x, y), dir(std::cosf(angle), std::sinf(angle)) {}

Ray::Ray() : pos(0.0f, 0.0f), dir(1.0f, 0.0f) {}

std::optional<Point> Ray::intersects(const LineSegment& segment) const {
    const float x1 = segment.a.x;
    const float y1 = segment.a.y;
    const float x2 = segment.b.x;
    const float y2 = segment.b.y;

    const float x3 = pos.x;
    const float y3 = pos.y;
    const float x4 = x3 + dir.x;
    const float y4 = y3 + dir.y;
   
    const float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (den == 0.0f) {
        return std::nullopt;
    }

    const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
    const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

    if ((t >= 0.0f && t <= 1.0f) && u >= 0.0f) {
        return std::make_optional<Point>(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
    }

    else {
        return std::nullopt;
    }
}
