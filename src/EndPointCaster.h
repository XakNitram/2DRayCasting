#pragma once
#include <algorithm>
#include "Caster.h"


class EndPointCaster : public Caster {
    Point pos;
    VertexArray vao;
    unsigned int currentRays;
    std::vector<std::unique_ptr<Point>> intersections;

public:
    EndPointCaster(float x, float y, unsigned int numBounds);

    void update(const float x, const float y);

    void look(const std::vector<Boundary>& bounds);

    void draw() const;
};


class FilledEndPointCaster : public Caster {
    Point pos;
    VertexArray vao;
    unsigned int currentRays;
    std::vector<std::unique_ptr<Point>> intersections;

public:
    FilledEndPointCaster(float x, float y, unsigned int numBounds);

    void update(const float x, const float y);

    void look(const std::vector<Boundary>& bounds);

    void draw() const;
};
