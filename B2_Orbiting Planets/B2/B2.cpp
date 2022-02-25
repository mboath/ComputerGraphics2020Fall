#include <iostream>
#include <cmath>
#include <string>
#include <map>

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

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other includes
#include "shader.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void increase_speed();

// Window dimensions
GLuint WIDTH = 900, HEIGHT = 900;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAOCh, VBOCh;

// Parameters
GLfloat unitLength = 1000.0f;
const GLfloat near[5] = { 46.0f, 107.5f, 147.1f, 206.6, 740.5f };
const GLfloat far[5] = { 69.8f, 108.9f, 152.1f, 249.2, 816.6f };
GLfloat earthT = 10.0f;
const GLfloat T[5] = { 0.24f, 0.61f, 1.00f, 1.88f, 11.86f };
const GLfloat e[5] = { 0.205f, 0.007f, 0.017f, 0.094f, 0.049f };
const GLfloat omega_bar[5] = { 29.13f + 48.3f, 54.9f + 76.7f, 102.9f + 0.0f, 286.5f + 49.6f, 274.3f + 100.4f };
const GLfloat mercuryR = 8.0f;
const GLfloat size[5] = { 4879.0f, 12104.0f, 12756.0f, 6792.0f, 142984.0f / 1.0f };
GLfloat theta[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
std::string name[6] = { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Sun" };
const GLfloat deltaT = 0.01f;
const GLfloat deltaLength = 5.0f;

// Key_callback
bool keys[1024];

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
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "B2 yyf 2020215224", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
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
    Shader ourShaderOrbit("orbit.vert.glsl", "orbit.frag.glsl");
    Shader ourShaderCh("text.vert.glsl", "text.frag.glsl");
    

    // Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
        // Positions                          // Texture Coords
        // Sun and Planets
        -mercuryR, mercuryR, 0.0f,   0.0f, 1.0f, // Top Left
        mercuryR, mercuryR, 0.0f,   1.0f, 1.0f, // Top Right
        mercuryR, -mercuryR, 0.0f,   1.0f, 0.0f, // Bottom Right
        mercuryR, -mercuryR, 0.0f,   1.0f, 0.0f, // Bottom Right
        -mercuryR, -mercuryR, 0.0f,   0.0f, 0.0f,  // Bottom Left
		-mercuryR, mercuryR, 0.0f,   0.0f, 1.0f, // Top Left
        // Orbits
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Load and create textures
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint texture[6];
    int width, height;
    unsigned char* image;
    for (int i = 0; i < 6; i++)
    {
        glGenTextures(1, &texture[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        // Set our texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Load, create texture and generate mipmaps
        switch (i)
        {
            case 0:
                image = SOIL_load_image("Mercury.png", &width, &height, 0, SOIL_LOAD_RGBA);
                break;
            case 1:
                image = SOIL_load_image("Venus.png", &width, &height, 0, SOIL_LOAD_RGBA);
                break;
            case 2:
                image = SOIL_load_image("Earth.png", &width, &height, 0, SOIL_LOAD_RGBA);
                break;
            case 3:
                image = SOIL_load_image("Mars.png", &width, &height, 0, SOIL_LOAD_RGBA);
                break;
            case 4:
                image = SOIL_load_image("Jupiter.png", &width, &height, 0, SOIL_LOAD_RGBA);
                break;
            case 5:
                image = SOIL_load_image("Sun.png", &width, &height, 0, SOIL_LOAD_RGBA);
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    // ========================================
    // FreeType Begin
    // ========================================

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint textureCh;
        glGenTextures(1, &textureCh);
        glBindTexture(GL_TEXTURE_2D, textureCh);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            textureCh,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            GLuint(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAOCh);
    glGenBuffers(1, &VBOCh);
    glBindVertexArray(VAOCh);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCh);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ========================================
    // FreeType End
    // ========================================


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        increase_speed();

        // Render
        // Clear the colorbuffer
        glClearColor(31.0f / 255.0f, 28.0f / 255.0f, 99.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint modelLocOrbit = glGetUniformLocation(ourShaderOrbit.Program, "model");
        glm::mat4 model(1);

        // Draw orbits
        glBindVertexArray(VAO);
        ourShaderOrbit.Use();

        int pointNum = 3600;
        for (int i = 0; i < 5; i++)
        {
            GLfloat a = (near[i] + far[i]) / 2.0f;
            for (int j = 0; j < pointNum; j++)
            {
                GLfloat theta = 360.0f / (GLfloat)pointNum * (GLfloat)j;
                GLfloat r = (a * (1.0f - pow(e[i], 2.0f))) / (1.0f + e[i] * cos(glm::radians(theta)));
                GLfloat x = r * cos(glm::radians(theta + omega_bar[i]));
                GLfloat y = r * sin(glm::radians(theta + omega_bar[i]));
                model = glm::mat4(1);
                model = glm::translate(model, glm::vec3(x / unitLength, y / unitLength, 0.0f));
                glUniformMatrix4fv(modelLocOrbit, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_POINTS, 6, 1);
            }
        }

        glBindVertexArray(0);

        // Draw the sun
        glBindVertexArray(VAO);
        ourShader.Use();
        glBindTexture(GL_TEXTURE_2D, texture[5]);
        
        model = glm::mat4(1);
        model = glm::scale(model, glm::vec3(3.0f / unitLength, 3.0f / unitLength, 3.0f / unitLength));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        RenderText(ourShaderCh, name[5], - 3.0f * mercuryR / unitLength, 3.0f * mercuryR / unitLength, 0.5f / unitLength, glm::vec3(1.0f, 1.0f, 1.0f));

        // Draw planets
        for (int i = 0; i < 5; i++)
        {
            GLfloat omega = 360.0f / (T[i] * earthT);
            if (i != 1)
            {
                theta[i] += omega * deltaTime;
                if (theta[i] >= 360.0f)
                    theta[i] -= 360.0f;
            }
            else
            {
                theta[i] -= omega * deltaTime;
                if (theta[i] <= -360.0f)
                    theta[i] += 360.0f;
            }
            GLfloat a = (near[i] + far[i]) / 2.0f;
            GLfloat r = (a * (1.0f - pow(e[i], 2.0f))) / (1.0f + e[i] * cos(glm::radians(theta[i])));
            GLfloat x = r * cos(glm::radians(theta[i] + omega_bar[i]));
            GLfloat y = r * sin(glm::radians(theta[i] + omega_bar[i]));
            model = glm::mat4(1);
            model = glm::translate(model, glm::vec3(x / unitLength, y / unitLength, 0.0f));
            model = glm::scale(model, glm::vec3(size[i] / size[0] / unitLength, size[i] / size[0] / unitLength, size[i] / size[0] / unitLength));
            
            glBindVertexArray(VAO);
            ourShader.Use();
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

            RenderText(ourShaderCh, name[i], (x - size[i] / size[0] * mercuryR) / unitLength, (y + size[i] / size[0] * mercuryR) / unitLength, 0.5f / unitLength, glm::vec3(1.0f, 1.0f, 1.0f));
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


void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    glBindVertexArray(VAOCh);
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat verticesCh[6][4] = {
            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos,     ypos,       0.0, 0.0 },
            { xpos + w, ypos,       1.0, 0.0 },

            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos + w, ypos + h,   1.0, 1.0 }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBOCh);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesCh), verticesCh); // Be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

void increase_speed()
{
    std::cout << earthT << std::endl;
    if (keys[GLFW_KEY_RIGHT])
    {
        if (earthT - deltaT * earthT > 2.0f)
            earthT -= deltaT * earthT;
    }
    if (keys[GLFW_KEY_LEFT])
    {
        if(earthT + deltaT * earthT < 3600.0f)
            earthT += deltaT * earthT;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    unitLength -= yoffset * deltaLength;
}