#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Simulation.h"


class RayCasting final : public Simulation {
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

public:
    RayCasting(unsigned int width, unsigned int height) : 
            Simulation(width, height, "2D Ray Casting") {
        attachKeyCallback();
    }
    
    ~RayCasting() final = default;
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
