#include "EndPointCaster.h"

static constexpr float M_PI = 3.14159265358979323846f;
static constexpr float M_TAU = M_PI * 2.0f;
static constexpr unsigned int raysPerBound = 2 * 3;

inline unsigned int calculateRays(unsigned int numWalls) {
    return numWalls * raysPerBound;
}


// EndPointCaster
EndPointCaster::EndPointCaster(float x, float y, unsigned int numBounds): pos(x, y), vao(true) {
    const unsigned int neededRays = calculateRays(numBounds);
    rays.reserve(neededRays);

    const unsigned int bufferSize = 2 * (neededRays + 1);
    std::vector<float> positions(bufferSize);
    std::vector<unsigned int> indices(2 * neededRays);

    positions[0] = x;
    positions[1] = y;

    for (unsigned int i = 0; i < neededRays; i++) {
        rays.emplace_back(x, y, 0.0f);

        positions[(i + 1) * 2 + 0] = x;
        positions[(i + 1) * 2 + 1] = y;

        indices[i * 2 + 0] = 0;
        indices[i * 2 + 1] = i + 1;
    }

    // Construct array buffer.
    vao.constructArrayBuffer(bufferSize * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
    vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));

    // Construct index buffer.
    vao.constructIndexBuffer(2 * neededRays * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
}

void EndPointCaster::update(const float x, const float y) {
    pos.x = x;
    pos.y = y;
    
    const unsigned int numRays = rays.size();
    for (Ray& ray : rays) {
        ray.pos.x = x;
        ray.pos.y = y;
    }
}

void EndPointCaster::look(const std::vector<Boundary>& bounds) {
    // Add rays and lines to match the number of walls.
    const unsigned int currentRays = rays.size();
    const unsigned int neededRays = calculateRays(bounds.size());

    const unsigned int bufferSize = 2 * (neededRays + 1);
    std::vector<float> positions(bufferSize);
    positions[0] = pos.x;
    positions[1] = pos.y;

    if (neededRays > currentRays) {
        rays.reserve(neededRays);

        for (unsigned int i = currentRays; i < neededRays; i++) {
            rays.emplace_back(pos.x, pos.y, 0.0f);
        }

        std::vector<unsigned int> indices(2 * neededRays);
        for (unsigned int i = 0; i < neededRays; i++) {
            indices[i * 2 + 0] = 0;
            indices[i * 2 + 1] = i + 1;
        }
        
        // Make new, bigger buffers on the GPU.
        vao.constructArrayBuffer(
            bufferSize * sizeof(float), 
            positions.data(), 
            GL_DYNAMIC_DRAW
        );

        vao.constructIndexBuffer(
            2 * neededRays * sizeof(unsigned int), 
            indices.data(), GL_DYNAMIC_DRAW
        );
    }

    const unsigned int numRays = neededRays;
    const unsigned int numBounds = bounds.size();

    // Point the rays at the wall endpoints.
    for (unsigned int i = 0; i < numBounds; i++) {
        const LineSegment& line = bounds[i].line;

        const float angleA = std::atan2f(line.a.y - pos.y, line.a.x - pos.x);
        const float angleB = std::atan2f(line.b.y - pos.y, line.b.x - pos.x);
        const float angles[raysPerBound] = {
            angleA - 0.0001f, angleA, angleA + 0.0001f,
            angleB - 0.0001f, angleB, angleB + 0.0001f
        };

        for (unsigned int j = 0; j < raysPerBound; j++) {
            const float angle = angles[j];

            Ray& ray = rays[i * raysPerBound + j];

            ray.dir.x = std::cosf(angle);
            ray.dir.y = std::sinf(angle);
        }
    }
    
    std::vector<std::unique_ptr<Point>> intersections;
    intersections.reserve(bounds.size());
    for (unsigned int i = 0; i < numRays; i++) {
        const Ray& ray = rays[i];

        const unsigned int index = (i + 1) * 2;
        pushIntersections(ray, bounds, intersections);
        if (intersections.size()) {
            std::unique_ptr<Point> shortestPath = closestIntersection(ray, intersections);

            positions[index + 0] = shortestPath->x;
            positions[index + 1] = shortestPath->y;
        }

        else {
            positions[index + 0] = pos.x;
            positions[index + 1] = pos.y;
        }

        intersections.clear();
    }

    vao.setArrayData(0, bufferSize * sizeof(float), positions.data());
}

