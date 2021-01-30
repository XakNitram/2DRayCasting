#include "pch.hpp"
#include "Core/Utils.hpp"
#include "Core/Simulation.hpp"
#include "Math/Geometrics.hpp"
#include "Renderer/GLDebug.hpp"
#include "Renderer/Shader.hpp"
#include "Primitives/Boundary.hpp"
#include "Primitives/Floor.hpp"
#include "Primitives/FloorTexture.hpp"
#include "Casters/AngleCaster.hpp"
#include "Casters/EndPointCaster.hpp"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156
// MSVCP140D
// VCRUNTIME140D
// ucrtbased.dll

constexpr unsigned int FOLLOW_MOUSE = 0b1;
constexpr unsigned int SHOW_BOUNDS  = 0b10;
constexpr uint32_t CIRCLE_SLICES = 64;
constexpr float M_TAU = 6.283185307179586f;


// We can remove this template variable in the future
//  if it makes the class easier to use.
template<size_t capacity>
class NodeRenderer {
    // Attributes
    lwvl::VertexArray vao;
    lwvl::ArrayBuffer vbo;

    std::vector<LineSegment> m_segments;

    // Methods
    std::array<float, capacity * 4> collectData() {
        std::array<float, capacity * 4> temp {};

        size_t currentElements = size();
        for (size_t i = 0; i < currentElements; i++) {
            LineSegment& segment = m_segments[i];
            temp[i * 4 + 0] = segment.a.x;
            temp[i * 4 + 1] = segment.a.y;
            temp[i * 4 + 2] = segment.b.x;
            temp[i * 4 + 3] = segment.b.y;
        }

        return temp;
    }

public:
    NodeRenderer() {
        m_segments.reserve(capacity);

        vao.bind();
        vbo.bind();
        vbo.usage(lwvl::Usage::Dynamic);

        std::array<float, capacity * 4> data = collectData();
        vbo.construct(data.begin(), data.end());
        vao.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

        lwvl::VertexArray::clear();
        lwvl::ArrayBuffer::clear();
    }

    const std::vector<LineSegment>& segments() { return m_segments; }

    size_t size() { return m_segments.size(); }
    size_t max() { return capacity; }

    void add(LineSegment&& segment) {
        if (size() != capacity) {
            m_segments.push_back(segment);
        } else {
            throw std::exception("NodeRenderer capacity exceeded.");
        }
    }

    // Maybe a remove method but I won't use it here.

    void update() {
        vbo.bind();
        std::array<float, capacity * 4> data = collectData();
        vbo.update(data.begin(), data.end());
    }

    void draw() {
        vao.bind();
        vao.drawArrays(GL_LINES, capacity * 2);
    }
};


struct CasterConfig {
	float prevX, prevY;
	std::unique_ptr<Caster> caster;

	explicit CasterConfig(std::unique_ptr<Caster>&& caster) : prevX(0.0), prevY(0.0), caster(std::move(caster)) {}
	CasterConfig() : prevX(0.0f), prevY(0.0f), caster(nullptr) {}

	void setCaster(std::unique_ptr<Caster>&& newCaster) { this->caster = std::move(newCaster); }
};


enum class RenderMode: size_t {
	LINE_ANGLE,
	FILLED_ANGLE,
	LINE_END_POINT,
	FILLED_END_POINT
};


struct SimulationConfig {
	RenderMode mode;
	unsigned int config;
	float frameWidth, frameHeight;

	SimulationConfig() : 
		mode(RenderMode::FILLED_END_POINT), config(FOLLOW_MOUSE | SHOW_BOUNDS),
		frameWidth(0.0f), frameHeight(0.0f)
	{}
};


