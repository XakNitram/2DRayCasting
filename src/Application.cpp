#include <cmath>
#include <memory>
#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Vector.h"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156

constexpr double M_PI = 3.14159265358979323846;
constexpr double M_TAU = M_PI * 2.0;


class Line {
    VertexArray vao;

    inline void setup(double x1, double y1, double x2, double y2, GLenum usage) {
        float positions[4] = {
            float(x1), float(y1),
            float(x2), float(y2)
        };

        vao.constructBuffer(4 * sizeof(float), positions, usage);
        vao.attachAttribute(2, GL_FLOAT, 2 * sizeof(float));
    }
    
public:
    Line(const Vector& start, const Vector& end) { setup(start.x, start.y, end.x, end.y, GL_STATIC_DRAW); }
    Line(const Vector& start, const Vector& end, GLenum usage) { setup(start.x, start.y, end.x, end.y, usage); }

    Line(const Line& other) {}
    Line(Line&& other) noexcept : vao(std::move(other.vao)) {}

    void update(const Vector& start, const Vector& end) {
        float positions[4] = {
            float(start.x), float(start.y),
            float(end.x), float(end.y)
        };

        vao.setData(0, sizeof(positions), positions);
    }

    void updateStart(const Vector& start) {
        float positions[2] = {
            float(start.x), float(start.y)
        };

        vao.setData(0, sizeof(positions), positions);
    }

    void updateEnd(const Vector& end) {
        float positions[2] = {
            float(end.x), float(end.y)
        };

        vao.setData(2 * sizeof(float), sizeof(positions), positions);
    }

    void draw() const {
        vao.draw(GL_LINES, 2);
    }
};


struct Boundary {
    Vector a, b;
    Line m_line;

    Boundary(double x1, double y1, double x2, double y2) : a({ x1, y1 }), b({ x2, y2 }), m_line(a, b) {}
    // need a noexcept move constructor here for m_line

    void show() const {
        m_line.draw();
    }
};


struct Ray {
    Vector pos, dir;

    Ray(double x, double y, double angle) : pos({ x, y }), dir({ std::cos(angle), std::sin(angle) }) {}

    // Can't delete the copy and move constructors because std::vector references them _just in case_
    // Ray(const Ray& other) = delete;
    Ray(const Ray& other) : pos({ other.pos.x, other.pos.y }), dir({ other.dir.x, other.dir.y }) {
        std::cout << "Copied a ray" << std::endl;
    }

    std::unique_ptr<Vector> cast(const Boundary& wall) const {
        const double x1 = wall.a.x;
        const double y1 = wall.a.y;
        const double x2 = wall.b.x;
        const double y2 = wall.b.y;

        const double x3 = pos.x;
        const double y3 = pos.y;
        const double x4 = pos.x + dir.x;
        const double y4 = pos.y + dir.y;

        const double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (den == 0) {
            return nullptr;
        }

        const double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        const double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

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
    LineCaster(double x, double y) : pos({ x, y }) {
        rays.reserve(numRays);
        lines.reserve(numRays);
        
        const double slice = (M_TAU / double(numRays));
        for (unsigned int i = 0; i < numRays; i++) {
            const double angle = double(i) * slice;
            rays.emplace_back(x, y, angle);
            
            lines.emplace_back(pos, pos, GL_DYNAMIC_DRAW);
        }
    }

    void update(const Vector& newPos) {
        pos = newPos;

        const double radius = 10.0;
        const double slice = (M_TAU / double(numRays));
        for (unsigned int i = 0; i < numRays; i++) {
            const double angle = double(i) * slice;
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

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

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
            2.0f / float(width), 0.0f, 0.0f, 0.0f,
            0.0f,         2.0f / float(height), 0.0f, 0.0f,
            0.0f,         0.0f, 1.0f, 0.0f,
            -1.0f,       -1.0f, 0.0f, 1.0f
        };

        lineShader.uniformMatrix4fv(u_projection, worldMatrix);
    }

    /* ****** Main code below ****** */
    std::vector<Boundary> bounds;
    bounds.reserve(14);

    // Later on we will allow reading these from a file
    double widthDbl = double(width);
    double heightDbl = double(height);

    double aspect = widthDbl / heightDbl;
    double widthPad = (10.0 * widthDbl) / (400.0 * aspect);
    double heightPad = (10.0 * heightDbl) / (400.0 / aspect);

    double width14 = widthDbl / 4.0;
    double height14 = heightDbl / 4.0;

    double width34 = 3.0 * width14;
    double height34 = 3.0 * height14;

    // Exterior Bounds
    bounds.emplace_back(widthPad, heightPad, widthDbl - widthPad, heightPad);
    bounds.emplace_back(widthDbl - widthPad, heightPad, widthDbl - widthPad, heightDbl - heightPad);
    bounds.emplace_back(widthDbl - widthPad, heightDbl - heightPad, widthPad, heightDbl - heightPad);
    bounds.emplace_back(widthPad, heightDbl - heightPad, widthPad, heightPad);

    // ****** Interior Bounds ****** //
    // **** Left Block **** //
    bounds.emplace_back(width14 - widthPad, height14 - heightPad, width14 + widthPad, height14 - heightPad);
    bounds.emplace_back(width14 + widthPad, height14 - heightPad, width14 + widthPad, height34 + heightPad);
    bounds.emplace_back(width14 + widthPad, height34 + heightPad, width14 - widthPad, height34 + heightPad);
    bounds.emplace_back(width14 - widthPad, height34 + heightPad, width14 - widthPad, height14 - heightPad);

    // **** Right Block **** //
    bounds.emplace_back(width34 - widthPad, height14 - heightPad, width34 + widthPad, height14 - heightPad);
    bounds.emplace_back(width34 + widthPad, height14 - heightPad, width34 + widthPad, height34 + heightPad);
    bounds.emplace_back(width34 + widthPad, height34 + heightPad, width34 - widthPad, height34 + heightPad);
    bounds.emplace_back(width34 - widthPad, height34 + heightPad, width34 - widthPad, height14 - heightPad);

    float widthPad5 = 5.0 * widthPad;
    float heightPad5 = 5.0 * heightPad;

    bounds.emplace_back(width14 + widthPad5, height14 + heightPad5, width34 - widthPad5, height34 - heightPad5);
    bounds.emplace_back(width34 - widthPad5, height14 + heightPad5, width14 + widthPad5, height34 - heightPad5);
    
    // Particle to follow the mouse
    LineCaster entity(double(width) / 2.0, double(height) / 2.0);

    double oldMouseX = 0.0;
    double oldMouseY = 0.0;

    lineShader.bind();
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        mousePosY = heightDbl - mousePosY;
        
        if (!(mousePosX == oldMouseX && mousePosY == oldMouseY)) {
            if (!(mousePosX < widthPad || mousePosX > widthDbl - widthPad || mousePosY < heightPad || mousePosY > heightDbl - heightPad)) {
                entity.update({ mousePosX, mousePosY });
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