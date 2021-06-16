#include "pch.hpp"
#include "Core/Event.hpp"
#include "Core/Window.hpp"
#include "Math/Geometrics.hpp"
#include "Primitives/Floor.hpp"
#include "Primitives/FloorTexture.hpp"
#include "Primitives/NodeRenderer.hpp"
#include "Casters/AngleCaster.hpp"
#include "Casters/EndPointCaster.hpp"
#include "Debug.hpp"
#include "Shader.hpp"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156

using namespace lwvl::debug;

constexpr uint32_t CIRCLE_SLICES = 32;
constexpr float M_TAU = 6.283185307179586f;


struct CasterConfig {
    float prevX, prevY;
    std::unique_ptr<Caster> caster;

    explicit CasterConfig(std::unique_ptr<Caster> &&caster) : prevX(0.0), prevY(0.0), caster(std::move(caster)) {}

    CasterConfig() : prevX(0.0f), prevY(0.0f), caster(nullptr) {}

    void setCaster(std::unique_ptr<Caster> &&newCaster) { this->caster = std::move(newCaster); }
};


static inline double delta(std::chrono::time_point<std::chrono::steady_clock> start) {
    return 0.000001 * static_cast<double>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start
        ).count()
    );
}


typedef enum {
    LineAngle = 0,
    FilledAngle = 1,
    LineEndpoint = 2,
    FilledEndpoint = 3
} RenderMode;


class Application {
    Window window;

public:
    Application(uint32_t width, uint32_t height) : window({width, height}, "RayCasting") {}

