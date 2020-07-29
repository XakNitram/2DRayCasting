#include "Vector.h"

Vector::Vector() : x(0.0f), y(0.0f) {}

Vector::Vector(float x, float y) : x(x), y(y) {}

Vector::Vector(const Vector& other) : x(other.x), y(other.y) {}

float Vector::distanceTo(const Vector& other) const {
	const float dx = other.x - x;
	const float dy = other.y - y;

	return dx * dx + dy * dy;
}
