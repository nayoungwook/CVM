#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char** args) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", 0, 0);

	glfwMakeContextCurrent(window);

	gladLoadGL();

	while (!glfwWindowShouldClose(window)) {
		double time = glfwGetTime();

		GLfloat color[3] = { (float)std::abs(std::sin(time)), (float)std::abs(std::cos(time)), (float)std::abs(std::sin(time)) };

		glClearBufferfv(GL_COLOR, 0, color);

		glViewport(0, 0, 1280, 720);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}