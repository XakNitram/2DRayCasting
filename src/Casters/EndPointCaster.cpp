#include "rcpch.h"
#include "EndPointCaster.h"

static constexpr float M_PI = 3.14159265358979323846f;
static constexpr float M_TAU = M_PI * 2.0f;
static constexpr float EPSILON = 0.0001f;
static constexpr unsigned int raysPerBound = 2 * 3;

inline unsigned int calculateRays(unsigned int numWalls) {
    return numWalls * raysPerBound;
}


// EndPointCaster
LineEndPointCaster::LineEndPointCaster(unsigned int numBounds): pos(0.0f, 0.0f), vao(2 * sizeof(float), true), currentRays(calculateRays(numBounds)) {
    const unsigned int neededRays = currentRays;
    const unsigned int bufferSize = 2 * (neededRays + 1);
    std::vector<float> positions(bufferSize);
    std::vector<unsigned int> indices(2 * neededRays);

    positions[0] = pos.x;
    positions[1] = pos.x;

    for (unsigned int i = 0; i < neededRays; i++) {
        positions[(i + 1) * 2 + 0] = pos.x;
        positions[(i + 1) * 2 + 1] = pos.y;

        indices[i * 2 + 0] = 0;
        indices[i * 2 + 1] = i + 1;
    }

    // Construct array buffer.
    vao.constructArrayBuffer(bufferSize * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
    vao.attachAttribute(2, GL_FLOAT, 0);

    // Construct index buffer.
    vao.constructIndexBuffer(2 * neededRays * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
}

void LineEndPointCaster::update(const float x, const float y) {
    pos.x = x;
    pos.y = y;
}

void LineEndPointCaster::look(const std::vector<Boundary>& bounds) {
    // Add rays and lines to match the number of walls.
    const unsigned int neededRays = calculateRays(bounds.size());

    const unsigned int bufferSize = 2 * (neededRays + 1);
    std::vector<float> positions(bufferSize);
    positions[0] = pos.x;
    positions[1] = pos.y;

    if (neededRays > currentRays) {
        std::vector<unsigned int> indices(2 * neededRays);
        for (unsigned int i = 0; i < neededRays; i++) {
            indices[i * 2 + 0] = 0;
            indices[i * 2 + 1] = i + 1;
        }
        
        // Make new, bigger buffers on the GPU.
        vao.constructArrayBuffer(
            bufferSize * sizeof(float), 
            nullptr, 
            GL_DYNAMIC_DRAW
        );

        vao.constructIndexBuffer(
            2 * neededRays * sizeof(unsigned int), 
            indices.data(), GL_DYNAMIC_DRAW
        );

        currentRays = neededRays;
    }

    const unsigned int numRays = neededRays;
    const unsigned int numBounds = bounds.size();
    std::vector<float> headings(numRays);

    // Point the rays at the wall endpoints.
    for (unsigned int i = 0; i < numBounds; i++) {
        const LineSegment& line = bounds[i].line;

        const float angleA = std::atan2f(line.a.y - pos.y, line.a.x - pos.x);
        const float angleB = std::atan2f(line.b.y - pos.y, line.b.x - pos.x);
        const float angles[raysPerBound] = {
            angleA - EPSILON, angleA, angleA + EPSILON,
            angleB - EPSILON, angleB, angleB + EPSILON
        };

        for (unsigned int j = 0; j < raysPerBound; j++) {
            headings[i * raysPerBound + j] = angles[j];
        }
    }
    
    Ray ray(pos.x, pos.y, 0.0f);
    intersections.reserve(numBounds);
    for (unsigned int i = 0; i < numRays; i++) {
        const float angle = headings[i];
        ray.dir.x = std::cosf(angle);
        ray.dir.y = std::sinf(angle);

        const unsigned int index = (i + 1) * 2;
        pushIntersections(ray, bounds, intersections);
        if (intersections.size()) {
            Point shortestPath = closestIntersection(ray, intersections);

            positions[index + 0] = shortestPath.x;
            positions[index + 1] = shortestPath.y;
        }

        else {
            positions[index + 0] = pos.x;
            positions[index + 1] = pos.y;
        }

        intersections.clear();
    }

    vao.setArrayData(0, bufferSize * sizeof(float), positions.data());
}

void LineEndPointCaster::draw() const {
    vao.drawElements(GL_LINES, 2 * currentRays, GL_UNSIGNED_INT);
}


// Filled EndPointCaster
FilledEndPointCaster::FilledEndPointCaster(unsigned int numBounds): pos(0.0f, 0.0f), vao(2 * sizeof(float), false), currentRays(calculateRays(numBounds)) {
    const unsigned int neededRays = currentRays;
    const unsigned int bufferSize = 2 * (neededRays + 2);
    std::vector<float> positions(bufferSize);

    positions[0] = pos.x;
    positions[1] = pos.y;

    for (unsigned int i = 0; i < neededRays; i++) {
        positions[(i + 1) * 2 + 0] = pos.x;
        positions[(i + 1) * 2 + 1] = pos.y;
    }

    positions[bufferSize - 2] = pos.x;
    positions[bufferSize - 1] = pos.y;

    // Construct array buffer.
    vao.constructArrayBuffer(bufferSize * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
    vao.attachAttribute(2, GL_FLOAT, 0);
}

void FilledEndPointCaster::update(const float x, const float y) {
    pos.x = x;
    pos.y = y;
}

void FilledEndPointCaster::look(const std::vector<Boundary>& bounds) {
    // Add rays and lines to match the number of walls.
    const unsigned int neededRays = calculateRays(bounds.size());

    const unsigned int bufferSize = 2 * (neededRays + 2);
    std::vector<float> positions(bufferSize);
    positions[0] = pos.x;
    positions[1] = pos.y;

    if (neededRays > currentRays) {
        // Make new, bigger buffers on the GPU.
        vao.constructArrayBuffer(
            bufferSize * sizeof(float),
            nullptr,
            GL_DYNAMIC_DRAW
        );

        currentRays = neededRays;
    }

    const unsigned int numRays = neededRays;
    const unsigned int numBounds = bounds.size();

    std::vector<float> headings(numRays);

    // Point the rays at the wall endpoints.
    for (unsigned int i = 0; i < numBounds; i++) {
        const LineSegment& line = bounds[i].line;

        const float angleA = std::fmodf(std::atan2f(line.a.y - pos.y, line.a.x - pos.x) + M_TAU, M_TAU);
        const float angleB = std::fmodf(std::atan2f(line.b.y - pos.y, line.b.x - pos.x) + M_TAU, M_TAU);
        const float angles[raysPerBound] = {
            angleA - EPSILON, angleA, angleA + EPSILON,
            angleB - EPSILON, angleB, angleB + EPSILON
        };

        for (unsigned int j = 0; j < raysPerBound; j++) {
            const unsigned int index = i * raysPerBound + j;
            headings[index] = angles[j];
        }
    }

    std::sort(headings.begin(), headings.end());

    Ray ray(pos.x, pos.y, 0.0f);
    intersections.reserve(numBounds);
    
    for (unsigned int i = 0; i < numRays; i++) {
        const float angle = headings[i];
        ray.dir.x = std::cosf(angle);
        ray.dir.y = std::sinf(angle);

        const unsigned int index = (i + 1) * 2;
        pushIntersections(ray, bounds, intersections);
        if (intersections.size()) {
            Point shortestPath = closestIntersection(ray, intersections);

            positions[index + 0] = shortestPath.x;
            positions[index + 1] = shortestPath.y;
        }

        else {
            positions[index + 0] = pos.x;
            positions[index + 1] = pos.y;
        }

        intersections.clear();
    }

    positions[bufferSize - 2] = positions[2];
    positions[bufferSize - 1] = positions[3];

    vao.setArrayData(0, bufferSize * sizeof(float), positions.data());
}

void FilledEndPointCaster::draw() const {
    vao.drawArrays(GL_TRIANGLE_FAN, currentRays + 2);
}
