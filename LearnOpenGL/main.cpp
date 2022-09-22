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

int g_CursorMode      = GLFW_CURSOR_DISABLED;
int g_DrawMode        = GL_FILL;
int g_EnableNPMapping = 1;

glm::mat4      g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 1000.0f);
Camera*        g_MainCamera;

ShaderProgram* g_BasicSP;
ShaderProgram* g_AdvancedLightingSP;

VertexArray*   g_PointVAO;
VertexBuffer*  g_PointVBO;

VertexArray*   g_QuadVAO;
VertexBuffer*  g_QuadVBO;

Texture*       g_BrickwallTex;
Texture*       g_BrickwallNMTex;
Texture*       g_BrickwallPMTex;

Texture*       g_WoodenSurfaceTex;
Texture*       g_WoodenSurfaceNMTex;
Texture*       g_WoodenSurfacePMTex;

void setup()
{
    float pointVertice[] = { 0.0f, 0.0f, 0.0f };

    // Positions.
    glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
    glm::vec3 pos4( 1.0f,  1.0f, 0.0f);

    // Texture coordinates,
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);

    // Normal vector.
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // Calculate tangent/bitangent vectors of both triangles.
    glm::vec3 tan1, bitan1;
    glm::vec3 tan2, bitan2;

    glm::vec3 edge1;
    glm::vec3 edge2;
    glm::vec2 deltaUV1;
    glm::vec2 deltaUV2;

    // Triangle 1.
    edge1 = pos2 - pos1;
    edge2 = pos3 - pos1;
    deltaUV1 = uv2 - uv1;
    deltaUV2 = uv3 - uv1;

    float f1 = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tan1.x = f1 * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tan1.y = f1 * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tan1.z = f1 * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitan1.x = f1 * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitan1.y = f1 * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitan1.z = f1 * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // Triangle 2.
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    float f2 = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tan2.x = f2 * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tan2.y = f2 * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tan2.z = f2 * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitan2.x = f2 * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitan2.y = f2 * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitan2.z = f2 * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    float quadVertices[] = {
        // Positions            // Normal         // Tex coords // Tangent              // Bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tan1.x, tan1.y, tan1.z, bitan1.x, bitan1.y, bitan1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tan1.x, tan1.y, tan1.z, bitan1.x, bitan1.y, bitan1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tan1.x, tan1.y, tan1.z, bitan1.x, bitan1.y, bitan1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tan2.x, tan2.y, tan2.z, bitan2.x, bitan2.y, bitan2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tan2.x, tan2.y, tan2.z, bitan2.x, bitan2.y, bitan2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tan2.x, tan2.y, tan2.z, bitan2.x, bitan2.y, bitan2.z
    };

    g_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
    g_BasicSP = new ShaderProgram("scripts/1_basic_vs.glsl", "scripts/1_basic_fs.glsl");
    g_AdvancedLightingSP = new ShaderProgram("scripts/16_advanced_bplm_vs.glsl", "scripts/16_advanced_bplm_fs.glsl");

    g_PointVAO = new VertexArray();
    g_PointVBO = new VertexBuffer(pointVertice, sizeof(pointVertice));

    g_PointVAO->bind();
    g_PointVBO->bind();

    g_PointVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

    g_PointVAO->unbind(); // Unbind VAO before another buffer.
    g_PointVBO->unbind();

    g_QuadVAO = new VertexArray();
    g_QuadVBO = new VertexBuffer(quadVertices, sizeof(quadVertices));

    g_QuadVAO->bind();
    g_QuadVBO->bind();

    g_QuadVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)( 0));
    g_QuadVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)( 3 * sizeof(float)));
    g_QuadVAO->setVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)( 6 * sizeof(float)));
    g_QuadVAO->setVertexAttribute(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)( 8 * sizeof(float)));
    g_QuadVAO->setVertexAttribute(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

    g_QuadVAO->unbind(); // Unbind VAO before another buffer.
    g_QuadVBO->unbind();

    g_BrickwallTex = new Texture("assets/textures/brickwall_2.jpg", true);
    g_BrickwallNMTex = new Texture("assets/textures/brickwall_2_normal_map.jpg");
    g_BrickwallPMTex = new Texture("assets/textures/brickwall_2_displacement_map.jpg");

    g_WoodenSurfaceTex = new Texture("assets/textures/wooden_surface.png", true);
    g_WoodenSurfaceNMTex = new Texture("assets/textures/toybox_normal_map.png");
    g_WoodenSurfacePMTex = new Texture("assets/textures/toybox_displacement_map.png");

    // Binding all textures at the end to prevent conflicts.

    g_BrickwallTex->bind(0);
    g_BrickwallNMTex->bind(1);
    g_BrickwallPMTex->bind(2);

    g_WoodenSurfaceTex->bind(3);
    g_WoodenSurfaceNMTex->bind(4);
    g_WoodenSurfacePMTex->bind(5);
}

