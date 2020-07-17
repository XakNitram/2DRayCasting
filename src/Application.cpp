#include <cmath>
#include <memory>
#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Vector.h"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156

constexpr float M_PI = 3.14159265358979323846f;
constexpr float M_PI_30 = M_PI / 30.0f;
constexpr float M_TAU = M_PI * 2.0f;


class Line {
public:
    unsigned int vao, vbo;
private:
    void setData(float x1, float y1, float x2, float y2) {
        float newPositions[] = { x1, y1, x2, y2 };

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newPositions), newPositions);
    }

    void setStart(float x1, float y1) {
        float newStart[] = { x1, y1 };

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(float), newStart);
    }

    void setEnd(float x2, float y2) {
        float newEnd[] = { x2, y2 };

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(float), 2 * sizeof(float), newEnd);
    }

    void setup(float x1, float y1, float x2, float y2, GLenum usage) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        float positions[] = { x1, y1, x2, y2 };
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, usage);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    }
    
public:
    Line(const Vector& start, const Vector& end) { setup(start.x, start.y, end.x, end.y, GL_STATIC_DRAW); }
    Line(const Vector& start, const Vector& end, GLenum usage) { setup(start.x, start.y, end.x, end.y, usage); }
    
    Line(float x1, float y1, float x2, float y2) { setup(x1, y1, x2, y2, GL_STATIC_DRAW); }
    Line(float x1, float y1, float x2, float y2, GLenum usage) { setup(x1, y1, x2, y2, usage); }

    ~Line() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void update(const Vector& start, const Vector& end) { setData(start.x, start.y, end.x, end.y); }
    void update(float x1, float y1, float x2, float y2) { setData(x1, y1, x2, y2); }

    void updateStart(const Vector& start) { setStart(start.x, start.y); }
    void updateStart(float x1, float y1) { setStart(x1, y1); }

    void updateEnd(const Vector& end) { setEnd(end.x, end.y); }
    void updateEnd(float x2, float y2) { setEnd(x2, y2); }

    void draw() const {
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 2);
    }
};


struct Boundary {
    Line m_line;
    Vector a, b;

    Boundary(float x1, float y1, float x2, float y2) : m_line(x1, y1, x2, y2), a({ x1, y1 }), b({ x2, y2 }) {}

    void show() const {
        m_line.draw();
    }
};


struct Ray {
    Vector pos, dir;

    Ray(float x, float y, float angle) : pos({ x, y }), dir({ std::cosf(angle), std::sinf(angle) }) {}

    //Ray(const Ray& other) = delete;
    Ray(const Ray& other) : pos({ other.pos.x, other.pos.y }), dir({ other.dir.x, other.dir.y }) {
        std::cout << "Copied a ray" << std::endl;
    }

    std::unique_ptr<Vector> cast(const Boundary& wall) const {
        const float x1 = wall.a.x;
        const float y1 = wall.a.y;
        const float x2 = wall.b.x;
        const float y2 = wall.b.y;

        const float x3 = pos.x;
        const float y3 = pos.y;
        const float x4 = pos.x + dir.x;
        const float y4 = pos.y + dir.y;

        const float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) {
            return nullptr;
        }

        const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

        if ((t > 0.0f && t < 1.0f) && u > 0.0f) {
            return std::make_unique<Vector>(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
        }
        else {
            return nullptr;
        }
    }
};


class LineCaster {
    static const unsigned int numRays = 90;
public:
    Vector pos;
private:
    std::vector<Ray> rays;
    std::vector<Line> lines;
public:
    LineCaster(float x, float y) : pos({ x, y }) {
        rays.reserve(numRays);
        lines.reserve(numRays);
        
        for (unsigned int i = 0; i < numRays; i++) {
            const float angle = float(i) * (M_TAU / float(numRays));
            rays.emplace_back(x, y, angle);
            
            lines.emplace_back(pos, pos, GL_DYNAMIC_DRAW);
        }
    }

