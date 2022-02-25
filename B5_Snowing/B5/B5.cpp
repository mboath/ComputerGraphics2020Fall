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
#include "camera.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Structure
struct Flake{
	GLfloat x0;
	GLfloat y0;
	GLfloat z0;
	GLfloat speed;
	GLfloat size;
};

// Window dimensions
GLuint WIDTH = 1920, HEIGHT = 1080;

// Parameters
const GLfloat skyHeight = 20.0f;
const GLfloat skyLength = 40.0f;
const GLfloat skyWidth = 20.0f;
const GLfloat minSpeed = 1.0f;
const GLfloat maxSpeed = 4.0f;
const GLfloat minSize = 0.1f;
const GLfloat maxSize = 0.5f;
const int maxNum = 100;
const GLfloat constTime = 1.0f; 
int num_per_second = 1;
int num = 0;
GLfloat genTime = 0.0f;

// Camera
Camera camera(glm::vec3(0.0f, skyHeight / 2.0f, skyWidth), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -10.0f);
bool keys[1024];
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame


// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "B5 yyf 2020215224", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
    
    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");
    
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat planeLength = skyLength + 2.0f;
	GLfloat planeWidth = skyWidth + 2.0f;
	GLfloat vertices[] = {
        // Positions          // Texture Coords
		//Flake
        -0.5f, 0.5f, 0.0f,   0.0f, 1.0f, // Top Left
        0.5f, 0.5f, 0.0f,   1.0f, 1.0f, // Top Right
        0.5f,  -0.5f, 0.0f,   1.0f, 0.0f, // Bottom Right
		0.5f,  -0.5f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -0.5f,  -0.5f, 0.0f,   0.0f, 0.0f,  // Bottom Left
		-0.5f, 0.5f, 0.0f,   0.0f, 1.0f, // Top Left

		//Plane
		-skyLength/2.0f, 0.0f, -skyWidth/2.0f,   0.0f, skyWidth/2.0f, // Top Left
		skyLength/2.0f, 0.0f, -skyWidth/2.0f,   skyHeight/2.0f, skyWidth/2.0f, // Top Right
		skyLength/2.0f, 0.0f, skyWidth/2.0f,    skyHeight/2.0f, 0.0f, // Bottom Right
		skyLength/2.0f, 0.0f, skyWidth/2.0f,    skyHeight/2.0f, 0.0f, // Bottom Right
		-skyLength/2.0f, 0.0f, skyWidth/2.0f,   0.0f, 0.0f, // Bottom Left
		-skyLength/2.0f, 0.0f, -skyWidth/2.0f,   0.0f, skyWidth/2.0f // Top Left
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
	GLuint texture1, texture2;
	// ====================
    // Texture 1
    // ====================
	glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("SnowFlakeAlpha.png", &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
	// ===================
    // Texture 2
    // ===================
	glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); 
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    image = SOIL_load_image("Ground.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);


	srand((unsigned int)(time(NULL)));
	std::vector <Flake> flakes;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Render
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate shader
        ourShader.Use();
        
		
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");

		//Model
        glm::mat4 model(1);
		// Camera/View transformation
		glm::mat4 view(1);
        view = camera.GetViewMatrix();
        // Projection
		glm::mat4 projection(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		

		glBindVertexArray(VAO);

		//Draw ground
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 6, 6);

		//Draw flakes
		glBindTexture(GL_TEXTURE_2D, texture1);
		std::vector<Flake>::iterator itr = flakes.begin();
		while (itr != flakes.end())
		{
			if (itr -> y0 > itr -> size / 2.0f)
			{
				model = glm::mat4(1);
				model = glm::translate(model, glm::vec3(itr -> x0, itr -> y0, itr -> z0));
				model = glm::scale(model, glm::vec3(itr -> size, itr -> size, itr -> size));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				itr -> y0 -= itr -> speed *deltaTime;
				itr ++;
			}
			else
				itr = flakes.erase(itr);
		}

		glBindVertexArray(0);
		

		// Create new flake
		genTime += deltaTime;
		//std::cout<<genTime<<std::endl;
		if (genTime >= 1.0f / num_per_second)
		{
			Flake newFlake;
			newFlake.x0 = ((GLfloat)rand() / (GLfloat)RAND_MAX - 0.5f) * skyLength;
			newFlake.z0 = ((GLfloat)rand() / (GLfloat)RAND_MAX - 0.5f) * skyWidth;
			newFlake.speed = (GLfloat)rand() / (GLfloat)RAND_MAX * (maxSpeed - minSpeed) + minSpeed;
			newFlake.size = (GLfloat)rand() / (GLfloat)RAND_MAX * (maxSize - minSize) + minSize;
			newFlake.y0 = skyHeight - newFlake.size / 2.0f;
			//std::cout<<num<<' '<<newFlake.x0<<' '<<newFlake.y0<<' '<<newFlake.z0<<' '<<newFlake.speed<<' '<<newFlake.size<<std::endl;
			flakes.push_back(newFlake);
			genTime = 0.0f;
			num += 1;
		}
		
		// Increase generating speed by time
        if (num >= num_per_second * constTime)
		{
			num = 0;
			num_per_second += 2;
			if (num_per_second >= maxNum)
				num_per_second = maxNum;
			std::cout<<num_per_second<<std::endl;
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


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}