#include "rcpch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Utils.h"
#include "Renderer/Shader.h"
#include "Math/Geometrics.h"
#include "Math/Boundary.h"
#include "Casters/AngleCaster.h"
#include "Casters/EndPointCaster.h"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156


enum RenderMode {
	LINE_ANGLE      = 0,
	FILLED_ANGLE    = 1,
	LINE_ENDPOINT   = 2,
	FILLED_ENDPOINT = 3,
};


enum ConfigToggle {
	SHOW_BOUNDS  = 1,
	FOLLOW_MOUSE = 2
};


struct CasterConfig {
	float prevX, prevY;
	std::unique_ptr<Caster> caster;

	CasterConfig(std::unique_ptr<Caster>&& caster): prevX(0.0), prevY(0.0), caster(std::move(caster)) {}
	CasterConfig() : prevX(0.0f), prevY(0.0f), caster(nullptr) {}
	
	void setCaster(std::unique_ptr<Caster>&& caster) { this->caster = std::move(caster); }
};


class Simulation {
	GLFWwindow* window;
	struct {
		unsigned int renderMode = FILLED_ENDPOINT;
		unsigned int configBits = 0x0003;
	} settings;

	static void terminateGLFW() {
		std::cout << "Terminating GLFW." << std::endl;
		glfwTerminate();
	}

	void destroyWindow() {
		std::cout << "Destroying main window." << std::endl;

		glfwSetWindowUserPointer(window, nullptr);
		glfwDestroyWindow(window);
		glfwPollEvents();
	}

	static void handleKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, true);
			return;
		}

		else {
			void* userPointer = glfwGetWindowUserPointer(window);

			if (!userPointer) {
				return;
			}

			Simulation& self = *static_cast<Simulation*>(userPointer);

			if (action == GLFW_RELEASE) {
				switch (key) {
				case GLFW_KEY_1:
					self.settings.renderMode = FILLED_ENDPOINT;
					break;
				case GLFW_KEY_2:
					self.settings.renderMode = LINE_ENDPOINT;
					break;
				case GLFW_KEY_3:
					self.settings.renderMode = FILLED_ANGLE;
					break;
				case GLFW_KEY_4:
					self.settings.renderMode = LINE_ANGLE;
					break;
				case GLFW_KEY_B:
					self.settings.configBits ^= SHOW_BOUNDS;
					break;
				case GLFW_KEY_SPACE:
					self.settings.configBits ^= FOLLOW_MOUSE;
					break;
				default:
					return;
				}
			}
		}
	}

