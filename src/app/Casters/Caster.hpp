#pragma once

#include "pch.hpp"
#include "Math/Geometrics.hpp"

class __declspec(novtable) Caster {
public:
    virtual void update(float x, float y) = 0;

    virtual void look(const std::vector<LineSegment> &bounds) = 0;

    virtual void draw() = 0;
};

Point closestIntersection(const Ray &ray, std::vector<Point> intersections);

void pushIntersections(const Ray &ray, const std::vector<LineSegment> &bounds, std::vector<Point> &intersections);
