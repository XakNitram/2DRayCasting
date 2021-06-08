#pragma once

#include "pch.hpp"

struct Point {
    float x, y;

    Point(float x, float y);

    Point();

    [[nodiscard]] float distanceTo(const Point &other) const;
};

struct Vector {
    float x, y;

    Vector();

    Vector(float x, float y);

    Vector(const Vector &other);

    [[nodiscard]] float angle() const;
};

struct LineSegment {
    Point a, b;

    LineSegment() = default;

    LineSegment(float x1, float y1, float x2, float y2);

    LineSegment(Point a, Point b);
};

struct Ray {
    Point pos;
    Vector dir;

    Ray(float x, float y, float angle);

    Ray();

    [[nodiscard]] std::optional<Point> intersects(const LineSegment &bound) const;
};
