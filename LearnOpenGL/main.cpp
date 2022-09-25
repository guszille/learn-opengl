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
float g_HDRExposure = 1.0f;

int g_CursorMode       = GLFW_CURSOR_DISABLED;
int g_DrawMode         = GL_FILL;
int g_ActivateLighting = 1;

glm::mat4      g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 1000.0f);
Camera*        g_MainCamera;

ShaderProgram* g_BPLightingSP;
ShaderProgram* g_ScreenSP;

VertexArray*   g_QuadVAO;
VertexBuffer*  g_QuadVBO;

VertexArray*   g_CubeVAO;
VertexBuffer*  g_CubeVBO;

FrameBuffer*   g_ScreenFB;

Texture*       g_WoodenSurfaceTex;

void setup()
{
    float quadVertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
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
        
    g_BPLightingSP = new ShaderProgram("scripts/3_blinn_phong_lighting_model_vs.glsl", "scripts/4_bplm_fs_multiple_lc.glsl");
    g_ScreenSP = new ShaderProgram("scripts/5_screen_quad_vs.glsl", "scripts/5_screen_quad_fs.glsl");

    g_QuadVAO = new VertexArray();
    g_QuadVBO = new VertexBuffer(quadVertices, sizeof(quadVertices));

    g_QuadVAO->bind();
    g_QuadVBO->bind();

    g_QuadVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    g_QuadVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    g_QuadVAO->unbind(); // Unbind VAO before another buffer.
    g_QuadVBO->unbind();

    g_CubeVAO = new VertexArray();
    g_CubeVBO = new VertexBuffer(cubeVertices, sizeof(cubeVertices));

    g_CubeVAO->bind();
    g_CubeVBO->bind();

    g_CubeVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    g_CubeVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    g_CubeVAO->setVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    g_CubeVAO->unbind(); // Unbind VAO before another buffer.
    g_CubeVBO->unbind();

    g_ScreenFB = new FrameBuffer(g_WindowWidth, g_WindowHeight, 0, GL_RGBA16F);

    g_WoodenSurfaceTex = new Texture("assets/textures/wooden_surface.png", true);

    // Bind framebuffers and textures at the end to prevent conflicts.

    g_ScreenFB->bindColorBuffer(0);
    g_WoodenSurfaceTex->bind(1);
}

/*
 * Drawing...
 *
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    // Render scene into the custom framebuffer.
    {
        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;

        lightPositions.push_back(glm::vec3( 0.0f,  0.0f, -48.5f));
        lightPositions.push_back(glm::vec3(-1.4f, -1.8f,  -8.0f));
        lightPositions.push_back(glm::vec3( 0.0f, -1.8f,  -6.0f));
        lightPositions.push_back(glm::vec3( 1.4f, -1.8f,  -4.0f));

        lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
        lightColors.push_back(glm::vec3(  1.5f,   0.0f,   0.0f));
        lightColors.push_back(glm::vec3(  0.0f,   0.0f,   1.5f));
        lightColors.push_back(glm::vec3(  0.0f,   1.5f,   0.0f));

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -22.5f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f, 2.5f, -27.5f));

        g_ScreenFB->bind();
        g_BPLightingSP->bind();
        g_CubeVAO->bind();

        g_BPLightingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
        g_BPLightingSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
        g_BPLightingSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

        g_BPLightingSP->setUniform1i("uInverseNormals", 1);

        for (unsigned int i = 0; i < 4; i++)
        {
            g_BPLightingSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].position").c_str(), lightPositions[i]);
            g_BPLightingSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].ambient").c_str(), glm::vec3(0.0f, 0.0f, 0.0f));
            g_BPLightingSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].diffuse").c_str(), lightColors[i]);
            g_BPLightingSP->setUniform3f(("uPointLights[" + std::to_string(i) + "].specular").c_str(), glm::vec3(0.0f, 0.0f, 0.0f));
            g_BPLightingSP->setUniform1f(("uPointLights[" + std::to_string(i) + "].constant").c_str(), 1.0f);
            g_BPLightingSP->setUniform1f(("uPointLights[" + std::to_string(i) + "].linear").c_str(), 0.35f);
            g_BPLightingSP->setUniform1f(("uPointLights[" + std::to_string(i) + "].quadratic").c_str(), 0.44f);
        }

        g_BPLightingSP->setUniform1i("uMaterial.diffuse", 1);
        g_BPLightingSP->setUniform1i("uMaterial.specular", 1);
        g_BPLightingSP->setUniform1f("uMaterial.shininess", 32.0f);

        g_BPLightingSP->setUniform3f("uViewPos", g_MainCamera->getPosition());

        g_BPLightingSP->setUniform1i("uActivateDirectionalLight", 0);
        g_BPLightingSP->setUniform1i("uActivatePointLights", g_ActivateLighting);
        g_BPLightingSP->setUniform1i("uActivateSpotLight", 0);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        g_CubeVAO->unbind();
        g_BPLightingSP->unbind();
        g_ScreenFB->unbind();
    }

    // Render on screen framebuffer.
    {
        g_ScreenSP->bind();
        g_QuadVAO->bind();

        g_ScreenSP->setUniform1i("uScreenTexture", 0);
        g_ScreenSP->setUniform1i("uActiveEffect", 6); // HDR effect.
        g_ScreenSP->setUniform1f("uExposure", g_HDRExposure);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_FRAMEBUFFER_SRGB); // Enable gamma correction.

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisable(GL_FRAMEBUFFER_SRGB);

        g_ScreenSP->unbind();
        g_QuadVAO->unbind();
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
            ImGui::Text("Lights: %s", g_ActivateLighting == 1 ? "ENABLED" : "DISABLED");
            ImGui::Text("Exposure: %.1f", g_HDRExposure);
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
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) // To activate/deactivate lights.
    {
        g_ActivateLighting = g_ActivateLighting == 0 ? 1 : 0;
    }
    else if ((key == GLFW_KEY_N || key == GLFW_KEY_M) && action == GLFW_PRESS) // To increase/decrease camera speed.
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
    else if ((key == GLFW_KEY_Z || key == GLFW_KEY_X) && action == GLFW_PRESS) // To increase/decrease exposure.
    {
        if (key == GLFW_KEY_Z)
        {
            g_HDRExposure = std::min(g_HDRExposure + 0.1f, 5.0f);
        }
        else // Implies "key == GLFW_KEY_X".
        {
            g_HDRExposure = std::max(g_HDRExposure - 0.1f, 0.1f);
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