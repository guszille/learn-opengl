#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
#include "util/DepthMap.h"

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
float g_DeltaTime   = 0.0f;
float g_LastFrame   = 0.0f;
float g_CameraSpeed = 7.5f;

int g_CursorMode     = GLFW_CURSOR_DISABLED;
int g_DrawMode       = GL_FILL;
int g_ActivateLights = 0;

glm::mat4      g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 1000.0f);
Camera*        g_MainCamera;

ShaderProgram* g_ShadowMapSP;
DepthMap*      g_ShadowMapDM;
float          g_ShadowMapNear = 1.0f;
float          g_ShadowMapFar = 7.5f;
int            g_ShadowMapWidth = 2048;
int            g_ShadowMapHeight = 2048;

ShaderProgram* g_AdvancedLightingSP;

VertexArray*   g_PlaneVAO;
VertexBuffer*  g_PlaneVBO;

VertexArray*   g_CubeVAO;
VertexBuffer*  g_CubeVBO;

Texture*       g_WoodTex;

void setup()
{
    float planeVertices[] = {
        // positions            // normals         // texture coords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

    float cubeVertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right     
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right 
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left 
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right 
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
    };

    g_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    g_ShadowMapSP = new ShaderProgram("scripts/12_shadow_map_vs.glsl", "scripts/12_shadow_map_fs.glsl");
    g_ShadowMapDM = new DepthMap(g_ShadowMapWidth, g_ShadowMapHeight);
    
    g_AdvancedLightingSP = new ShaderProgram("scripts/13_advanced_lighting_vs.glsl", "scripts/13_advanced_lighting_fs.glsl");

    g_PlaneVAO = new VertexArray();
    g_PlaneVBO = new VertexBuffer(planeVertices, sizeof(planeVertices));

    g_PlaneVAO->bind();
    g_PlaneVBO->bind();

    g_PlaneVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    g_PlaneVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    g_PlaneVAO->setVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    g_PlaneVAO->unbind(); // Unbind VAO before another buffer.
    g_PlaneVBO->unbind();

    g_CubeVAO = new VertexArray();
    g_CubeVBO = new VertexBuffer(cubeVertices, sizeof(cubeVertices));

    g_CubeVAO->bind();
    g_CubeVBO->bind();

    g_CubeVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    g_CubeVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    g_CubeVAO->setVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    g_CubeVAO->unbind(); // Unbind VAO before another buffer.
    g_CubeVBO->unbind();

    g_WoodTex = new Texture("assets/textures/wood.png", true);

    // Binding textures at the end to prevent conflicts.
    g_ShadowMapDM->bindDepthBuffer(0);
    g_WoodTex->bind(1);
}

/*
 * Call this function inside the context of an active shader program. 
 */
void drawScene(ShaderProgram* shaderProgram, bool cullBackFaces = true)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // Draw floor.
    shaderProgram->setUniformMatrix4fv("uModelMatrix", modelMatrix);

    g_PlaneVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    g_PlaneVAO->unbind();

    // Draw cubes.
    g_CubeVAO->bind();

    glEnable(GL_CULL_FACE); // Enable face culling only for rendering closed shapes.
    // glCullFace(cullBackFaces ? GL_BACK : GL_FRONT);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.5f, 0.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
    shaderProgram->setUniformMatrix4fv("uModelMatrix", modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 1.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
    shaderProgram->setUniformMatrix4fv("uModelMatrix", modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.0f, 0.0f, 2.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25));
    shaderProgram->setUniformMatrix4fv("uModelMatrix", modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    g_CubeVAO->unbind();
}

