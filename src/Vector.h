#pragma once

struct Vector {
	double x, y;

	Vector();
	Vector(double x, double y);
	Vector(const Vector& other);

	double distanceTo(const Vector& other) const;
};

