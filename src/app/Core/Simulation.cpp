#include "pch.hpp"
#include "Simulation.hpp"


void Simulation::terminate() {
    if (window != nullptr) {
#ifdef _DEBUG
        std::cout << "Destroying main window." << std::endl;
#endif // _DEBUG

        glfwSetWindowUserPointer(window, nullptr);
        glfwDestroyWindow(window);
        glfwPollEvents();
    }

#ifdef _DEBUG
    std::cout << "Terminating GLFW." << std::endl;
#endif
    glfwTerminate();
}

Simulation *Simulation::getState(GLFWwindow *window) {
    return static_cast<Simulation *>(glfwGetWindowUserPointer(window));
}

void Simulation::attachKeyCallback() {
    glfwSetKeyCallback(
        window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            Simulation *state = getState(window);
            if (state) {
                state->handleKeys(key, scancode, action, mods);
            }
        }
    );
}

void Simulation::attachCursorPositionCallback() {
    glfwSetCursorPosCallback(
        window, [](GLFWwindow *window, double xpos, double ypos) {
            Simulation *state = getState(window);
            if (state) {
                state->handleCursorPosition(xpos, ypos);
            }
        }
    );
}

void Simulation::attachCursorEnterCallback() {
    glfwSetCursorEnterCallback(
        window, [](GLFWwindow *window, int entered) {
            Simulation *state = getState(window);
            if (state) {
                state->handleCursorEnter(entered);
            }
        }
    );
}

void Simulation::attachMouseButtonCallback() {
    glfwSetMouseButtonCallback(
        window, [](GLFWwindow *window, int button, int action, int mods) {
            Simulation *state = getState(window);
            if (state) {
                state->handleMouseButton(button, action, mods);
            }
        }
    );
}

void Simulation::handleKeys(int key, int scancode, int action, int mods) {}

void Simulation::handleCursorPosition(double xpos, double ypos) {}

void Simulation::handleCursorEnter(int entered) {}

void Simulation::handleMouseButton(int button, int action, int mods) {}


void Simulation::swapInterval(int interval) {
    glfwSwapInterval(interval);
}

void Simulation::scaleToMonitor(bool value) {
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, value);
}

void Simulation::update(double dt) {}

void Simulation::draw() {}

void Simulation::initGLFW() {
#ifdef _DEBUG
    std::cout << "Initializing GLFW." << std::endl;
#endif // _DEBUG
    if (!glfwInit()) {
        throw std::exception("Failed to initialize GLFW.");
    }
}

Simulation::Simulation(unsigned int width, unsigned int height, const char *title, GLFWmonitor *monitor) {
    // Destructor is not called if exception is thrown 

    // Set GLFW window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // _DEBUG

    /* Initialize GLFW. */
    initGLFW();

    /* Create a GLFW window and its OpenGL context. */
    window = glfwCreateWindow(width, height, title, monitor, nullptr);
    glfwMakeContextCurrent(window);
    if (!window) {
        terminate();
        throw std::exception("Failed to create GLFW window.");
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        terminate();
        throw std::exception("Failed to initialize Glad.");
    }

    glfwSetWindowUserPointer(window, this);

    /* Output the current OpenGL version. */
#ifdef _DEBUG
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
#endif // _DEBUG
}

Simulation::~Simulation() {
    terminate();
}

void Simulation::run() {
    // This basically zeroes out the first delta calculation.
    // The higher the framerate, the more accurate 
    // this first delta will be.
    double lastFrame = glfwGetTime();

    /* Run the simulation loop. */
    while (!glfwWindowShouldClose(window)) {
        /* Calculate how much time the last frame took. */
        double currentFrame = glfwGetTime();
        double dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Call the loop update method. */
        update(dt);

        /* Call the loop draw method. */
        draw();
        glfwSwapBuffers(window);

        /* Poll for and process events. */
        glfwPollEvents();
    }
}
