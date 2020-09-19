#include "Simulation.h"


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
#endif // _DEBUG
    glfwTerminate();
}

Simulation* Simulation::getState(GLFWwindow* window) {
    return static_cast<Simulation*>(glfwGetWindowUserPointer(window));
}


void Simulation::attachKeyCallback() {
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Simulation* state = getState(window);
        if (state) {
            state->handleKeys(key, scancode, action, mods);
        }
    });
}

void Simulation::attachCursorPositionCallback() {
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        Simulation* state = getState(window);
        if (state) {
            state->handleCursorPosition(xpos, ypos);
        }
    });
}

void Simulation::attachCursorEnterCallback() {
    glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
        Simulation* state = getState(window);
        if (state) {
            state->handleCursorEnter(entered);
        }
    });
}

void Simulation::attachMouseButtonCallback() {
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        Simulation* state = getState(window);
        if (state) {
            state->handleMouseButton(button, action, mods);
        }
    });
}


void Simulation::handleKeys(int key, int scancode, int action, int mods) {}

void Simulation::handleCursorPosition(double xpos, double ypos) {}

void Simulation::handleCursorEnter(int entered) {}

void Simulation::handleMouseButton(int button, int action, int mods) {}


void Simulation::setSwapInterval(int interval) {
    glfwSwapInterval(interval);
}


void Simulation::update(double dt) {}

void Simulation::draw() {}


Simulation::Simulation(unsigned int width, unsigned int height, const char* title) {
    // Destructor is not called if exception is thrown 

    /* Initialize GLFW. */
#ifdef _DEBUG
    std::cout << "Initializing GLFW." << std::endl;
#endif // _DEBUG
    if (!glfwInit()) {
        throw std::exception("Failed to initialize GLFW.");
    }

    /* Set window creation flags. */
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

    /* Create a GLFW window and its OpenGL context. */
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window) {
        terminate();
        throw std::exception("Failed to create GLFW window.");
    }

    glfwSetWindowUserPointer(window, this);

    /* Make the window's context current. */
    glfwMakeContextCurrent(window);

    /* Enable VSync */
    glfwSwapInterval(0);

    /* Initialize GLEW. */
    if (glewInit() != GLEW_OK) {
        terminate();
        throw std::exception("Failed to initialize GLEW.");
    }

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
