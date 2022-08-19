#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/VertexArray.h"
#include "core/VertexBuffer.h"
#include "core/ElementBuffer.h"
#include "core/ShaderProgram.h"
#include "core/FrameBuffer.h"

#include "util/Texture.h"
#include "util/Camera.h"

// Global window properties.
int   g_WindowWidth       = 1280;
int   g_WindowHeight      = 720;
bool  g_CursorAttached    = false;
float g_WindowAspectRatio = (float)g_WindowWidth / (float)g_WindowHeight;
float g_CursorPosX        = (float)g_WindowWidth / 2.0f;
float g_CursorPosY        = (float)g_WindowHeight / 2.0f;
float g_FieldOfView       = 45.0f;

// GLFW window callbacks.
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Global variables.
float g_DeltaTime = 0.0f;
float g_LastFrame = 0.0f;

int g_CursorMode  = GLFW_CURSOR_DISABLED;
int g_DrawMode    = GL_FILL;

glm::mat4 g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 100.0f);

ShaderProgram* g_TexturingSP;
ShaderProgram* g_ScreenSP;

FrameBuffer*   g_BasicFB;

Camera*        g_MainCamera;

VertexArray*   g_CubeVAO;
VertexBuffer*  g_CubeVBO;
ElementBuffer* g_CubeEBO;

VertexArray*   g_PlaneVAO;
VertexBuffer*  g_PlaneVBO;

VertexArray*   g_QuadVAO;
VertexBuffer*  g_QuadVBO;

Texture*       g_MetalTex;
Texture*       g_BoxTex;

