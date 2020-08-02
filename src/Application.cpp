#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Utils.h"
#include "Boundary.h"
#include "AngleCaster.h"

// Code Signing: https://stackoverflow.com/questions/16673086/how-to-correctly-sign-an-executable/48244156


class Simulation {
	GLFWwindow* window;

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
		}
	}

public:
	Simulation(unsigned int width, unsigned int height) {
		// Destructor is not called if exception is thrown from constructor.

		/* Initialize GLFW. */
		std::cout << "Initializing GLFW." << std::endl;
		if (!glfwInit()) {
			throw std::exception("Failed to initialize GLFW.");
		}

		glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

		/* Create a windowed mode window and its OpenGL context. */
		window = glfwCreateWindow(width, height, "Ray Casting", nullptr, nullptr);

		if (!window) {
			terminateGLFW();
			throw std::exception("Failed to create GLFW window.");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, handleKeys);

		/* Make the window's context current. */
		glfwMakeContextCurrent(window);

		/* Enable VSync */
		glfwSwapInterval(1);

		/* Initialize GLEW. */
		if (glewInit() != GLEW_OK) {
			this->~Simulation();
			throw std::exception("Failed to initialize GLEW.");
		}

		/* Output the current OpenGL version. */
		std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
	}

	~Simulation() {
		destroyWindow();
		terminateGLFW();
	}

	void run() {
		std::string vertexSource = readFile("Shaders/default.vert");
		std::string fragmentSource = readFile("Shaders/default.frag");
        Shader boundaryShader(vertexSource, fragmentSource);
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

		std::vector<Boundary> bounds;
		bounds.reserve(16); {
			const float wPad = (10.0f * widthf)  / 400.0f;
			const float hPad = (10.0f * heightf) / 400.0f;

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

		AngleCaster entity(widthf / 2.0f, heightf / 2.0f);

		while (!glfwWindowShouldClose(window)) {
			double mousePosX, mousePosY;
			glfwGetCursorPos(window, &mousePosX, &mousePosY);
			
			entity.update(float(mousePosX), heightf - float(mousePosY));
			entity.look(bounds);

			setShaderTime(glfwGetTime());

			/****** Update shaders and render. ******/
			// Clear the screen.
			glClear(GL_COLOR_BUFFER_BIT);

			// Draw caster.
			lightShader.bind();
			entity.draw();

			// Draw boundaries.
			boundaryShader.bind();
			for (const Boundary& bound : bounds) {
				bound.draw();
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