class RayCasting final : public Simulation {
	static void __stdcall debug_gl(
        lwvl::debug::Source source, lwvl::debug::Type type, lwvl::debug::Severity severity,
        unsigned int id, int length, const char* message, const void* userState
	) {
		if (type == lwvl::debug::Type::ERROR) {
			throw std::exception(message);
		}
#ifndef NDEBUG
		else {
			std::cout << "[OpenGL] " << message << std::endl;
		}
#endif
	}

protected:
	void handleKeys(int key, int scancode, int action, int mods) final {
		if (action == GLFW_RELEASE) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				return;
			case GLFW_KEY_1:
				settings.mode = RenderMode::FILLED_END_POINT;
				return;
			case GLFW_KEY_2:
				settings.mode = RenderMode::LINE_END_POINT;
				return;
			case GLFW_KEY_3:
				settings.mode = RenderMode::FILLED_ANGLE;
				return;
			case GLFW_KEY_4:
				settings.mode = RenderMode::LINE_ANGLE;
				return;
			case GLFW_KEY_B:
				settings.config ^= SHOW_BOUNDS;
				return;
			case GLFW_KEY_SPACE:
				settings.config ^= FOLLOW_MOUSE;
				return;
			default:
				return;
			}
		}
	}

private:
	lwvl::debug::GLEventListener m_listener;
	SimulationConfig settings;
	NodeRenderer<12 + CIRCLE_SLICES> bounds;
	lwvl::ShaderProgram lineShader, lightShader, floorShader;
	lwvl::Uniform lightCenter;
	CasterConfig casters[4];
	FloorTexture floorTexture;
	Floor floor;

	inline std::unique_ptr<Caster>& currentCaster() {
		return casters[static_cast<size_t>(settings.mode)].caster;
	}