    void update(const Vector& newPos) {
        pos = newPos;

        const float radius = 10.0f;
        for (unsigned int i = 0; i < numRays; i++) {
            const float angle = float(i) * (M_TAU / float(numRays));
            const Vector newStart(
                radius * std::cos(angle) + newPos.x, 
                radius * std::sin(angle) + newPos.y
            );

            lines[i].update(newStart, newStart);
            rays[i].pos = newStart;
        }
    }

    void look(const std::vector<Boundary>& walls) {
        for (unsigned int i = 0; i < numRays; i++) {
            const Ray& ray = rays[i];

            std::vector<std::unique_ptr<Vector>> intersections;
            intersections.reserve(walls.size());
            for (const Boundary& wall : walls) {
                auto intersection = ray.cast(wall);
                if (intersection) {
                    intersections.push_back(std::move(intersection));
                }
            }

            unsigned int numIntersections = intersections.size();
            if (numIntersections) {
                std::unique_ptr<Vector> shortestPath = std::move(intersections.back());
                intersections.pop_back();
                
                numIntersections--;
                
                for (unsigned int i = 0; i < numIntersections; i++) {
                    const auto& intersection = intersections.back();

                    if (ray.pos.distanceTo(*intersection) < ray.pos.distanceTo(*shortestPath)) {
                        shortestPath = std::move(intersections.back());
                    }
                    intersections.pop_back();
                }

                lines[i].updateEnd(*shortestPath);
            }
        }
    }

    void show() {
        for (const Line& line : lines) {
            line.draw();
        }
    }
};


int main(void) {
    /* ****** Setup ****** */
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(400, 400, "Raycasting", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLEW */
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    /* Output GL Version */
    std::cout << glGetString(GL_VERSION) << std::endl;

    Shader lineShader("Shaders/line.vert", "Shaders/line.frag");
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    Location u_projection = lineShader.uniformLocation("u_Projection");
    if (u_projection) {

        float worldMatrix[] = {
            2.0f / width, 0.0f, 0.0f, 0.0f,
            0.0f,         2.0f / height, 0.0f, 0.0f,
            0.0f,         0.0f, 1.0f, 0.0f,
            -1.0f,       -1.0f, 0.0f, 1.0f
        };

        lineShader.uniformMatrix4fv(u_projection, worldMatrix);
    }

    /* ****** Main code below ****** */
    std::vector<Boundary> bounds;
    bounds.reserve(8);

    // Exterior Bounds
    bounds.emplace_back(10.0f, 10.0f, 390.0f, 10.0f);
    bounds.emplace_back(390.0f, 10.0f, 390.0f, 390.0f);
    bounds.emplace_back(390.0f, 390.0f, 10.0f, 390.0f);
    bounds.emplace_back(10.0f, 390.0f, 10.0f, 10.0f);

    // Interior Bounds
    bounds.emplace_back(100.0f, 100.0f, 100.0f, 300.0f);
    bounds.emplace_back(300.0f, 100.0f, 300.0f, 300.0f);
    
    bounds.emplace_back(150.0f, 150.0f, 250.0f, 250.0f);
    bounds.emplace_back(250.0f, 150.0f, 150.0f, 250.0f);
    
    // Particle to follow the mouse
    LineCaster entity(float(width) / 2.0f, float(height) / 2.0f);

    double oldMouseX = 0.0;
    double oldMouseY = 0.0;

    lineShader.bind();
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        mousePosY = width - mousePosY;
        
        if (!(mousePosX == oldMouseX && mousePosY == oldMouseY)) {
            if (!(mousePosX < 10.0f || mousePosX > 390.0f || mousePosY < 10.0f || mousePosY > 390.0f)) {
                entity.update({ float(mousePosX), float(mousePosY) });
                entity.look(bounds);
            }
        }

        oldMouseX = mousePosX;
        oldMouseY = mousePosY;
        

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        for (const Boundary& wall : bounds) {
            wall.show();
        }
        
        entity.show();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}