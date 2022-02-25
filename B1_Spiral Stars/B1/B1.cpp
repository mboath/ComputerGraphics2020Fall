#include <iostream>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <time.h>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

// Other includes
#include "shader.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// Other parameters
const GLfloat thetaSpeed = 45.0f;
const GLfloat deltaTheta = 22.5f;
const GLfloat maxTheta = 1800.0f;
const int starNum = 80;
const GLfloat b = 0.0415f;
GLfloat curTheta = 0.0f;


// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "B1 yyf 2020215224", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    
    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");
    
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // Positions          // Texture Coords
        -0.125f, 0.125f, 0.0f,   0.0f, 1.0f, // Top Left
        0.125f, 0.125f, 0.0f,   1.0f, 1.0f, // Top Right
        0.125f,  -0.125f, 0.0f,   1.0f, 0.0f, // Bottom Right
		0.125f,  -0.125f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -0.125f,  -0.125f, 0.0f,   0.0f, 0.0f,  // Bottom Left
		-0.125f, 0.125f, 0.0f,   0.0f, 1.0f // Top Left
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
	// Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
    

	// Load and create a texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texture;
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("Star.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);


	// Create random colors
	srand((unsigned int)(time(NULL)));
	std::vector <glm::vec3> starColor;
	int rr, gg, bb;
	for (int i = 0; i < starNum; i++)
	{
		do{
			rr = rand() % 256;
			gg = rand() % 256;
			bb = rand() % 256;
		}while(rr == 0 && gg == 0 && bb == 0);
		starColor.push_back(glm::vec3(((GLfloat)rr) / 255.0f, ((GLfloat)gg) / 255.0f, ((GLfloat)bb) / 255.0f));
	}


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Activate shader
        ourShader.Use();
        
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint colorLoc = glGetUniformLocation(ourShader.Program, "starColor");

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);

	    int curNum = (int)floor(curTheta / deltaTheta) + 1;
		GLfloat x, y;
		for (int i = curNum - 1; i >= 0; i--)
		{
			GLfloat theta = curTheta - i * deltaTheta;
			x = b * glm::radians(theta) * cos(glm::radians(theta));
			y = b * glm::radians(theta) * sin(glm::radians(theta));
			glm::mat4 model(1);
			model = glm::translate(model, glm::vec3(x, y, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		    glUniform3f(colorLoc, starColor[i].x, starColor[i].y, starColor[i].z);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);
		
        curTheta += thetaSpeed * deltaTime;
		if (curTheta >= maxTheta)
		{
			curTheta -= deltaTheta;
			starColor.erase(starColor.begin());
			int rr, gg, bb;
			do{
				rr = rand() % 256;
				gg = rand() % 256;
				bb = rand() % 256;
			}while(rr == 0 && gg == 0 && bb == 0);
			starColor.push_back(glm::vec3(((GLfloat)rr) / 255.0f, ((GLfloat)gg) / 255.0f, ((GLfloat)bb) / 255.0f));
		}
        
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