public:
	Simulation(unsigned int width, unsigned int height) {
		// Destructor is not called if exception is thrown from constructor.

		/* Initialize GLFW. */
#ifdef _DEBUG
		std::cout << "Initializing GLFW." << std::endl;
#endif // _DEBUG
		if (!glfwInit()) {
			throw std::exception("Failed to initialize GLFW.");
		}

		glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

		/* Create a windowed mode window and its OpenGL context. */
		window = glfwCreateWindow(width, height, "Ray Casting", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		if (!window) {
			terminateGLFW();
			throw std::exception("Failed to create GLFW window.");
		}

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			this->~Simulation();
			throw std::exception("Failed to initialize Glad.");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, handleKeys);

		/* Enable VSync */
		glfwSwapInterval(1);

		/* Output the current OpenGL version. */
#ifdef _DEBUG
		std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
#endif // _DEBUG
	}

	~Simulation() {
		destroyWindow();
		terminateGLFW();
	}

	void run() {
		std::string vertexSource = readFile("Shaders/default.vert");
		std::string fragmentSource = readFile("Shaders/default.frag");
        Shader boundaryShader(vertexSource, fragmentSource);

		//fragmentSource = readFile("Shaders/spacetime.frag");
		Shader lightShader(vertexSource, fragmentSource);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		float widthf = float(width);
		float heightf = float(height);

		Location u_projection = boundaryShader.uniformLocation("u_Projection");
		if (u_projection) { boundaryShader.setOrthographic2D(u_projection, heightf, 0.0f, widthf, 0.0f); }
		
		u_projection = lightShader.uniformLocation("u_Projection");
		if (u_projection) { lightShader.setOrthographic2D(u_projection, heightf, 0.0f, widthf, 0.0f); }

		boundaryShader.iUniform3f(boundaryShader.uniformLocation("u_Color"), 1.0f, 1.0f, 1.0f);
		lightShader.iUniform2f(lightShader.uniformLocation("u_Resolution"), widthf, heightf);

		Location u_time = lightShader.uniformLocation("u_Time");
		double startTime = glfwGetTime();
		auto setShaderTime = [&](double time) {
			lightShader.iUniform1f(u_time, float(startTime - time));
		};

		setShaderTime(glfwGetTime());

		const float wPad = (10.0f * widthf) / 400.0f;
		const float hPad = (10.0f * heightf) / 400.0f;

		std::vector<Boundary> bounds;
		bounds.reserve(16); {
			bounds.emplace_back(wPad, hPad, widthf - wPad, hPad);
			bounds.emplace_back(widthf - wPad, hPad, widthf - wPad, heightf - hPad);
			bounds.emplace_back(widthf - wPad, heightf - hPad, wPad, heightf - hPad);
			bounds.emplace_back(wPad, heightf - hPad, wPad, hPad);

			const float width14 = widthf / 4.0f;
			const float height14 = heightf / 4.0f;
			const float width34 = 3.0f * widthf / 4.0f;
			const float height34 = 3.0f * heightf / 4.0f;

			bounds.emplace_back(width14 - wPad, height14 - hPad, width14 + wPad, height14 - hPad);
			bounds.emplace_back(width14 + wPad, height14 - hPad, width14 + wPad, height34 + hPad);
			bounds.emplace_back(width14 + wPad, height34 + hPad, width14 - wPad, height34 + hPad);
			bounds.emplace_back(width14 - wPad, height34 + hPad, width14 - wPad, height14 - hPad);

			bounds.emplace_back(width34 - wPad, height14 - hPad, width34 + wPad, height14 - hPad);
			bounds.emplace_back(width34 + wPad, height14 - hPad, width34 + wPad, height34 + hPad);
			bounds.emplace_back(width34 + wPad, height34 + hPad, width34 - wPad, height34 + hPad);
			bounds.emplace_back(width34 - wPad, height34 + hPad, width34 - wPad, height14 - hPad);

			const float wPad5 = 5.0f * wPad;
			const float hPad5 = 5.0f * hPad;

			bounds.emplace_back(width14 + wPad5, height14, widthf / 2.0f, heightf / 2.0f);
			bounds.emplace_back(width34 - wPad5, height14, widthf / 2.0f, heightf / 2.0f);
			bounds.emplace_back(width14 + wPad5, height34, widthf / 2.0f, heightf / 2.0f);
			bounds.emplace_back(width34 - wPad5, height34, widthf / 2.0f, heightf / 2.0f);
		}
		
		const float width24 = widthf / 2.0f;
		const float height24 = heightf / 2.0f;
		const unsigned int numBounds = bounds.size();

		CasterConfig casters[4];
		casters[FILLED_ENDPOINT].setCaster(std::make_unique<FilledEndPointCaster>(width24, height24, numBounds));
		casters[ LINE_ENDPOINT ].setCaster(std::make_unique<EndPointCaster>(width24, height24, numBounds));
		casters[  FILLED_ANGLE ].setCaster(std::make_unique<FilledAngleCaster>(width24, height24));
		casters[   LINE_ANGLE  ].setCaster(std::make_unique<AngleCaster>(width24, height24));

		while (!glfwWindowShouldClose(window)) {
			const unsigned int renderMode = settings.renderMode;
			CasterConfig& mode = casters[renderMode];
			auto& entity = mode.caster;

			if (settings.configBits & FOLLOW_MOUSE) {
				double mousePosX, mousePosY;
				glfwGetCursorPos(window, &mousePosX, &mousePosY);
				const float mousePosXf = static_cast<float>(mousePosX);
				const float mousePosYf = static_cast<float>(mousePosY);

				if (!(mousePosXf == mode.prevX && mousePosYf == mode.prevY)) {
					if (!(mousePosXf < wPad || mousePosXf > widthf - wPad || mousePosYf < hPad || mousePosYf > heightf - hPad)) {
						entity->update(mousePosXf, heightf - mousePosYf);
						entity->look(bounds);
					}
				}

				mode.prevX = mousePosXf;
				mode.prevY = mousePosYf;
			}

			setShaderTime(glfwGetTime());

			/****** Update shaders and render. ******/
			// Clear the screen.
			glClear(GL_COLOR_BUFFER_BIT);

			// Draw caster.
			lightShader.bind();
			entity->draw();

			// Draw boundaries.
			if (settings.configBits & SHOW_BOUNDS) {
				boundaryShader.bind();
				for (const Boundary& bound : bounds) {
					bound.draw();
				}
			}
			
			/* Swap front and back buffers. */
			glfwSwapBuffers(window);
			
			/* Poll for and process events. */
			glfwPollEvents();
		}
	}
};


int main() {
	try {
		Simulation sim(800, 600);
		sim.run();
		
		return 0;
	}
	
	catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
}
