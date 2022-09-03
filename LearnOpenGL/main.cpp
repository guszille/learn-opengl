#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>

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
#include "core/UniformBuffer.h"

#include "util/Camera.h"
#include "util/Texture.h"
#include "util/CubeMap.h"

#include "util/object/Model.h"

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

int g_CursorMode     = GLFW_CURSOR_DISABLED;
int g_DrawMode       = GL_FILL;
int g_ActivateLights = 0;

glm::mat4 g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 100.0f);

ShaderProgram* g_PhongLightingModelSP;

Camera*        g_MainCamera;

Model*         g_BackpackModel;

void setup()
{
    g_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    g_PhongLightingModelSP = new ShaderProgram("scripts/3_phong_lighting_model_vs.glsl", "scripts/10_model_loading_fs.glsl");

    g_PhongLightingModelSP->bind();
    g_PhongLightingModelSP->setUniform1i("uMaterial.diffuseMaps[0]", 0);
    g_PhongLightingModelSP->setUniform1i("uMaterial.specularMaps[0]", 1);
    g_PhongLightingModelSP->unbind();

    g_BackpackModel = new Model("assets/objects/backpack/backpack.obj");
}

/*
 * Drawing...
 *
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    glClearColor(0.33f, 0.66f, 0.99f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,   2.0f),
        glm::vec3( 2.3f, -3.3f, - 4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, - 3.0f)
    };

    // Draw objects.
    {
        // Cube.
        {
            g_PhongLightingModelSP->bind();

            glm::mat4 modelMatrix = glm::mat4(1.0f);

            // Defining general uniforms.
            g_PhongLightingModelSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
            g_PhongLightingModelSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
            g_PhongLightingModelSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

            g_PhongLightingModelSP->setUniform1i("uActivateLights", g_ActivateLights);

            // Defining directional light uniorms.
            g_PhongLightingModelSP->setUniform3f("uDirectionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
            g_PhongLightingModelSP->setUniform3f("uDirectionalLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            g_PhongLightingModelSP->setUniform3f("uDirectionalLight.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
            g_PhongLightingModelSP->setUniform3f("uDirectionalLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

            // Defining point lights uniforms.
            for (unsigned int i = 0; i < 4; i++)
            {
                g_PhongLightingModelSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].position").c_str(), pointLightPositions[i]);
                g_PhongLightingModelSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].ambient").c_str(), glm::vec3(0.05f, 0.05f, 0.05f));
                g_PhongLightingModelSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].diffuse").c_str(), glm::vec3(0.8f, 0.8f, 0.8f));
                g_PhongLightingModelSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].specular").c_str(), glm::vec3(1.0f, 1.0f, 1.0f));
                g_PhongLightingModelSP->setUniform1f(("uPointLights[" + std::to_string(i) + "].constant").c_str(), 1.0f);
                g_PhongLightingModelSP->setUniform1f(("uPointLights[" + std::to_string(i) + "].linear").c_str(), 0.09f);
                g_PhongLightingModelSP->setUniform1f(("uPointLights[" + std::to_string(i) + "].quadratic").c_str(), 0.032f);
            }

            // Defining spot light uniforms.
            g_PhongLightingModelSP->setUniform3f("uSpotLight.position", g_MainCamera->getPosition());
            g_PhongLightingModelSP->setUniform3f("uSpotLight.direction", g_MainCamera->getDirection());
            g_PhongLightingModelSP->setUniform3f("uSpotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            g_PhongLightingModelSP->setUniform3f("uSpotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
            g_PhongLightingModelSP->setUniform3f("uSpotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            g_PhongLightingModelSP->setUniform1f("uSpotLight.constant", 1.0f);
            g_PhongLightingModelSP->setUniform1f("uSpotLight.linear", 0.045f);
            g_PhongLightingModelSP->setUniform1f("uSpotLight.quadratic", 0.0075f);
            g_PhongLightingModelSP->setUniform1f("uSpotLight.cutOff", glm::cos(glm::radians(12.5f)));
            g_PhongLightingModelSP->setUniform1f("uSpotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

            // The others material components was defined in setup stage.
            g_PhongLightingModelSP->setUniform1f("uMaterial.shininess", 64.0f);

            g_BackpackModel->draw(g_PhongLightingModelSP);

            g_PhongLightingModelSP->unbind();
        }
    }
}

int main()
{
    GLFWwindow* window;

    /* Initialize GLFW */
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;

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
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();

        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD." << std::endl;

        return -1;
    }

    /* Configure OpenGL context */
    glViewport(0, 0, g_WindowWidth, g_WindowHeight);

    /* Enable OpenGL features */
    glEnable(GL_DEPTH_TEST);   // Enable depth test.
    glEnable(GL_BLEND);        // Enable blending.
    // glEnable(GL_CULL_FACE); // Enable face culling only for rendering closed shapes.

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
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        g_ActivateLights = g_ActivateLights == 0 ? 1 : 0;
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
    g_FieldOfView = g_FieldOfView - (float)yoffset;
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