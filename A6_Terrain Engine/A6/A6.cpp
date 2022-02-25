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
void do_movement();

// Window dimensions
GLuint WIDTH = 1200, HEIGHT = 900;

// Parameters
const GLfloat skyBoxLength = 200.0f;
const GLfloat skyBoxWidth = 280.0f;
const GLfloat waveSpeedX = -0.02f;
const GLfloat waveSpeedY = -0.02f;
const GLfloat waterAlpha = 0.8f;
const GLfloat unitHeight = 0.015f;
const GLfloat terrainLength = 20.0f;
const GLfloat X0 = - terrainLength / 2.0f;
const GLfloat Z0 = - terrainLength / 2.0f;
const GLfloat Y0 = - 100.0f;
const GLfloat detailScale = 10.0f;

// Camera
Camera camera(glm::vec3(0.0, 2.5f, 10.0f));
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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "A6 yyf 2020215224", NULL, NULL);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");
	Shader ourShaderTerrain("terrain.vert.glsl", "terrain.frag.glsl");
    
	// Get the uniform locations
    GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
    GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
    GLint offsetLoc = glGetUniformLocation(ourShader.Program, "offset");
    GLint alphaLoc = glGetUniformLocation(ourShader.Program, "alpha");
	GLint reflectLoc = glGetUniformLocation(ourShader.Program, "reflection");

    GLint viewLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "view");
    GLint projLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "projection");
    GLint xLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "x");
	GLint zLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "z");
	GLint heightLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "height");
	GLint offsetLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "offset");
	GLint scaleLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "scale");
	GLint reflectLocTerrain = glGetUniformLocation(ourShaderTerrain.Program, "reflection");

    
    // Set up vertex data (and buffer(s)) and attribute pointers
	// ====================
    // Skybox and Water
    // ====================
	GLfloat vertices[] = {
        // Positions          // Texture Coords
		// Front (SkyBox0)
		-skyBoxLength/2.0f, skyBoxLength, -skyBoxWidth/2.0f,   0.0f, 1.0f,
		skyBoxLength/2.0f, skyBoxLength, -skyBoxWidth/2.0f,   1.0f, 1.0f,
		skyBoxLength/2.0f, 0.0f, -skyBoxWidth/2.0f,   1.0f, 0.0f,
		-skyBoxLength/2.0f, 0.0f, -skyBoxWidth/2.0f,   0.0f, 0.0f,

		// Right (SkyBox1)
		skyBoxLength/2.0f, skyBoxLength, -skyBoxWidth/2.0f,   0.0f, 1.0f,
		skyBoxLength/2.0f, skyBoxLength, skyBoxWidth/2.0f,   1.0f, 1.0f,
		skyBoxLength/2.0f, 0.0f, skyBoxWidth/2.0f,   1.0f, 0.0f,
		skyBoxLength/2.0f, 0.0f, -skyBoxWidth/2.0f,   0.0f, 0.0f,

		// Back (SkyBox2)
		skyBoxLength/2.0f, skyBoxLength, skyBoxWidth/2.0f,   0.0f, 1.0f,
		-skyBoxLength/2.0f, skyBoxLength, skyBoxWidth/2.0f,   1.0f, 1.0f,
		-skyBoxLength/2.0f, 0.0f, skyBoxWidth/2.0f,   1.0f, 0.0f,
		skyBoxLength/2.0f, 0.0f, skyBoxWidth/2.0f,   0.0f, 0.0f,

		// Left (SkyBox3)
		-skyBoxLength/2.0f, skyBoxLength, skyBoxWidth/2.0f,   0.0f, 1.0f,
		-skyBoxLength/2.0f, skyBoxLength, -skyBoxWidth/2.0f,   1.0f, 1.0f,
		-skyBoxLength/2.0f, 0.0f, -skyBoxWidth/2.0f,   1.0f, 0.0f,
		-skyBoxLength/2.0f, 0.0f, skyBoxWidth/2.0f,   0.0f, 0.0f,

		// Top (SkyBox4)
		-skyBoxLength/2.0f, skyBoxLength, skyBoxWidth/2.0f,   0.0f, 1.0f,
		skyBoxLength/2.0f, skyBoxLength, skyBoxWidth/2.0f,   1.0f, 1.0f,
		skyBoxLength/2.0f, skyBoxLength, -skyBoxWidth/2.0f,   1.0f, 0.0f, 
		-skyBoxLength/2.0f, skyBoxLength, -skyBoxWidth/2.0f,   0.0f, 0.0f,

		// Bottom (SkyBox5)
		-skyBoxLength/2.0f, 0.0f, -skyBoxWidth/2.0f,   0.0f, 28.0f,
		skyBoxLength/2.0f, 0.0f, -skyBoxWidth/2.0f,   20.0f, 28.0f,
		skyBoxLength/2.0f, 0.0f, skyBoxWidth/2.0f,   20.0f, 0.0f, 
		-skyBoxLength/2.0f, 0.0f, skyBoxWidth/2.0f,   0.0f, 0.0f
    };
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 2,
        2, 3, 0,
		4, 5, 6,
        6, 7, 4,
	    8, 9, 10,
        10, 11, 8,
		12, 13, 14,
        14, 15, 12,
		16, 17, 18,
        18, 19, 16,
		20, 21, 22,
        22, 23, 20
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


	// Load and create a texture
	// ====================
    // Skybox
    // ====================
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint textureSky[5];
	int width, height;
	unsigned char* image;
	for (int i = 0; i < 5; i++)
	{
		glGenTextures(1, &textureSky[i]);
		glBindTexture(GL_TEXTURE_2D, textureSky[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		switch ( i )
		{
			case 0:
				image = SOIL_load_image("SkyBox0.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
			case 1:
				image = SOIL_load_image("SkyBox1.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
			case 2:
				image = SOIL_load_image("SkyBox2.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
			case 3:
				image = SOIL_load_image("SkyBox3.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
			case 4:
				image = SOIL_load_image("SkyBox4.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
    }
	// ====================
    // Water
    // ====================
	GLuint textureWater;
	glGenTextures(1, &textureWater);
	glBindTexture(GL_TEXTURE_2D, textureWater);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("SkyBox5.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	// ====================
    // Terrain
    // ====================
	GLuint textureTerrain[2];
	for (int i = 0; i < 2; i++)
	{
		glGenTextures(1, & textureTerrain[i]);
		glBindTexture(GL_TEXTURE_2D,  textureTerrain[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		switch ( i )
		{
			case 0:
				image = SOIL_load_image("terrain-texture3.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
			case 1:
				image = SOIL_load_image("detail.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
				break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
    }
	// ====================
    // Heightmap
    // ====================
	unsigned char* heightmap = SOIL_load_image("heightmap.bmp", &width, &height, 0, SOIL_LOAD_L);


	// Set up vertex data (and buffer(s)) and attribute pointers
	// ====================
    // Terrain
    // ====================
	GLfloat terrainLength = 20.0f;
	GLfloat unitLength = terrainLength / width;
	GLfloat unitTexLength = 1.0f / width;
	GLfloat X0 = - terrainLength / 2.0f;
	GLfloat Z0 = - terrainLength / 2.0f;
	GLfloat Y0 = - 100.0f;
	GLfloat verticesTerrain[] = {
		-unitLength/2.0f, -unitLength/2.0f, 0.0f, unitTexLength, 0.0f,
		unitLength/2.0f, -unitLength/2.0f, unitTexLength, unitTexLength, 1.0f,
		unitLength/2.0f, unitLength/2.0f, unitTexLength, 0.0f, 2.0f,
		-unitLength/2.0f, unitLength/2.0f, 0.0f, 0.0f, 3.0f
	};
	GLuint indicesTerrain[] = {  // Note that we start from 0!
		0, 1, 2,
        2, 3, 0
    };
	GLuint VAOTerrain, VBOTerrain, EBOTerrain;
    glGenVertexArrays(1, &VAOTerrain);
    glGenBuffers(1, &VBOTerrain);
	glGenBuffers(1, &EBOTerrain);
    glBindVertexArray(VAOTerrain);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTerrain), verticesTerrain, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTerrain);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTerrain), indicesTerrain, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


	ourShaderTerrain.Use();
	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureTerrain[0]);
    glUniform1i(glGetUniformLocation(ourShaderTerrain.Program, "ourTexture1"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureTerrain[1]);
    glUniform1i(glGetUniformLocation(ourShaderTerrain.Program, "ourTexture2"), 2);
	glUniform1f(scaleLocTerrain, detailScale);


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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
		// Camera/View transformation
        view = camera.GetViewMatrix();
        // Projection
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 500.0f);

		ourShader.Use();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		ourShaderTerrain.Use();
		glUniformMatrix4fv(viewLocTerrain, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLocTerrain, 1, GL_FALSE, glm::value_ptr(projection));


		//Draw Skybox and Reflection
		ourShader.Use();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glActiveTexture(GL_TEXTURE0);

		glUniform1f(alphaLoc, 1.0f);
		glUniform2f(offsetLoc, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++)
        {
			glBindTexture(GL_TEXTURE_2D, textureSky[i]);

			glUniform1i(reflectLoc, 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(6 * i * sizeof(GLuint)));

			glUniform1i(reflectLoc, 1);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(6 * i * sizeof(GLuint)));
        }
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


		// Draw Terrain and Reflection
		ourShaderTerrain.Use();
		glBindVertexArray(VAOTerrain);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTerrain);

		for (int i = 0; i < height - 1; i++)
		{
			for (int j = 0; j < width - 1; j++)
			{
				GLfloat x = X0 + (j + 1) * unitLength;
				GLfloat z = Z0 + (i + 1) * unitLength;
				glUniform1f(xLocTerrain, x);
				glUniform1f(zLocTerrain, z);
				int tempIndex = i * width + j;
				glUniform4f(heightLocTerrain, ((GLfloat)heightmap[tempIndex] + Y0) * unitHeight, ((GLfloat)heightmap[tempIndex + 1] + Y0) * unitHeight , ((GLfloat)heightmap[tempIndex + width + 1] + Y0) * unitHeight, ((GLfloat)heightmap[tempIndex + width] + Y0) * unitHeight);
				glUniform2f(offsetLocTerrain, (j + 0.5) * unitTexLength, 1.0f - (i + 1.5) * unitTexLength);
				
				glUniform1i(reflectLocTerrain, 0);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				glUniform1i(reflectLocTerrain, 1);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		// Draw Water
		ourShader.Use();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWater);	

		glUniform1f(alphaLoc, waterAlpha);
		glUniform2f(offsetLoc, waveSpeedX * (GLfloat)glfwGetTime(), waveSpeedY * (GLfloat)glfwGetTime());
		glUniform1i(reflectLocTerrain, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(6 * 5 * sizeof(GLuint)));
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAOTerrain);
    glDeleteBuffers(1, &VBOTerrain);
	glDeleteBuffers(1, &EBOTerrain);
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
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_UP])
        camera.ProcessKeyboard(UP, deltaTime);
    if (keys[GLFW_KEY_DOWN])
        camera.ProcessKeyboard(DOWN, deltaTime);
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