    int run() {
#ifndef NDEBUG
        GLEventListener listener(
            [](
                Source source, Type type,
                Severity severity, unsigned int id, int length,
                const char *message, const void *userState
            ) {
                std::cout << "[OpenGL] " << message << std::endl;
            }
        );

        const auto setupStart = std::chrono::high_resolution_clock::now();
#endif
        float frameWidth, frameHeight;
        {
            const auto[width, height] = window.frame();
            frameWidth = static_cast<float>(width);
            frameHeight = static_cast<float>(height);
        }

        const float wPad = (10.0f * frameWidth) / 800.0f;
        const float hPad = (10.0f * frameHeight) / 600.0f;

        float floorWidth = frameWidth - (2.0f * wPad);
        float floorHeight = frameHeight - (2.0f * hPad);

        Floor floor(wPad, hPad, floorWidth, floorHeight);
        FloorTexture floorBuffer;
        floorBuffer.render(static_cast<uint32_t>(floorWidth), static_cast<uint32_t>(floorHeight));

        // ****** Construct Shaders ******
        //std::array<float, 16> projection {
        //    2.0f / frameWidth, 0.0f, 0.0f, 0.0f,
        //    0.0f, 2.0f / frameHeight, 0.0f, 0.0f,
        //    0.0f, 0.0f, 1.0f, 0.0f,
        //    -1.0f, -1.0f, 0.0f, 1.0f
        //};

        // **** Line Render Control ****
        lwvl::ShaderProgram lineControl;
        lineControl.link(
            lwvl::VertexShader::readFile("Data/Shaders/default.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/default.frag")
        );
        lineControl.bind();
        lineControl.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
        lineControl.uniform("u_Color").set3f(1.0f, 1.0f, 1.0f);

        // **** Ray-Caster Render Control ****
        lwvl::ShaderProgram lightControl;
        lightControl.link(
            lwvl::VertexShader::readFile("Data/Shaders/light.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/light.frag")
        );
        lightControl.bind();
        lightControl.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
        lightControl.uniform("u_Resolution").set2f(floorWidth, floorHeight);
        lightControl.uniform("u_Offset").set2f(wPad, hPad);
        lightControl.uniform("u_Texture").set1i(int32_t(floorBuffer.slot()));
        lwvl::Uniform lightCenter = lightControl.uniform("u_MouseCoords");

        lightControl.uniform("u_LightColor").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
        //lightControl.uniform("u_LightColor").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
        // lightControl.uniform("u_LightColor").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
        // lightControl.uniform("u_LightColor").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
        // lightControl.uniform("u_LightColor").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayola

        // **** Background Render Control ****
        lwvl::ShaderProgram floorControl;
        floorControl.link(
            lwvl::VertexShader::readFile("Data/Shaders/floor.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/floor.frag")
        );
        floorControl.bind();
        floorControl.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
        floorControl.uniform("u_Texture").set1i(int32_t(floorBuffer.slot()));

        NodeRenderer<12 + CIRCLE_SLICES> bounds;
        {
            // Bounding Wall
            Point frameWallA{wPad, hPad};
            Point frameWallB{frameWidth - wPad, hPad};
            Point frameWallC{frameWidth - wPad, frameHeight - hPad};
            Point frameWallD{wPad, frameHeight - hPad};

            bounds.add({frameWallA, frameWallB});
            bounds.add({frameWallB, frameWallC});
            bounds.add({frameWallC, frameWallD});
            bounds.add({frameWallD, frameWallA});

            const float width14 = frameWidth / 4.0f;
            const float height14 = frameHeight / 4.0f;
            const float width34 = 3.0f * frameWidth / 4.0f;
            const float height34 = 3.0f * frameHeight / 4.0f;

            // West Wall
            Point westWallA{width14 - wPad, height14 - hPad};
            Point westWallB{width14 + wPad, height14 - hPad};
            Point westWallC{width14 + wPad, height34 + hPad};
            Point westWallD{width14 - wPad, height34 + hPad};

            bounds.add({westWallA, westWallB});
            bounds.add({westWallB, westWallC});
            bounds.add({westWallC, westWallD});
            bounds.add({westWallD, westWallA});

            // East Wall
            Point eastWallA{width34 - wPad, height14 - hPad};
            Point eastWallB{width34 + wPad, height14 - hPad};
            Point eastWallC{width34 + wPad, height34 + hPad};
            Point eastWallD{width34 - wPad, height34 + hPad};

            bounds.add({eastWallA, eastWallB});
            bounds.add({eastWallB, eastWallC});
            bounds.add({eastWallC, eastWallD});
            bounds.add({eastWallD, eastWallA});

            // Center Circle
            Point center = {frameWidth / 2.0f, frameHeight / 2.0f};

            uint32_t circleSlices = CIRCLE_SLICES;
            float slice = M_TAU / static_cast<float>(circleSlices);
            //			float radius = frameHeight / 4.0f;
            float radius = (height34 - height14) / 2.0f;

            Point prevPoint{radius * std::cosf(0.0f) + center.x, radius * std::sinf(0.0f) + center.y};
            for (uint32_t i = 1; i < circleSlices + 1; i++) {
                float angle = static_cast<float>(i) * slice;
                Point newPoint{radius * std::cosf(angle) + center.x, radius * std::sinf(angle) + center.y};
                bounds.add({newPoint, prevPoint});
                prevPoint = newPoint;
            }
        }
        bounds.update();

        const unsigned int numBounds = bounds.size();
        CasterConfig casters[4]{};
        casters[FilledEndpoint].setCaster(std::make_unique<FilledEndPointCaster>(numBounds));
        casters[LineEndpoint].setCaster(std::make_unique<LineEndPointCaster>(numBounds));
        casters[FilledAngle].setCaster(std::make_unique<FilledAngleCaster>());
        casters[LineAngle].setCaster(std::make_unique<LineAngleCaster>());

#ifndef NDEBUG
        std::cout << "Setup took " << delta(setupStart) << " seconds." << std::endl;
#endif

        RenderMode renderMode = FilledEndpoint;
        bool followMouse = true;
        bool showBounds = true;

        const auto changeRenderMode = [&](RenderMode newMode) {
            renderMode = newMode;
            lightControl.bind();
            lightCenter.set2f(casters[newMode].prevX, frameHeight - casters[newMode].prevY);
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        while (!window.shouldClose()) {
            // Fill event stack
            Window::update();

            // Handle incoming events
            while (std::optional<Event> possible = window.pollEvent()) {
                if (!possible.has_value()) {
                    continue;
                }

                Event &concrete = possible.value();
                if (concrete.type == Event::Type::KeyRelease) {
                    KeyboardEvent &keyboardEvent = std::get<KeyboardEvent>(concrete.event);
                    switch (keyboardEvent.key) {
                        case GLFW_KEY_ESCAPE:window.shouldClose(true);
                            break;
                        case GLFW_KEY_1:changeRenderMode(FilledEndpoint);
                            break;
                        case GLFW_KEY_2:changeRenderMode(LineEndpoint);
                            break;
                        case GLFW_KEY_3:changeRenderMode(FilledAngle);
                            break;
                        case GLFW_KEY_4:changeRenderMode(LineAngle);
                            break;
                        case GLFW_KEY_B:showBounds ^= true;
                            break;
                        case GLFW_KEY_SPACE:followMouse ^= true;
                            break;
                        default:break;
                    }
                }
            }

            // Update engine
            CasterConfig &config = casters[renderMode];
            auto &caster = config.caster;

            if (followMouse) {
                const auto[mouseXd, mouseYd] = window.cursorPosition();
                const auto mouseX = static_cast<float>(mouseXd);
                const auto mouseY = static_cast<float>(mouseYd);

                if (
                    !(mouseX == config.prevX && mouseY == config.prevY)
                    && !(mouseX < wPad || mouseX > frameWidth - wPad || mouseY < hPad || mouseY > frameHeight - hPad)
                    ) {
                    lightControl.bind();
                    lightCenter.set2f(mouseX, frameHeight - mouseY);
                    caster->update(mouseX, frameHeight - mouseY);
                    caster->look(bounds.segments());
                }

                config.prevX = mouseX;
                config.prevY = mouseY;
            }

            // Rendering
            lwvl::clear();

            floorBuffer.bind();
            floorControl.bind();
            floor.draw();

            lightControl.bind();
            caster->draw();

            if (showBounds) {
                lineControl.bind();
                bounds.draw();
            }

            window.swapBuffers();
        }

        return 0;
    }
};


int main() {
    try {
        // Borderless window
        /*RayCasting::initGLFW();
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        RayCasting sim(mode->width, mode->height, monitor);*/
        //RayCasting sim(800, 600);
        //sim.run();

        Application app(800, 600);
        return app.run();
    }

    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
