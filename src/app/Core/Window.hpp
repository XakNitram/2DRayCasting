#pragma once

#include "pch.hpp"
#include "Event.hpp"


extern constexpr size_t eventStackCapacity = 3;


template<typename T>
struct Double {
    [[nodiscard]] T width() const { return a; }

    [[nodiscard]] T height() const { return b; }

    [[nodiscard]] T x() const { return a; }

    [[nodiscard]] T y() const { return b; }

    T a, b;
};

class Window;

struct Config {
    Double<uint32_t> window;

    uint8_t samples = 8;
    bool resizable = false;  // This can be converted to a flag bit type thing when additional features are required.
    friend Window;

    Config(uint32_t width, uint32_t height);
};


class Window {
    const char *m_title;
    GLFWwindow *m_window;
    const Config m_config;
    std::vector<Event> m_events;

    static Window *getState(GLFWwindow *window);

    static void terminate(GLFWwindow *window);

    static GLFWwindow *create(const Config &config, const char *title);

public:
    Window(const Config &config, const char *title);

    const Config &config();

    const char *title();

    Double<int32_t> frame();

    Double<double> cursorPosition();

    bool shouldClose();

    void shouldClose(bool value);

    void swapBuffers();

    static void update();

    static void clear();

    void pushEvent(Event event);

    std::optional<Event> pollEvent();
};
