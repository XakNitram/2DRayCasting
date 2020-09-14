#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"


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

		if (!window) {
			terminateGLFW();
			throw std::exception("Failed to create GLFW window.");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, handleKeys);

		/* Make the window's context current. */
		glfwMakeContextCurrent(window);

		/* Enable VSync */
		glfwSwapInterval(0);

		/* Initialize GLEW. */
		if (glewInit() != GLEW_OK) {
			this->~Simulation();
			throw std::exception("Failed to initialize GLEW.");
		}

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
		while (!glfwWindowShouldClose(window)) {
			// Clear the screen.
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

	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
}