/*
 * Drawing...
 *
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    float correntTime = glfwGetTime();
    float x = 2.0f * glm::cos(correntTime);
    float z = 2.0f * glm::sin(correntTime);

    glm::vec3 lightPosition = glm::vec3(x, 4.0f, z); // glm::vec3 lightPosition = glm::vec3(-2.0f, 4.0f, -1.0f);
    glm::mat4 lightProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, g_ShadowMapNear, g_ShadowMapFar);
    glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

    // Render the depth/shadow map.
    {
        glViewport(0, 0, g_ShadowMapWidth, g_ShadowMapHeight);

        g_ShadowMapSP->bind();
        g_ShadowMapDM->bind();

        g_ShadowMapSP->setUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);

        glClear(GL_DEPTH_BUFFER_BIT);
        drawScene(g_ShadowMapSP, false);

        g_ShadowMapDM->unbind();
        g_ShadowMapSP->unbind();
    }

    // Render the real scene.
    {
        glViewport(0, 0, g_WindowWidth, g_WindowHeight); // Reset viewport.

        g_AdvancedLightingSP->bind();

        g_AdvancedLightingSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
        g_AdvancedLightingSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);
        g_AdvancedLightingSP->setUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);

        g_AdvancedLightingSP->setUniform3f("uLight.ambient", glm::vec3(0.15f, 0.15f, 0.15f));
        g_AdvancedLightingSP->setUniform3f("uLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        g_AdvancedLightingSP->setUniform3f("uLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        g_AdvancedLightingSP->setUniform3f("uLight.position", lightPosition);

        g_AdvancedLightingSP->setUniform1i("uMaterial.diffuse", 1);
        g_AdvancedLightingSP->setUniform1i("uMaterial.specular", 1);
        g_AdvancedLightingSP->setUniform1f("uMaterial.shininess", 64.0f);

        g_AdvancedLightingSP->setUniform1i("uShadowMap", 0);
        g_AdvancedLightingSP->setUniform3f("uViewPos", g_MainCamera->getPosition());

        glEnable(GL_FRAMEBUFFER_SRGB); // Enable gamma correction.

        glClearColor(0.15f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(g_AdvancedLightingSP);

        glDisable(GL_FRAMEBUFFER_SRGB);

        g_AdvancedLightingSP->unbind();
    }

    // Draw ImGui interface/frame.
    {
        // Start ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("General");
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

    /* Define a multisample buffer to prevent anti-aliasing (MSAA) */
    glfwWindowHint(GLFW_SAMPLES, 4);

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
    glfwSwapInterval(0); // Removes FPS limit.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD." << std::endl;

        return -1;
    }

    /* Setup ImGui */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls.
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable gamepad controls.

    ImGui::StyleColorsDark();

    // Setup platform/renderer backends.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    /* Configure OpenGL context */
    glViewport(0, 0, g_WindowWidth, g_WindowHeight);

    /* Enable OpenGL features */
    glEnable(GL_DEPTH_TEST);       // Enable depth test.
    glEnable(GL_BLEND);            // Enable blending.
    glEnable(GL_MULTISAMPLE);      // Enable multisampling.
    // (...)                       // Enable gamma correction only for the last vertex shader.
    // (...)                       // Enable face culling only for rendering closed shapes.

    // WARNING:
    //
    // After enabling GL_FRAMEBUFFER_SRGB, OpenGL automatically performs gamma correction
    // after each fragment shader run to all subsequent framebuffers, including the default framebuffer.

    /* Configure depth buffer */
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); // Allows to enable/disable writing to the depth buffer.

    /* Configure blending */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Allows us to set the source and destination factor values of the blend equation.
                                                       // We can also do more stuff with "glBlendColor", "glBlendFuncSeparate" and "glBlendEquation" functions. 

    /* Configure face culling */
    glFrontFace(GL_CCW); // Consider counter-clockwise faces as front faces.
    glCullFace(GL_BACK);

    /* Define window callbacks */
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);

    setup();

    io.WantCaptureMouse = true; // FIXME: Forcing flag value to try fixing focus ImGui/GLFW problem.

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

    /* Cleanup */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // Window should close.
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) // To attach/detach cursor on screen.
    {
        g_CursorMode = g_CursorMode == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;

        glfwSetInputMode(window, GLFW_CURSOR, g_CursorMode);

        if (g_CursorMode == GLFW_CURSOR_NORMAL)
        {
            g_CursorAttached = false;

            glfwSetCursorPosCallback(window, NULL);
        }
        else
        {
            glfwSetCursorPosCallback(window, cursorPositionCallback);
        }
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS) // To enable/disable line draw mode.
    {
        g_DrawMode = g_DrawMode == GL_FILL ? GL_LINE : GL_FILL;

        glPolygonMode(GL_FRONT_AND_BACK, g_DrawMode);
    }
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) // To enable/disable lights.
    {
        g_ActivateLights = g_ActivateLights == 0 ? 1 : 0;
    }
    else if ((key == GLFW_KEY_N || key == GLFW_KEY_M ) && action == GLFW_PRESS) // To increase/decrease camera speed.
    {
        if (key == GLFW_KEY_N)
        {
            g_CameraSpeed = std::min(g_CameraSpeed + 2.5f, 25.0f);
        }
        else // Implies "key == GLFW_KEY_M".
        {
            g_CameraSpeed = std::max(g_CameraSpeed - 2.5f, 2.5f);
        }
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
    const float speed = g_CameraSpeed * g_DeltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(speed, Camera::Direction::FORWARD);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(speed, Camera::Direction::BACKWARD);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(speed, Camera::Direction::RIGHT);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_MainCamera->setPosition(speed, Camera::Direction::LEFT);
    }
}