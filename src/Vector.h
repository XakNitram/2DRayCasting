#pragma once

struct Vector {
	float x, y;

	Vector();
	Vector(float x, float y);
	Vector(const Vector& other);

	float distanceTo(const Vector& other) const;
};

