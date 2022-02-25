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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void do_movement();

// Window dimensions
GLuint WIDTH = 1200, HEIGHT = 900;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024];
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// Rotate
bool isRotate = true;

// Filter
int mode = 0;

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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "C2 yyf 2020215224", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
    
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
    GLfloat vertices[] = {
        // Positions          // Texture Coords
	    //Front
        -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, // Top Left
        0.5f, 0.5f, 0.5f,   1.0f, 1.0f, // Top Right
        0.5f,  -0.5f, 0.5f,   1.0f, 0.0f, // Bottom Right
        -0.5f,  -0.5f, 0.5f,   0.0f, 0.0f,  // Bottom Left

	    //Back
        0.5f, 0.5f, -0.5f,   0.0f, 1.0f, // Top Left
        -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, // Top Right
        -0.5f,  -0.5f, -0.5f,   1.0f, 0.0f, // Bottom Right
        0.5f,  -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom Left
		
		//Left
        -0.5f, 0.5f, -0.5f,   0.0f, 1.0f, // Top Left
        -0.5f, 0.5f, 0.5f,   1.0f, 1.0f, // Top Right
        -0.5f,  -0.5f, 0.5f,   1.0f, 0.0f, // Bottom Right
        -0.5f,  -0.5f, -0.5f,   0.0f, 0.0f,  // Bottom Left

	    //Right
        0.5f, 0.5f, 0.5f,   0.0f, 1.0f, // Top Left
        0.5f, 0.5f, -0.5f,   1.0f, 1.0f, // Top Right
        0.5f,  -0.5f, -0.5f,   1.0f, 0.0f, // Bottom Right
        0.5f,  -0.5f, 0.5f,   0.0f, 0.0f,  // Bottom Left
		
		//Top
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, // Top Left
        0.5f, 0.5f, -0.5f,   1.0f, 1.0f, // Top Right
        0.5f, 0.5f, 0.5f,   1.0f, 0.0f, // Bottom Right
        -0.5f,  0.5f, 0.5f,   0.0f, 0.0f,  // Bottom Left

		//Bottom
		0.5f, -0.5f, -0.5f,   0.0f, 1.0f, // Top Left
		-0.5f,  -0.5f, -0.5f,   1.0f, 1.0f, // Top Right
        -0.5f,  -0.5f, 0.5f,   1.0f, 0.0f,  // Bottom Right
        0.5f, -0.5f, 0.5f,   0.0f, 0.0f, // Bottom Left
    };
    GLuint indices[] = {  // Note that we start from 0!
		0, 1, 2, // First Triangle
        2, 3, 0,  // Second Triangle
		4, 5, 6, // First Triangle
        6, 7, 4,  // Second Triangle
	    8, 9, 10, // First Triangle
        10, 11, 8,  // Second Triangle
		12, 13, 14, // First Triangle
        14, 15, 12,  // Second Triangle
		16, 17, 18, // First Triangle
        18, 19, 16,  // Second Triangle
		20, 21, 22, // First Triangle
        22, 23, 20,  // Second Triangle
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
	// Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
    

	// Load and create a texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint texture[18];
	int width, height;
	unsigned char* image;
	for (int i = 0; i < 18; i++)
	{
		glGenTextures(1, &texture[i]);
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		// Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		if ( i < 6)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (i < 12)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		switch ( i % 6)
		{
			case 0:
				image = SOIL_load_image("Front2.jpg", &width, &height, 0, SOIL_LOAD_RGB);
				break;
			case 1:
				image = SOIL_load_image("Back2.jpg", &width, &height, 0, SOIL_LOAD_RGB);
				break;
			case 2:
				image = SOIL_load_image("Left2.jpg", &width, &height, 0, SOIL_LOAD_RGB);
				break;
			case 3:
				image = SOIL_load_image("Right2.jpg", &width, &height, 0, SOIL_LOAD_RGB);
				break;
			case 4:
				image = SOIL_load_image("Top2.jpg", &width, &height, 0, SOIL_LOAD_RGB);
				break;
			case 5:
				image = SOIL_load_image("Bottom2.jpg", &width, &height, 0, SOIL_LOAD_RGB);
				break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
    }

	glm::mat4 model(1);
	glm::mat4 view(1);
	glm::mat4 projection(1);
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
        glClearColor(255.0f/255.0f, 228.0f/255.0f, 225.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate shader
        ourShader.Use();
        
        //Model
		if (isRotate)
			model = glm::rotate(model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		// Camera/View transformation
        view = camera.GetViewMatrix();
        // Projection
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        for (int i = 0; i < 6; i++)
        {
            glBindTexture(GL_TEXTURE_2D, texture[mode * 6 + i]);	
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(6 * i * sizeof(GLuint)));
        }
        glBindVertexArray(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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
	if(keys[GLFW_KEY_1])
	{
	    mode = 0;
		std::cout<<"GL_TEXTURE_MIN_FILTER: GL_LINEAR"<<std::endl;
		std::cout<<"GL_TEXTURE_MAX_FILTER: GL_LINEAR"<<std::endl<<std::endl;
	}
	if(keys[GLFW_KEY_2])
	{
		mode = 1;
		std::cout<<"GL_TEXTURE_MIN_FILTER: GL_LINEAR_MIPMAP_LINEAR"<<std::endl;
		std::cout<<"GL_TEXTURE_MAX_FILTER: GL_LINEAR_MIPMAP_LINEAR"<<std::endl<<std::endl;
	}
	if(keys[GLFW_KEY_3])
	{
		mode = 2;
		std::cout<<"GL_TEXTURE_MIN_FILTER: GL_NEAREST"<<std::endl;
		std::cout<<"GL_TEXTURE_MAX_FILTER: GL_NEAREST"<<std::endl<<std::endl;
	}
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                isRotate = !isRotate;
                break;
            default:
                return;
        }
    }
    return;
}