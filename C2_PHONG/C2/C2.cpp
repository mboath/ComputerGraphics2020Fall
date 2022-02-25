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
Camera camera(glm::vec3(-1.0f + 1.0f, 2.0f + 2.0f, 4.0f + 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -76.0f, -26.5f);
bool keys[1024];
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// Rotate
bool isRotate = false;
GLfloat rotateTime = 0.0f;
bool isChanging = false;
GLfloat changingTime = 0.0f;

// Material and Light
GLfloat matShine = 64.0f;

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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "C3 yyf 2020215224", NULL, NULL);
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
    
    glEnable(GL_DEPTH_TEST);
    
    
    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");
    
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // x     y     z               normal                           color
        0.0f, sqrt(2.0f), 0.0f,   0.0f, 1.0f, -sqrt(2.0f),   255.0f/255.0f, 165.0f/255.0f, 0.0f/255.0f, 
        1.0f, 0.0f, -1.0f,   0.0f, 1.0f, -sqrt(2.0f),   0.0f/255.0f, 139.0f/255.0f, 69.0f/255.0f,
        -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, -sqrt(2.0f),   238.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f,

        0.0f, sqrt(2.0f), 0.0f,   sqrt(2.0f), 1.0f, 0.0f,   255.0f/255.0f, 165.0f/255.0f, 0.0f/255.0f, 
        1.0f, 0.0f, 1.0f,   sqrt(2.0f), 1.0f, 0.0f,   0.0f/255.0f, 0.0f/255.0f, 205.0f/255.0f,
        1.0f, 0.0f, -1.0f,   sqrt(2.0f), 1.0f, 0.0f, 0.0f/255.0f, 139.0f/255.0f, 69.0f/255.0f,

        0.0f, sqrt(2.0f), 0.0f,   0.0f, 1.0f, sqrt(2.0f),   255.0f/255.0f, 165.0f/255.0f, 0.0f/255.0f, 
        -1.0f, 0.0f, 1.0f,   0.0f, 1.0f, sqrt(2.0f),   125.0f/255.0f, 38.0f/255.0f, 205.0f/255.0f,
        1.0f, 0.0f, 1.0f,   0.0f, 1.0f, sqrt(2.0f),   0.0f/255.0f, 0.0f/255.0f, 205.0f/255.0f,

		0.0f, sqrt(2.0f), 0.0f,   -sqrt(2.0f), 1.0f, 0.0f,   255.0f/255.0f, 165.0f/255.0f, 0.0f/255.0f, 
        -1.0f, 0.0f, -1.0f,   -sqrt(2.0f), 1.0f, 0.0f,   238.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f,
        -1.0f, 0.0f, 1.0f,   -sqrt(2.0f), 1.0f, 0.0f,   125.0f/255.0f, 38.0f/255.0f, 205.0f/255.0f,

		-1.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,   238.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f,
		1.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,   0.0f/255.0f, 139.0f/255.0f, 69.0f/255.0f,
		1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f/255.0f, 0.0f/255.0f, 205.0f/255.0f,

		1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f/255.0f, 0.0f/255.0f, 205.0f/255.0f,
		-1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,   125.0f/255.0f, 38.0f/255.0f, 205.0f/255.0f,
		-1.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,   238.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f,
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
    
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
        glClearColor(79.0f/255.0f, 79.0f/255.0f, 79.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate shader
        ourShader.Use();
        
		//Model
		if (isRotate)
			rotateTime += deltaTime;
		glm::mat4 model(1);
		model = glm::translate(model, glm::vec3(1.0f, 2.0f, 3.0f));
		model = glm::rotate(model, rotateTime * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // Camera/View transformation
		glm::mat4 view(1);
        view = camera.GetViewMatrix();
        // Projection
		glm::mat4 projection(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
        // Pass the matrices to the shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
		
		//Material
		GLint matSpecularLoc = glGetUniformLocation(ourShader.Program, "material.specular");
		GLint matShineLoc = glGetUniformLocation(ourShader.Program, "material.shininess");

		glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
		glUniform1f(matShineLoc, matShine);

		//Light
		GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "light.position");
		GLint lightAmbientLoc = glGetUniformLocation(ourShader.Program, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(ourShader.Program, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(ourShader.Program, "light.specular");
		
		glm::vec3 lightColor;
		if (isChanging)
		{
			changingTime += deltaTime;
			lightColor.x = sin(changingTime * 2.0f);
			lightColor.y = sin(changingTime * 0.7f);
			lightColor.z = sin(changingTime * 1.3f);
		}
		else
			lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.6f);
		glUniform3f(lightPosLoc, 1.0f + 1.0f, 4.0f + 2.0f, 4.0f + 3.0f);
		glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

		//Draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);
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
	if (keys[GLFW_KEY_MINUS] || keys[GLFW_KEY_KP_SUBTRACT])
	{
		if (matShine > 1.0f)
			matShine -= 0.01f * matShine;
		std::cout<< matShine << std::endl;
	}
	if (keys[GLFW_KEY_EQUAL] || keys[GLFW_KEY_KP_ADD])
	{
		if (matShine < 1024.0f)
			matShine += 0.01f * matShine;
		std::cout<< matShine << std::endl;
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
			case GLFW_MOUSE_BUTTON_RIGHT:
				isChanging = !isChanging;
				changingTime = 0.0f;
                break;
            default:
                return;
        }
    }
    return;
}