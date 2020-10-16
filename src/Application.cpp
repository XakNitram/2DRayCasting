#include "rcpch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Utils.h"
#include "Core/Simulation.h"
#include "Renderer/Shader.h"
#include "Math/Geometrics.h"
#include "Math/Boundary.h"
#include "Casters/AngleCaster.h"
#include "Casters/EndPointCaster.h"
#include "Primitives/Quad.h"
#include "Primitives/Floor.h"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156


constexpr unsigned int FOLLOW_MOUSE = 0b1;
constexpr unsigned int SHOW_BOUNDS  = 0b10;


struct CasterConfig {
	float prevX, prevY;
	std::unique_ptr<Caster> caster;

	CasterConfig(std::unique_ptr<Caster>&& caster) : prevX(0.0), prevY(0.0), caster(std::move(caster)) {}
	CasterConfig() : prevX(0.0f), prevY(0.0f), caster(nullptr) {}

	void setCaster(std::unique_ptr<Caster>&& caster) { this->caster = std::move(caster); }
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
	SimulationConfig settings;
	GLuint floorTexture;
	ShaderProgram lineShader, lightShader, floorShader;
	std::vector<Boundary> bounds;
	CasterConfig casters[4];
	std::unique_ptr<Floor> floor;
	Uniform lightCenter;

	inline std::unique_ptr<Caster>& currentCaster() {
		return casters[static_cast<size_t>(settings.mode)].caster;
	}

public:
	~RayCasting() final { glDeleteTextures(1, &floorTexture); }
	RayCasting(unsigned int width, unsigned int height, GLFWmonitor* monitor = nullptr) : Simulation(width, height, "RayCasting", monitor) {
		attachKeyCallback();
		swapInterval(1);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// ****** Set Up OpenGL Stuff ******
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// ****** Get Actual Frame Size ******
		int iFrameWidth, iFrameHeight;
		glfwGetFramebufferSize(window, &iFrameWidth, &iFrameHeight);

		const float frameWidth = static_cast<float>(iFrameWidth);
		const float frameHeight = static_cast<float>(iFrameHeight);
		settings.frameWidth  = frameWidth;
		settings.frameHeight = frameHeight;

		const float wPad = (10.0f * frameWidth) / 800.0f;
		const float hPad = (10.0f * frameHeight) / 600.0f;

		float floorTextureWidth = frameWidth - (2.0f * wPad);
		float floorTextureHeight = frameHeight - (2.0f * hPad);

		floor = std::make_unique<Floor>(wPad, hPad, floorTextureWidth, floorTextureHeight);

		// ****** Render Floor Texture ******
		glGenTextures(1, &floorTexture); {
			GLsizei ftwi = static_cast<GLsizei>(floorTextureWidth);
			GLsizei fthi = static_cast<GLsizei>(floorTextureHeight);
			GLuint floorFrame;
			glGenFramebuffers(1, &floorFrame);
			glBindFramebuffer(GL_FRAMEBUFFER, floorFrame);

			glBindTexture(GL_TEXTURE_2D, floorTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ftwi, fthi, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, floorTexture, 0);

			Quad textureBase(-1.0f, -1.0f, 2.0f, 2.0f);

			ShaderProgram floorTexturePipeline;
			VertexShader floorVertex("#version 330 core\nlayout(location=0) in vec4 position;\nvoid main() { gl_Position = position; }");
			//FragmentShader floorFragment(readFile("Shaders/default.frag"));
			FragmentShader floorFragment(readFile("Shaders/mazing.frag"));
			floorTexturePipeline.link(floorVertex, floorFragment);

			floorTexturePipeline.bind();
			//floorTexturePipeline.uniform("u_Color").set3f(1.0f, 0.0f, 0.0f);
			floorTexturePipeline.uniform("u_Resolution").set2f(floorTextureWidth, floorTextureHeight);

			GLsizei prevViewport[4];
			glGetIntegerv(GL_VIEWPORT, prevViewport);
			glViewport(0, 0, ftwi, fthi);
			textureBase.draw();
			glViewport(0, 0, prevViewport[2], prevViewport[3]);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &floorFrame);
		}

		// ****** Construct Shaders ******
		lineShader.link(readFile("Shaders/default.vert"), readFile("Shaders/default.frag"));
		lightShader.link(readFile("Shaders/light.vert"), readFile("Shaders/light.frag"));
		//lightShader.link(readFile("Shaders/light.vert"), readFile("Shaders/floor.frag"));
		floorShader.link(readFile("Shaders/floor.vert"), readFile("Shaders/floor.frag"));

		// ****** Initialize Shader Uniforms ******
		lineShader.bind();
		lineShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		lineShader.uniform("u_Color").set3f(1.0f, 1.0f, 1.0f);

		lightShader.bind();
		lightShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		lightShader.uniform("u_Resolution").set2f(floorTextureWidth, floorTextureHeight);
		lightShader.uniform("u_Offset").set2f(wPad, hPad);
		lightShader.uniform("u_Texture").set1i(0);
		lightCenter = lightShader.uniform("u_MouseCoords");

		floorShader.bind();
		floorShader.uniform("u_Projection").set2DOrthographic(frameHeight, 0.0f, frameWidth, 0.0f);
		floorShader.uniform("u_Texture").set1i(0);

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

		const size_t renderMode = static_cast<size_t>(settings.mode);
		CasterConfig& mode = casters[renderMode];
		auto& entity = mode.caster;

		if (settings.config & FOLLOW_MOUSE) {
			double mousePosX, mousePosY;
			glfwGetCursorPos(window, &mousePosX, &mousePosY);
			const float mousePosXf = static_cast<float>(mousePosX);
			const float mousePosYf = static_cast<float>(mousePosY);

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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		floorShader.bind();
		floor->draw();

		lightShader.bind();
		entity->draw();

		if (settings.config & SHOW_BOUNDS) {
			lineShader.bind();
			for (const Boundary& bound : bounds) {
				bound.draw();
			}
		}
	}
};


int main() {
	try {
		// Borderless window
		RayCasting::initGLFW();
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		RayCasting sim(mode->width, mode->height, monitor);
		//RayCasting sim(800, 600);
		sim.run();

		return 0;
	}

	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
}
