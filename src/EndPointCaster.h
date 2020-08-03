#pragma once
#include <algorithm>
#include "Caster.h"


class EndPointCaster : public Caster {
    Point pos;
    VertexArray vao;
    std::vector<Ray> rays;

public:
    EndPointCaster(float x, float y, unsigned int numBounds);

    void update(const float x, const float y);

    void look(const std::vector<Boundary>& bounds);

    void draw() const;
};


class FilledEndPointCaster : public Caster {
    struct RayDirection {
        unsigned int index;
        float angle;

        RayDirection(const unsigned int index, const float angle);
        bool operator<(const RayDirection & other);
    };

    Point pos;
    VertexArray vao;
    std::vector<Ray> rays;

public:
    FilledEndPointCaster(float x, float y, unsigned int numBounds);

    void update(const float x, const float y);

    void look(const std::vector<Boundary>& bounds);

    void draw() const;
};
