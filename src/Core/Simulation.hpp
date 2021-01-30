#pragma once
#include "pch.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Simulation {
protected:
    GLFWwindow* window;

private:
    void terminate();
    static Simulation* getState(GLFWwindow* window);

protected:
    void attachKeyCallback();
    void attachCursorPositionCallback();
    void attachCursorEnterCallback();
    void attachMouseButtonCallback();

    virtual void handleKeys(int key, int scancode, int action, int mods);
    virtual void handleCursorPosition(double xpos, double ypos);
    virtual void handleCursorEnter(int entered);
    virtual void handleMouseButton(int button, int action, int mods);

    // Setting GLFW Variables
    void swapInterval(int interval);

    // GLFW Window Hints
    static void scaleToMonitor(bool value);

    virtual void update(double dt);
    virtual void draw();

public:
    static void initGLFW();

    Simulation(unsigned int width, unsigned int height, const char* title, GLFWmonitor* = nullptr);
    virtual ~Simulation();
    void run();
};