/*
 * Drawing...
 *
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, 0.5f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the light object.
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, lightPosition);

        g_BasicSP->bind();
        g_PointVAO->bind();

        g_BasicSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
        g_BasicSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
        g_BasicSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

        g_BasicSP->setUniform1i("uEnablePointSize", 1);
        g_BasicSP->setUniform1f("uPointSize", 10.0f);

        g_BasicSP->setUniform3f("uColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glEnable(GL_PROGRAM_POINT_SIZE); // Enable point size customization.

        glDrawArrays(GL_POINTS, 0, 1);

        glDisable(GL_PROGRAM_POINT_SIZE);

        g_PointVAO->unbind();
        g_BasicSP->unbind();
    }

    // Render quads.
    {
        g_AdvancedLightingSP->bind();
        g_QuadVAO->bind();
        
        g_AdvancedLightingSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
        g_AdvancedLightingSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

        g_AdvancedLightingSP->setUniform3f("uLight.ambientComp", glm::vec3(0.1f, 0.1f, 0.1f));
        g_AdvancedLightingSP->setUniform3f("uLight.diffuseComp", glm::vec3(1.0f, 1.0f, 1.0f));
        g_AdvancedLightingSP->setUniform3f("uLight.specularComp", glm::vec3(0.2f, 0.2f, 0.2f));
        g_AdvancedLightingSP->setUniform3f("uLight.position", lightPosition);

        g_AdvancedLightingSP->setUniform3f("uViewPos", g_MainCamera->getPosition());
        g_AdvancedLightingSP->setUniform3f("uLightPos", lightPosition);
        g_AdvancedLightingSP->setUniform1f("uHeightScale", 0.1f);
        g_AdvancedLightingSP->setUniform1i("uEnableNPMapping", g_EnableNPMapping);

        glEnable(GL_FRAMEBUFFER_SRGB); // Enable gamma correction.

        // Quad 1.
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.05f, 0.0f, 0.0f));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(15.5f), glm::vec3(0.0f, 1.0f, 0.0f));

            g_AdvancedLightingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);

            g_AdvancedLightingSP->setUniform1i("uMaterial.diffuseMap", 0);
            g_AdvancedLightingSP->setUniform1i("uMaterial.specularMap", 0);
            g_AdvancedLightingSP->setUniform1i("uMaterial.normalMap", 1);
            g_AdvancedLightingSP->setUniform1i("uMaterial.parallaxMap", 2);
            g_AdvancedLightingSP->setUniform1f("uMaterial.shininess", 32.0f);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Quad 2.
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(1.05f, 0.0f, 0.0f));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(-15.5f), glm::vec3(0.0f, 1.0f, 0.0f));

            g_AdvancedLightingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);

            g_AdvancedLightingSP->setUniform1i("uMaterial.diffuseMap", 3);
            g_AdvancedLightingSP->setUniform1i("uMaterial.specularMap", 3);
            g_AdvancedLightingSP->setUniform1i("uMaterial.normalMap", 4);
            g_AdvancedLightingSP->setUniform1i("uMaterial.parallaxMap", 5);
            g_AdvancedLightingSP->setUniform1f("uMaterial.shininess", 32.0f);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glDisable(GL_FRAMEBUFFER_SRGB);

        g_QuadVAO->unbind();
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
            ImGui::Text("NP Mapping %s", g_EnableNPMapping == 1 ? "ENABLED" : "DISABLED");
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
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) // To enable/disable normal and parallax mapping.
    {
        g_EnableNPMapping = g_EnableNPMapping == 0 ? 1 : 0;
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