void EndPointCaster::draw() const {
    vao.drawElements(GL_LINES, 2 * rays.size(), GL_UNSIGNED_INT);
}


// Filled EndPointCaster
FilledEndPointCaster::FilledEndPointCaster(float x, float y, unsigned int numBounds): pos(x, y), vao(false) {
    const unsigned int neededRays = calculateRays(numBounds);
    rays.reserve(neededRays);

    const unsigned int bufferSize = 2 * (neededRays + 2);
    std::vector<float> positions(bufferSize);

    positions[0] = x;
    positions[1] = y;

    for (unsigned int i = 0; i < neededRays; i++) {
        rays.emplace_back(x, y, 0.0f);

        positions[(i + 1) * 2 + 0] = x;
        positions[(i + 1) * 2 + 1] = y;
    }

    positions[bufferSize - 2] = x;
    positions[bufferSize - 1] = y;

    // Construct array buffer.
    vao.constructArrayBuffer(bufferSize * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
    vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));
}

void FilledEndPointCaster::update(const float x, const float y) {
    pos.x = x;
    pos.y = y;

    for (Ray& ray : rays) {
        ray.pos.x = x;
        ray.pos.y = y;
    }
}

void FilledEndPointCaster::look(const std::vector<Boundary>& bounds) {
    // Add rays and lines to match the number of walls.
    const unsigned int currentRays = rays.size();
    const unsigned int neededRays = calculateRays(bounds.size());

    const unsigned int bufferSize = 2 * (neededRays + 2);
    std::vector<float> positions(bufferSize);
    positions[0] = pos.x;
    positions[1] = pos.y;

    if (neededRays > currentRays) {
        rays.reserve(neededRays);

        for (unsigned int i = currentRays; i < neededRays; i++) {
            rays.emplace_back(pos.x, pos.y, 0.0f);
        }

        // Make new, bigger buffers on the GPU.
        vao.constructArrayBuffer(
            bufferSize * sizeof(float),
            positions.data(),
            GL_DYNAMIC_DRAW
        );
    }

    const unsigned int numRays = neededRays;
    const unsigned int numBounds = bounds.size();

    std::vector<RayDirection> headings;
    headings.reserve(numRays);

    // Point the rays at the wall endpoints.
    for (unsigned int i = 0; i < numBounds; i++) {
        const LineSegment& line = bounds[i].line;

        const float angleA = std::fmodf(std::atan2f(line.a.y - pos.y, line.a.x - pos.x) + M_TAU, M_TAU);
        const float angleB = std::fmodf(std::atan2f(line.b.y - pos.y, line.b.x - pos.x) + M_TAU, M_TAU);
        const float angles[raysPerBound] = {
            angleA - 0.0001f, angleA, angleA + 0.0001f,
            angleB - 0.0001f, angleB, angleB + 0.0001f
        };

        for (unsigned int j = 0; j < raysPerBound; j++) {
            const float angle = angles[j];
            const unsigned int index = i * raysPerBound + j;
            
            headings.emplace_back(index, angle);
            Ray& ray = rays[index];

            ray.dir.x = std::cosf(angle);
            ray.dir.y = std::sinf(angle);
        }
    }

    std::sort(headings.begin(), headings.end());

    std::vector<std::unique_ptr<Point>> intersections;
    intersections.reserve(numBounds);
    
    for (unsigned int i = 0; i < numRays; i++) {
        const RayDirection& heading = headings[i];
        const unsigned int j = heading.index;

        const Ray& ray = rays[j];

        const unsigned int index = (i + 1) * 2;
        pushIntersections(ray, bounds, intersections);
        if (intersections.size()) {
            std::unique_ptr<Point> shortestPath = closestIntersection(ray, intersections);

            positions[index + 0] = shortestPath->x;
            positions[index + 1] = shortestPath->y;
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
    vao.drawArrays(GL_TRIANGLE_FAN, rays.size() + 2);
}

FilledEndPointCaster::RayDirection::RayDirection(const unsigned int index, const float angle): index(index), angle(angle) {}

bool FilledEndPointCaster::RayDirection::operator<(const RayDirection& other) {
    return angle < other.angle;
}
