#include "pch.hpp"
#include "EndPointCaster.hpp"

static constexpr float M_PI = 3.14159265358979323846f;
static constexpr float M_TAU = M_PI * 2.0f;
static constexpr float EPSILON = 0.0001f;
static constexpr unsigned int raysPerBound = 2 * 3;

inline unsigned int calculateRays(unsigned int numWalls) {
    return numWalls * raysPerBound;
}


// EndPointCaster
LineEndPointCaster::LineEndPointCaster(unsigned int numBounds) :
    pos(0.0f, 0.0f), currentRays(calculateRays(numBounds)) {
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

    vao.bind();
    // Construct array buffer.

    vbo.bind();
    vbo.usage(lwvl::Usage::Dynamic);
    vbo.construct(positions.begin(), positions.end());
    vao.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    // Construct index buffer.
    ebo.bind();
    ebo.usage(lwvl::Usage::Static);
    ebo.construct(indices.begin(), indices.end());

    lwvl::VertexArray::clear();
    lwvl::ArrayBuffer::clear();
    lwvl::ElementBuffer::clear();
}

void LineEndPointCaster::update(const float x, const float y) {
    pos.x = x;
    pos.y = y;
}

void LineEndPointCaster::look(const std::vector<LineSegment> &bounds) {
    // Add rays and lines to match the number of walls.
    const uint32_t neededRays = calculateRays(bounds.size());

    const uint32_t bufferSize = 2 * (neededRays + 1);
    std::vector<float> positions(bufferSize);
    positions[0] = pos.x;
    positions[1] = pos.y;

    if (neededRays > currentRays) {
        std::vector<uint32_t> indices(2 * neededRays);
        for (uint32_t i = 0; i < neededRays; i++) {
            indices[i * 2 + 0] = 0;
            indices[i * 2 + 1] = i + 1;
        }

        // Make new, bigger buffers on the GPU.
        vbo.bind();
        vbo.construct<float>(nullptr, bufferSize);

        ebo.bind();
        ebo.construct(indices.begin(), indices.end());
        currentRays = neededRays;
    }

    const uint32_t numRays = neededRays;
    const uint32_t numBounds = bounds.size();
    std::vector<float> headings(numRays);

    // Point the rays at the wall endpoints.
    for (uint32_t i = 0; i < numBounds; i++) {
        const LineSegment &line = bounds[i];

        const float angleA = std::atan2f(line.a.y - pos.y, line.a.x - pos.x);
        const float angleB = std::atan2f(line.b.y - pos.y, line.b.x - pos.x);
        const float angles[raysPerBound] = {
            angleA - EPSILON, angleA, angleA + EPSILON,
            angleB - EPSILON, angleB, angleB + EPSILON
        };

        for (uint32_t j = 0; j < raysPerBound; j++) {
            headings[i * raysPerBound + j] = angles[j];
        }
    }

    Ray ray(pos.x, pos.y, 0.0f);
    intersections.reserve(numBounds);
    for (uint32_t i = 0; i < numRays; i++) {
        const float angle = headings[i];
        ray.dir.x = std::cosf(angle);
        ray.dir.y = std::sinf(angle);

        const uint32_t index = (i + 1) * 2;
        pushIntersections(ray, bounds, intersections);
        if (!intersections.empty()) {
            Point shortestPath = closestIntersection(ray, intersections);

            positions[index + 0] = shortestPath.x;
            positions[index + 1] = shortestPath.y;
        } else {
            positions[index + 0] = pos.x;
            positions[index + 1] = pos.y;
        }

        intersections.clear();
    }

    vbo.bind();
    vbo.update(positions.begin(), positions.end());
}

void LineEndPointCaster::draw() {
    vao.bind();
    vao.drawElements(lwvl::PrimitiveMode::Lines, static_cast<int32_t>(2 * currentRays), lwvl::ByteFormat::UnsignedInt);
}


// Filled EndPointCaster
FilledEndPointCaster::FilledEndPointCaster(unsigned int numBounds) :
    pos(0.0f, 0.0f), currentRays(calculateRays(numBounds)) {
    const uint32_t neededRays = currentRays;
    const uint32_t bufferSize = 2 * (neededRays + 2);
    std::vector<float> positions(bufferSize);

    positions[0] = pos.x;
    positions[1] = pos.y;

    for (unsigned int i = 0; i < neededRays; i++) {
        positions[(i + 1) * 2 + 0] = pos.x;
        positions[(i + 1) * 2 + 1] = pos.y;
    }

    positions[bufferSize - 2] = pos.x;
    positions[bufferSize - 1] = pos.y;

    vao.bind();

    // Construct array buffer.
    vbo.bind();
    vbo.usage(lwvl::Usage::Dynamic);
    vbo.construct(positions.begin(), positions.end());
    vao.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    lwvl::VertexArray::clear();
    lwvl::ArrayBuffer::clear();
}

void FilledEndPointCaster::update(const float x, const float y) {
    pos.x = x;
    pos.y = y;
}

void FilledEndPointCaster::look(const std::vector<LineSegment> &bounds) {
    // Add rays and lines to match the number of walls.
    const uint32_t neededRays = calculateRays(bounds.size());

    const uint32_t bufferSize = 2 * (neededRays + 2);
    std::vector<float> positions(bufferSize);
    positions[0] = pos.x;
    positions[1] = pos.y;

    if (neededRays > currentRays) {
        // Make new, bigger buffers on the GPU.
        vbo.bind();
        vbo.construct<float>(nullptr, bufferSize * sizeof(float));

        currentRays = neededRays;
    }

    const uint32_t numRays = neededRays;
    const uint32_t numBounds = bounds.size();

    std::vector<float> headings(numRays);

    // Point the rays at the wall endpoints.
    for (uint32_t i = 0; i < numBounds; i++) {
        const LineSegment &line = bounds[i];

        const float angleA = std::fmodf(std::atan2f(line.a.y - pos.y, line.a.x - pos.x) + M_TAU, M_TAU);
        const float angleB = std::fmodf(std::atan2f(line.b.y - pos.y, line.b.x - pos.x) + M_TAU, M_TAU);
        const float angles[raysPerBound] = {
            angleA - EPSILON, angleA, angleA + EPSILON,
            angleB - EPSILON, angleB, angleB + EPSILON
        };

        for (uint32_t j = 0; j < raysPerBound; j++) {
            const uint32_t index = i * raysPerBound + j;
            headings[index] = angles[j];
        }
    }

    std::sort(headings.begin(), headings.end());

    Ray ray(pos.x, pos.y, 0.0f);
    intersections.reserve(numBounds);

    for (uint32_t i = 0; i < numRays; i++) {
        const float angle = headings[i];
        ray.dir.x = std::cosf(angle);
        ray.dir.y = std::sinf(angle);

        const uint32_t index = (i + 1) * 2;
        pushIntersections(ray, bounds, intersections);
        if (!intersections.empty()) {
            Point shortestPath = closestIntersection(ray, intersections);

            positions[index + 0] = shortestPath.x;
            positions[index + 1] = shortestPath.y;
        } else {
            positions[index + 0] = pos.x;
            positions[index + 1] = pos.y;
        }

        intersections.clear();
    }

    positions[bufferSize - 2] = positions[2];
    positions[bufferSize - 1] = positions[3];

    vbo.bind();
    vbo.update(positions.begin(), positions.end());
}

void FilledEndPointCaster::draw() {
    vao.bind();
    vao.drawArrays(lwvl::PrimitiveMode::TriangleFan, int32_t(currentRays + 2));
}
