#include "pch.hpp"
#include "Window.hpp"


// ****** Config ******
Config::Config(uint32_t width, uint32_t height) : window{width, height} {}


// ****** Window ******
Window *Window::getState(GLFWwindow *window) {
    return static_cast<Window *>(glfwGetWindowUserPointer(window));
}


void Window::terminate(GLFWwindow *window) {
    if (window != nullptr) {
        glfwSetWindowUserPointer(window, nullptr);
        glfwDestroyWindow(window);
        glfwPollEvents();
    }

    glfwTerminate();
}


GLFWwindow *Window::create(const Config &config, const char *title) {
    /* Initialize GLFW. */
    if (!glfwInit()) {
        throw std::exception("Failed to initialize GLFW.");
    }

    // Set GLFW window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // !NDEBUG

    glfwWindowHint(GLFW_SAMPLES, config.samples);

    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    //glfwWindowHint(GLFW_SRGB_CAPABLE, true);

    /* Create a GLFW window and its OpenGL context. */
    GLFWwindow *window = glfwCreateWindow(
        static_cast<int32_t>(config.window.width()),
        static_cast<int32_t>(config.window.height()),
        title, nullptr, nullptr
    );

    glfwMakeContextCurrent(window);
    if (!window) {
        terminate(window);
        throw std::exception("Failed to create GLFW window.");
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        terminate(window);
        throw std::exception("Failed to initialize Glad.");
    }
    return window;
}


Window::Window(const Config &config, const char *title) :
    m_title(title), m_window(create(config, title)), m_config(config) {
    m_events.reserve(eventStackCapacity);

    glfwSetWindowUserPointer(m_window, this);
    glfwSwapInterval(1);

    glfwSetKeyCallback(
        m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            Window *state = Window::getState(window);
            switch (action) {
                case GLFW_PRESS:state->m_events.emplace_back(Event::Type::KeyPress, KeyboardEvent{key, scancode, mods});
                    return;
                case GLFW_RELEASE:
                    state->m_events
                         .emplace_back(Event::Type::KeyRelease, KeyboardEvent{key, scancode, mods});
                    return;
                case GLFW_REPEAT:
                    state->m_events
                         .emplace_back(Event::Type::KeyRepeat, KeyboardEvent{key, scancode, mods});
                    return;
                default:return;
            }
        }
    );

    glfwSetCursorPosCallback(
        m_window, [](GLFWwindow *window, double xpos, double ypos) {
            Window *state = Window::getState(window);
            state->m_events.emplace_back(Event::Type::MouseMotion, MouseMotionEvent{xpos, ypos});
        }
    );

    glfwSetMouseButtonCallback(
        m_window, [](GLFWwindow *window, int button, int action, int mods) {
            Window *state = Window::getState(window);
            switch (action) {
                case GLFW_PRESS:state->m_events.emplace_back(Event::Type::MouseDown, MouseButtonEvent{button, mods});
                    return;
                case GLFW_RELEASE:state->m_events.emplace_back(Event::Type::MouseUp, MouseButtonEvent{button, mods});
                default:return;
            }
        }
    );

    glfwSetCharCallback(
        m_window, [](GLFWwindow *window, unsigned int codepoint) {
            Window *state = Window::getState(window);
            state->m_events.emplace_back(Event::Type::TextInput, TextEvent{codepoint});
        }
    );

    /* Output the current OpenGL version. */
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(m_window);
}


void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}


void Window::update() {
    glfwPollEvents();
}


void Window::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}


std::optional<Event> Window::pollEvent() {
    if (m_events.empty()) {
        return std::nullopt;
    } else {
        Event event = m_events.back();
        m_events.pop_back();
        return event;
    }
}

void Window::pushEvent(Event event) {
    m_events.push_back(event);
}

void Window::shouldClose(bool value) {
    glfwSetWindowShouldClose(m_window, value);
}

const Config &Window::config() {
    return m_config;
}

const char *Window::title() {
    return m_title;
}

Double<int32_t> Window::frame() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {width, height};
}

Double<double> Window::cursorPosition() {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return {x, y};
}
