#include <iostream>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Simulation.h"
#include "Shader.h"
#include "Utils.h"


class RayCasting final : public Simulation {
    unsigned int vao;
    unsigned int vbo;
    lwvl::Shader* defaultShader;

public:
    RayCasting(unsigned int width, unsigned int height) :
            Simulation(width, height, "2D Ray Casting"),
            defaultShader(nullptr) {
        attachKeyCallback();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        float positions[12] = {
            -0.5, -0.5,
            0.5, -0.5,
            0.5, 0.5,

            0.5, 0.5,
            -0.5, 0.5,
            -0.5, -0.5
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2 * sizeof(float), GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        std::string vertexSource = lwvl::readFile("Shaders/default.vert");
        std::string fragmentSource = lwvl::readFile("Shaders/default.frag");
        defaultShader = new lwvl::Shader(vertexSource, fragmentSource);
    }

    ~RayCasting() final = default;
protected:
    void handleKeys(int key, int scancode, int action, int mods) final {
        if (action == GLFW_RELEASE) {
            switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                return;
            default:
                return;
            }
        }
    }

    void update(double dt) final {

    }

    void draw() final {
        defaultShader->bind();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};


int main() {
    try {
        RayCasting sim(800, 600);
        sim.run();
    }

    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
