#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Vector.h"
#include "Utils.h"

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
        Shader boundShader(vertexSource, fragmentSource);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		std::string projectionUniformName = "u_Projection";
		Location u_projection = boundShader.uniformLocation(projectionUniformName);

		float widthf = float(width);
		float heightf = float(height);

		if (u_projection) {
			boundShader.setOrthographic2D(u_projection, heightf, 0.0f, widthf, 0.0f);
		}

		while (!glfwWindowShouldClose(window)) {
			/* Update shaders and render. */
			glClear(GL_COLOR_BUFFER_BIT);
			
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