void setup()
{
    // Specified in a counter-clockwise order...
    //
    float cubeVertices[] = {
        // positions          // texture coords

        // back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
    };

    unsigned int cubeIndices[] = {
         0,  1,  2,  3,  4,  5,
         6,  7,  8,  9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35
    };

    // Note we set the texture coords higher than 1 (together with GL_REPEAT as texture wrapping mode).
    // This will cause the plane texture to repeat.
    //
    float planeVertices[] = {
        // positions          // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f
    };

    // Vertex attributes for a quad that fills the entire screen in "Normalized Device Coordinates".
    //
    float quadVertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    g_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    g_TexturingSP = new ShaderProgram("scripts/2_simple_texturing_vs.glsl", "scripts/2_simple_texturing_fs.glsl");
    g_ScreenSP = new ShaderProgram("scripts/5_screen_quad_vs.glsl", "scripts/5_screen_quad_fs.glsl");

    g_BasicFB = new FrameBuffer(g_WindowWidth, g_WindowHeight);

    g_MetalTex = new Texture("assets/textures/metal.png");
    g_BoxTex = new Texture("assets/textures/box.jpg");

    g_BasicFB->bindColorBuffer(0);

    g_MetalTex->bind(1);
    g_BoxTex->bind(2);

    g_CubeVAO = new VertexArray();
    g_CubeVBO = new VertexBuffer(cubeVertices, sizeof(cubeVertices));
    g_CubeEBO = new ElementBuffer(cubeIndices, sizeof(cubeIndices));

    g_CubeVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    g_CubeVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    g_CubeVAO->unbind(); // Unbind VAO before another buffer.
    g_CubeVBO->unbind();
    g_CubeEBO->unbind();

    g_PlaneVAO = new VertexArray();
    g_PlaneVBO = new VertexBuffer(planeVertices, sizeof(planeVertices));

    g_PlaneVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    g_PlaneVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    g_PlaneVAO->unbind(); // Unbind VAO before another buffer.
    g_PlaneVBO->unbind();

    g_QuadVAO = new VertexArray();
    g_QuadVBO = new VertexBuffer(quadVertices, sizeof(quadVertices));

    g_QuadVAO->setVertexAttribute(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
    g_QuadVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    g_QuadVAO->unbind(); // Unbind VAO before another buffer.
    g_QuadVBO->unbind();
}

/*
 * Drawing...
 *  
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    g_BasicFB->bind();

    glClearColor(0.25f, 0.55f, 0.85f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw objects.
    {
        // Ground.
        {          
            g_TexturingSP->bind();
            g_PlaneVAO->bind();

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(7.5f, 7.5f, 0.0f));

            g_TexturingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
            g_TexturingSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
            g_TexturingSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);
            g_TexturingSP->setUniform1i("uTexture", 1);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            g_PlaneVAO->unbind();
            g_TexturingSP->unbind();
        }

        // Cube.
        {
            glEnable(GL_CULL_FACE); // Only enables face culling for rendering closed shapes.

            g_TexturingSP->bind();
            g_CubeVAO->bind();

            g_TexturingSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
            g_TexturingSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);
            g_TexturingSP->setUniform1i("uTexture", 2);

            // Cube #1.
            {
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, glm::vec3( 2.0f, 0.0f, -0.5f));

                g_TexturingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
                
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }

            // Cube #2.
            {
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0.0f, -1.5f));

                g_TexturingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);

                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }

            g_CubeVAO->unbind();
            g_TexturingSP->unbind();

            glDisable(GL_CULL_FACE); 
        }
    }

    g_BasicFB->unbind();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw screen quad.
    {
        glDisable(GL_DEPTH_TEST);

        g_ScreenSP->bind();
        g_QuadVAO->bind();

        g_ScreenSP->setUniform1i("uScreenTexture", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        g_QuadVAO->unbind();
        g_ScreenSP->unbind();

        glEnable(GL_DEPTH_TEST);
    }
}

int main()
{
    GLFWwindow* window;

    /* Initialize GLFW */
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    /* Define OpenGL version and profile model */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Configure OpenGL context */
    glViewport(0, 0, g_WindowWidth, g_WindowHeight);

    /* Enable OpenGL features */
    glEnable(GL_DEPTH_TEST);   // Enable depth test.
    glEnable(GL_BLEND);        // Enable blending.
    // Face culling will be enabled only for rendering closed shapes.

    /* Configure depth buffer */
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); // Allows to enable/disable writing to the depth buffer.

    /* Configure blending */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Allows us to set the source and destination factor values of the blend equation.
    // We can also do more stuff with "glBlendColor", "glBlendFuncSeparate" and "glBlendEquation" functions. 

    /* Configure face culling */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); // Consider counter-clockwise faces as front faces.

    /* Define window callbacks */
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    setup();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();

        g_DeltaTime = currentFrame - g_LastFrame;
        g_LastFrame = currentFrame;

        /* Render here */
        render();

        /* Process some keyboard/mouse inputs */
        processInput(window);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    g_WindowWidth = width;
    g_WindowHeight = height;
    g_WindowAspectRatio = (float)width / (float)height;

    glViewport(0, 0, width, height);

    g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 100.0f);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
    {
        g_CursorMode = g_CursorMode == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;

        glfwSetInputMode(window, GLFW_CURSOR, g_CursorMode);
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        g_DrawMode = g_DrawMode == GL_FILL ? GL_LINE : GL_FILL;

        glPolygonMode(GL_FRONT_AND_BACK, g_DrawMode);
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    const float cameraSensitivity = 0.05f;

    if (!g_CursorAttached)
    {
        g_CursorPosX = xpos;
        g_CursorPosY = ypos;

        g_CursorAttached = true;
    }

    float xOffset = xpos - g_CursorPosX;
    float yOffset = g_CursorPosY - ypos;

    g_CursorPosX = xpos;
    g_CursorPosY = ypos;

    xOffset *= cameraSensitivity;
    yOffset *= cameraSensitivity;

    g_MainCamera->setDirection(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_FieldOfView -= (float)yoffset;
    g_FieldOfView = std::min(std::max(g_FieldOfView, 1.0f), 45.0f);

    g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 100.0f);
}

void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * g_DeltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(cameraSpeed, Camera::Direction::FORWARD);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(cameraSpeed, Camera::Direction::BACKWARD);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(cameraSpeed, Camera::Direction::RIGHT);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(cameraSpeed, Camera::Direction::LEFT);
    }
}