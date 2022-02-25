#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 800;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "C1 yyf 2020215224", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return -1;
	};

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);


	// Build and compile our shader program
	Shader ourShader_smooth("main.vert.smooth.glsl", "main.frag.smooth.glsl");
	Shader ourShader_flat("main.vert.flat.glsl", "main.frag.flat.glsl");


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions         // Colors
		//Triangle
		0.0f, -0.2f, 0.0f, 255.0f/255.0f, 165.0f/255.0f, 0.0f/255.0f,  // Bottom Left
		0.2f, -0.2f, 0.0f, 255.0f/255.0f, 253.0f/255.0f, 138.0f/255.0f,  // Bottom Right
		-0.2f, 0.2f, 0.0f, 118.0f/255.0f, 238.0f/255.0f, 0.0f/255.0f,   // Top

		//Square(smooth)
		-0.2f, -0.2f, 0.0f, 255.0f/255.0f, 192.0f/255.0f, 203.0f/255.0f,  // Bottom Left
		-0.2f, 0.2f, 0.0f, 193.0f/255.0f, 255.0f/255.0f, 193.0f/255.0f,   // Top Left
		0.2f, 0.2f, 0.0f, 135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f,  // Top Right
		0.2f, 0.2f, 0.0f, 135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f,  // Top Right
		0.2f, -0.2f, 0.0f, 171.0f/255.0f, 130.0f/255.0f, 255.0f/255.0f,  // Bottom Right
		-0.2f, -0.2f, 0.0f, 255.0f/255.0f, 192.0f/255.0f, 203.0f/255.0f,  // Bottom Left

		//Square(flat)
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Center
		-0.2f, -0.2f, 0.0f, 255.0f/255.0f, 192.0f/255.0f, 203.0f/255.0f,  // Bottom Left
		-0.2f, 0.2f, 0.0f, 193.0f/255.0f, 255.0f/255.0f, 193.0f/255.0f,   // Top Left
		0.2f, 0.2f, 0.0f, 135.0f/255.0f, 206.0f/255.0f, 250.0f/255.0f,  // Top Right
		0.2f, -0.2f, 0.0f, 171.0f/255.0f, 130.0f/255.0f, 255.0f/255.0f,  // Bottom Right
		-0.2f, -0.2f, 0.0f, 255.0f/255.0f, 192.0f/255.0f, 203.0f/255.0f,  // Bottom Left
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	GLfloat x0[3] = {-0.7f, 0.7f, -0.7f}, time[3] = {0.0f, 0.0f, 0.0f};
	bool moveRight[3] = {true, false, true};

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(96.0f/255.0f, 123.0f/255.0f, 139.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 model[3];
		for (int i = 0; i < 3; i++)
		{
		    model[i] = glm::mat4(1.0f);
		    x0[i] = moveRight[i] ? -0.7f + ((GLfloat)glfwGetTime() - time[i]) * 0.2f : 0.7f - ((GLfloat)glfwGetTime() - time[i]) * 0.2f;
			if (x0[i] <= -0.7f || x0[i] >= 0.7f)
			{
				time[i] = (GLfloat)glfwGetTime();
				moveRight[i] = !moveRight[i];
			}
		}

		// Draw the triangle
		GLint modelLoc_smooth = glGetUniformLocation(ourShader_smooth.Program, "model");
		GLint modelLoc_flat = glGetUniformLocation(ourShader_flat.Program, "model");

		glBindVertexArray(VAO);

		ourShader_smooth.Use();
		model[0] = glm::translate(model[0], glm::vec3(x0[0], 0.6f, 0.0f));
		model[0] = glm::rotate(model[0], (GLfloat)glfwGetTime() * glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc_smooth, 1, GL_FALSE, glm::value_ptr(model[0]));
		glDrawArrays(GL_TRIANGLES, 0, 3);
		model[1] = glm::translate(model[1], glm::vec3(x0[1], 0.0f, 0.0f));
		model[1] = glm::rotate(model[1], (GLfloat)glfwGetTime() * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc_smooth, 1, GL_FALSE, glm::value_ptr(model[1]));
		glDrawArrays(GL_TRIANGLES, 3, 6);

		ourShader_flat.Use();
		model[2] = glm::translate(model[2], glm::vec3(x0[2], -0.6f, 0.0f));
		model[2] = glm::rotate(model[2], (GLfloat)glfwGetTime() * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc_flat, 1, GL_FALSE, glm::value_ptr(model[2]));
		glDrawArrays(GL_TRIANGLE_FAN, 9, 6);

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}