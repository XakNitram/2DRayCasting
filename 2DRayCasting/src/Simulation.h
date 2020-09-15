#pragma once
#include <memory>
#include <exception>
#include <iostream>
#include <type_traits>

#include "GL/glew.h"
#include "GLFW/glfw3.h"


class MainLoop {
public:
    /* Event handlers */
    virtual void handleKeys(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
    
    /* Loop stages */
    virtual void update(double dt) = 0;
    virtual void draw() = 0;
};


// Apparently templates need to be defined in the same place as the declaration.
template<class Loop>
class Simulation {
    static_assert(std::is_base_of<MainLoop, Loop>::value, "Template type must inherit from MainLoop.");

    GLFWwindow* window;
    
    void terminate() {
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

    static Loop* getLoop(GLFWwindow* window) {
        void* userPointer = glfwGetWindowUserPointer(window);

        if (!userPointer) {
            return nullptr;
        }

        return static_cast<Loop*>(userPointer);
    }

    static void handleKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Loop* loop = getLoop(window);
        loop->handleKeys(window, key, scancode, action, mods);
    }

public:
    Simulation(unsigned int width, unsigned int height, const char* title) {
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
    
    ~Simulation() {
        terminate();
    }

    template<int swapInterval>
    void run() {
        Loop loop;

        glfwSetWindowUserPointer(window, &loop);
        glfwSetKeyCallback(window, handleKeys);
        glfwSwapInterval(swapInterval);

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
            loop.update(dt);

            /* Call the loop draw method. */
            loop.draw();
            glfwSwapBuffers(window);

            /* Poll for and process events. */
            glfwPollEvents();
        }
    }
};