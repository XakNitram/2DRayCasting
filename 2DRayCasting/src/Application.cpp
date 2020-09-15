#include <iostream>

//#include "GL/glew.h"
//#include "GLFW/glfw3.h"
#include "Simulation.h"


class RayCasting : public MainLoop {
    double averageDelta = 0.0;
public:
    RayCasting() = default;
    ~RayCasting() {
        std::cout << "Average time: " << averageDelta << std::endl;
    }

    void handleKeys(GLFWwindow* window, int key, int scancode, int action, int mods) override {
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
    
    void update(double dt) override {
        averageDelta = (averageDelta + dt) / 2.0;
    }

    void draw() override {

    }
};


int main() {
    try {
        Simulation<RayCasting> sim(800, 600, "2D Ray Casting");
        sim.run<0>();
    }

    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    //GLFWwindow* window;

    ///* Initialize the library */
    //if (!glfwInit())
    //    return -1;

    ///* Create a windowed mode window and its OpenGL context */
    //window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    //if (!window)
    //{
    //    glfwTerminate();
    //    return -1;
    //}

    ///* Make the window's context current */
    //glfwMakeContextCurrent(window);

    //double lastFrame = glfwGetTime();
    //double averageDelta = 0.0;

    ///* Loop until the user closes the window */
    //while (!glfwWindowShouldClose(window)) {
    //    double currentFrame = glfwGetTime();
    //    double dt = currentFrame - lastFrame;
    //    lastFrame = currentFrame;

    //    averageDelta = (averageDelta + dt) / 2.0;

    //    /* Render here */
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    /* Swap front and back buffers */
    //    glfwSwapBuffers(window);

    //    /* Poll for and process events */
    //    glfwPollEvents();
    //}

    //std::cout << "Average time: " << averageDelta << std::endl;

    //glfwTerminate();
    //return 0;
}