public:
	RayCasting(unsigned int width, unsigned int height, GLFWmonitor* monitor = nullptr) :
		Simulation(width, height, "RayCasting", monitor),
		m_listener(this, debug_gl, true)
	{
		attachKeyCallback();
		swapInterval(1);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// ****** Set Up OpenGL Stuff ******
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// ****** Get Actual Frame Size ******
		int iFrameWidth, iFrameHeight;
		glfwGetFramebufferSize(window, &iFrameWidth, &iFrameHeight);

		const auto frameWidth = static_cast<float>(iFrameWidth);
		const auto frameHeight = static_cast<float>(iFrameHeight);
		settings.frameWidth  = frameWidth;
		settings.frameHeight = frameHeight;

		const float wPad = (10.0f * frameWidth) / 800.0f;
		const float hPad = (10.0f * frameHeight) / 600.0f;


		float floorWidth = frameWidth - (2.0f * wPad);
		float floorHeight = frameHeight - (2.0f * hPad);

		floorTexture.render(
            static_cast<uint32_t>(floorWidth),
            static_cast<uint32_t>(floorHeight)
        );
		floor.update(wPad, hPad, floorWidth, floorHeight);

		// ****** Construct Shaders ******
		lineShader.link(readFile("Shaders/default.vert"), readFile("Shaders/default.frag"));
		lightShader.link(readFile("Shaders/light.vert"), readFile("Shaders/light.frag"));
		floorShader.link(readFile("Shaders/floor.vert"), readFile("Shaders/floor.frag"));

		// ****** Initialize Shader Uniforms ******
		lineShader.bind();
		lineShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		lineShader.uniform("u_Color").set3f(1.0f, 1.0f, 1.0f);

		lightShader.bind();
		lightShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		lightShader.uniform("u_Resolution").set2f(floorWidth, floorHeight);
		lightShader.uniform("u_Offset").set2f(wPad, hPad);
		lightShader.uniform("u_Texture").set1i(floorTexture.slot());
		lightCenter = lightShader.uniform("u_MouseCoords");

		//        lightShader.uniform("u_LightColor").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
//        lightShader.uniform("u_LightColor").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
        lightShader.uniform("u_LightColor").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
//        lightShader.uniform("u_LightColor").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayola

		floorShader.bind();
		floorShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		floorShader.uniform("u_Texture").set1i(floorTexture.slot());

		// ****** Construct Boundaries ******
        {
            // Bounding Wall
            Point frameWallA {wPad, hPad};
            Point frameWallB {frameWidth - wPad, hPad};
            Point frameWallC {frameWidth - wPad, frameHeight - hPad};
            Point frameWallD {wPad, frameHeight - hPad};

            bounds.add({ frameWallA, frameWallB });
            bounds.add({ frameWallB, frameWallC });
            bounds.add({ frameWallC, frameWallD });
            bounds.add({ frameWallD, frameWallA });

            const float width14 = frameWidth / 4.0f;
			const float height14 = frameHeight / 4.0f;
			const float width34 = 3.0f * frameWidth / 4.0f;
			const float height34 = 3.0f * frameHeight / 4.0f;

			// West Wall
			Point westWallA {width14 - wPad, height14 - hPad};
			Point westWallB {width14 + wPad, height14 - hPad};
			Point westWallC {width14 + wPad, height34 + hPad};
			Point westWallD {width14 - wPad, height34 + hPad};

			bounds.add({ westWallA, westWallB });
			bounds.add({ westWallB, westWallC });
			bounds.add({ westWallC, westWallD });
			bounds.add({ westWallD, westWallA });

			// East Wall
			Point eastWallA {width34 - wPad, height14 - hPad};
			Point eastWallB {width34 + wPad, height14 - hPad};
			Point eastWallC {width34 + wPad, height34 + hPad};
			Point eastWallD {width34 - wPad, height34 + hPad};

			bounds.add({ eastWallA, eastWallB });
			bounds.add({ eastWallB, eastWallC });
			bounds.add({ eastWallC, eastWallD });
			bounds.add({ eastWallD, eastWallA });

			// Center Circle
			Point center = {frameWidth / 2.0f, frameHeight / 2.0f};

			uint32_t circleSlices = CIRCLE_SLICES;
			float slice = M_TAU / static_cast<float>(circleSlices);
//			float radius = frameHeight / 4.0f;
            float radius = (height34 - height14) / 2.0f;

			Point prevPoint {radius * std::cosf(0.0f) + center.x, radius * std::sinf(0.0f) + center.y};
            for (uint32_t i = 1; i < circleSlices + 1; i++) {
                float angle = static_cast<float>(i) * slice;
                Point newPoint {radius * std::cosf(angle) + center.x, radius * std::sinf(angle) + center.y};
                bounds.add({ newPoint, prevPoint });
                prevPoint = newPoint;
            }
        }

		bounds.update();

		// ****** Initialize Casters ******
		const unsigned int numBounds = bounds.size();
		casters[static_cast<size_t>(RenderMode::FILLED_END_POINT)].setCaster(std::make_unique<FilledEndPointCaster>(numBounds));
		casters[static_cast<size_t>(RenderMode::LINE_END_POINT)  ].setCaster(std::make_unique<LineEndPointCaster>(numBounds));
		casters[static_cast<size_t>(RenderMode::FILLED_ANGLE)    ].setCaster(std::make_unique<FilledAngleCaster>());
		casters[static_cast<size_t>(RenderMode::LINE_ANGLE)      ].setCaster(std::make_unique<LineAngleCaster>());
	}

	void update(double dt) final {
		const float frameWidth = settings.frameWidth;
		const float frameHeight = settings.frameHeight;
		const float wPad = (10.0f * frameWidth) / 400.0f;
		const float hPad = (10.0f * frameHeight) / 400.0f;

		const auto renderMode = static_cast<size_t>(settings.mode);
		CasterConfig& mode = casters[renderMode];
		auto& entity = mode.caster;

		if (settings.config & FOLLOW_MOUSE) {
			double mousePosX, mousePosY;
			glfwGetCursorPos(window, &mousePosX, &mousePosY);
			const auto mousePosXf = static_cast<float>(mousePosX);
			const auto mousePosYf = static_cast<float>(mousePosY);

			if (!(mousePosXf == mode.prevX && mousePosYf == mode.prevY)) {
				if (!(mousePosXf < wPad || mousePosXf > frameWidth - wPad || mousePosYf < hPad || mousePosYf > frameHeight - hPad)) {
					lightShader.bind();
					lightCenter.set2f(mousePosXf, frameHeight - mousePosYf);
					entity->update(mousePosXf, frameHeight - mousePosYf);
					entity->look(bounds.segments());
				}
			}

			mode.prevX = mousePosXf;
			mode.prevY = mousePosYf;
		}
	}

	void draw() final {
		auto& entity = currentCaster();

		/****** Update shaders and render. ******/
		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw caster.
		floorTexture.bind();
		floorShader.bind();
		floor.draw();

		lightShader.bind();
		entity->draw();

		if (settings.config & SHOW_BOUNDS) {
			lineShader.bind();
			bounds.draw();
		}
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
		RayCasting sim(800, 600);
		sim.run();

		return 0;
	}

	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
}
