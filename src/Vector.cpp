#include "Vector.h"

Vector::Vector() : x(0.0), y(0.0) {}

Vector::Vector(double x, double y) : x(x), y(y) {}

Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

double Vector::distanceTo(const Vector& other) const {
	const double dx = other.x - x;
	const double dy = other.y - y;

	return dx * dx + dy * dy;
}
