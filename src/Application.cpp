#include "pch.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Utils.hpp"
#include "Core/Simulation.hpp"
#include "Renderer/GLDebug.hpp"
#include "Renderer/Shader.hpp"
#include "Math/Geometrics.hpp"
#include "Primitives/Boundary.hpp"
#include "Casters/AngleCaster.hpp"
#include "Casters/EndPointCaster.hpp"
#include "Primitives/Quad.hpp"
#include "Primitives/Floor.hpp"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156

constexpr unsigned int FOLLOW_MOUSE = 0b1;
constexpr unsigned int SHOW_BOUNDS  = 0b10;


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


class FloorTexture {
    uint32_t m_id = 0;
    uint32_t m_slot = 0;

public:
    FloorTexture() {
        glGenTextures(1, &m_id);
    }

    ~FloorTexture() {
        glDeleteTextures(1, &m_id);
    }

    void render(uint32_t width, uint32_t height) {
        GLuint frame;
        glGenFramebuffers(1, &frame);
        glBindFramebuffer(GL_FRAMEBUFFER, frame);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGB, width, height,
                0, GL_RGB, GL_UNSIGNED_BYTE, nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0);

        Quad floor(-1.0f, -1.0f, 2.0f, 2.0f);
        lwvl::ShaderProgram texturePipeline;
        lwvl::VertexShader fv("#version 330 core\nlayout(location=0) in vec4 position;\nvoid main() { gl_Position = position; }");
        lwvl::FragmentShader ff(readFile("Shaders/default.frag"));
//        lwvl::FragmentShader ff(readFile("Shaders/mazing.frag"));
        texturePipeline.link(fv, ff);
        texturePipeline.bind();

        // Floor color:
        texturePipeline.uniform("u_Color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
//        texturePipeline.uniform("u_Color").set3f(0.61569f, 0.63529f, 0.67059f);  // Silver

        // resolution for mazing.frag
//        texturePipeline.uniform("u_Resolution").set2f(static_cast<float>(width), static_cast<float>(height));

        GLsizei prevViewport[4];
        glGetIntegerv(GL_VIEWPORT, prevViewport);
        const GLsizei prevWidth = prevViewport[2];
        const GLsizei prevHeight = prevViewport[3];

        glViewport(0, 0, width + 1, height + 1);
        floor.draw();

        glViewport(0, 0, prevWidth, prevHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &frame);
    }

    [[nodiscard]] uint32_t slot() const { return m_slot; }
    void slot(uint32_t slot) { m_slot = slot; }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, m_id);
        glActiveTexture(GL_TEXTURE0 + m_slot);
    }
};


class RayCasting final : public Simulation {
	static void __stdcall debug_gl(
        lwvl::debug::Source source, lwvl::debug::Type type, lwvl::debug::Severity severity,
        unsigned int id, int length, const char* message, const void* userState
	) {
		if (type == lwvl::debug::Type::ERROR) {
			throw std::exception(message);
		}

		else {
			std::cout << "[OpenGL] " << message << std::endl;
		}
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
	lwvl::ShaderProgram lineShader, lightShader, floorShader;
	std::vector<Boundary> bounds;
	CasterConfig casters[4];
	FloorTexture floorTexture;
	std::unique_ptr<Floor> floor;
	lwvl::Uniform lightCenter;

	inline std::unique_ptr<Caster>& currentCaster() {
		return casters[static_cast<size_t>(settings.mode)].caster;
	}

public:
	RayCasting(unsigned int width, unsigned int height, GLFWmonitor* monitor = nullptr) :
		Simulation(width, height, "RayCasting", monitor), m_listener(this, debug_gl, true)
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

		floor = std::make_unique<Floor>(wPad, hPad, floorWidth, floorHeight);

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
//        lightShader.uniform("u_LightColor").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
//        lightShader.uniform("u_LightColor").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
        lightShader.uniform("u_LightColor").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
//        lightShader.uniform("u_LightColor").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayola
		lightShader.uniform("u_Texture").set1i(floorTexture.slot());
		lightCenter = lightShader.uniform("u_MouseCoords");

		floorShader.bind();
		floorShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		floorShader.uniform("u_Texture").set1i(floorTexture.slot());

		// ****** Construct Boundaries ******
		bounds.reserve(14); {
			bounds.emplace_back(wPad, hPad, frameWidth - wPad, hPad);
			bounds.emplace_back(frameWidth - wPad, hPad, frameWidth - wPad, frameHeight - hPad);
			bounds.emplace_back(frameWidth - wPad, frameHeight - hPad, wPad, frameHeight - hPad);
			bounds.emplace_back(wPad, frameHeight - hPad, wPad, hPad);

			const float width14 = frameWidth / 4.0f;
			const float height14 = frameHeight / 4.0f;
			const float width34 = 3.0f * frameWidth / 4.0f;
			const float height34 = 3.0f * frameHeight / 4.0f;

			bounds.emplace_back(width14 - wPad, height14 - hPad, width14 + wPad, height14 - hPad);
			//bounds.emplace_back(width14 + wPad, height14 - hPad, width14 + wPad, height34 + hPad);
			bounds.emplace_back(width14 + wPad, height34 + hPad, width14 - wPad, height34 + hPad);
			bounds.emplace_back(width14 - wPad, height34 + hPad, width14 - wPad, height14 - hPad);

			bounds.emplace_back(width34 - wPad, height14 - hPad, width34 + wPad, height14 - hPad);
			bounds.emplace_back(width34 + wPad, height14 - hPad, width34 + wPad, height34 + hPad);
			bounds.emplace_back(width34 + wPad, height34 + hPad, width34 - wPad, height34 + hPad);
			//bounds.emplace_back(width34 - wPad, height34 + hPad, width34 - wPad, height14 - hPad);

			const float wPad5 = 5.0f * wPad;
			const float hPad5 = 5.0f * hPad;

			bounds.emplace_back(width14 + wPad5, height14, frameWidth / 2.0f, frameHeight / 2.0f);
			bounds.emplace_back(width34 - wPad5, height14, frameWidth / 2.0f, frameHeight / 2.0f);
			bounds.emplace_back(width14 + wPad5, height34, frameWidth / 2.0f, frameHeight / 2.0f);
			bounds.emplace_back(width34 - wPad5, height34, frameWidth / 2.0f, frameHeight / 2.0f);
		}


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
					entity->look(bounds);
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
		floor->draw();

		lightShader.bind();
		entity->draw();

		if (settings.config & SHOW_BOUNDS) {
			lineShader.bind();
			for (Boundary& bound : bounds) {
				bound.draw();
			}
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
