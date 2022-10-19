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
#include "util/TextRenderer.h"

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

int g_CursorMode       = GLFW_CURSOR_DISABLED;
int g_DrawMode         = GL_FILL;
int g_ActivateLighting = 1;

// DEBUG variables.
bool g_DebugMode = false;

// Feature variables.
glm::mat4      g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 100.0f);
glm::mat4      g_UIProjectionMatrix = glm::ortho(0.0f, (float)g_WindowWidth, 0.0f, (float)g_WindowHeight);
Camera*        g_MainCamera;

ShaderProgram* g_DeferredGPassSP;
ShaderProgram* g_DeferredLPassSP;
ShaderProgram* g_ForwardRenderingSP;

ShaderProgram* g_RenderQuadSP;

VertexArray*   g_QuadVAO;
VertexBuffer*  g_QuadVBO;

VertexArray*   g_CubeVAO;
VertexBuffer*  g_CubeVBO;

FrameBuffer*   g_GBufferFB;

Texture*       g_ContainerTex;
Texture*       g_ContainerSpecMap;

std::vector<glm::vec3> g_ObjectPositions;
std::vector<glm::vec3> g_LightPositions;
std::vector<glm::vec3> g_LightColors;

const int g_NumberOfLights = 32;

TextRenderer*  g_TextRenderer;
ShaderProgram* g_TextRendererSP;

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
        
    g_DeferredGPassSP = new ShaderProgram("scripts/17_ds_geometry_pass_vs.glsl", "scripts/17_ds_geometry_pass_fs.glsl");
    g_DeferredLPassSP = new ShaderProgram("scripts/17_ds_lighting_pass_vs.glsl", "scripts/17_ds_lighting_pass_fs.glsl");
    g_ForwardRenderingSP = new ShaderProgram("scripts/17_forward_rendering_vs.glsl", "scripts/17_forward_rendering_fs.glsl");

    g_RenderQuadSP = new ShaderProgram("scripts/5_screen_quad_vs.glsl", "scripts/5_screen_quad_fs.glsl");

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

    g_GBufferFB = new FrameBuffer(g_WindowWidth, g_WindowHeight, 3, GL_RGBA16F);

    g_ContainerTex = new Texture("assets/textures/container.png", true);
    g_ContainerSpecMap = new Texture("assets/textures/container_specular_map.png");

    // Bind framebuffers and textures at the end to prevent conflicts.

    g_GBufferFB->bindColorBuffer(0, 0);
    g_GBufferFB->bindColorBuffer(1, 1);
    g_GBufferFB->bindColorBuffer(2, 2);

    g_ContainerTex->bind(3);
    g_ContainerSpecMap->bind(4);

    srand(13);

    g_ObjectPositions.push_back(glm::vec3(-3.0f, -0.5f, -3.0f));
    g_ObjectPositions.push_back(glm::vec3( 0.0f, -0.5f, -3.0f));
    g_ObjectPositions.push_back(glm::vec3( 3.0f, -0.5f, -3.0f));
    g_ObjectPositions.push_back(glm::vec3(-3.0f, -0.5f,  0.0f));
    g_ObjectPositions.push_back(glm::vec3( 0.0f, -0.5f,  0.0f));
    g_ObjectPositions.push_back(glm::vec3( 3.0f, -0.5f,  0.0f));
    g_ObjectPositions.push_back(glm::vec3(-3.0f, -0.5f,  3.0f));
    g_ObjectPositions.push_back(glm::vec3( 0.0f, -0.5f,  3.0f));
    g_ObjectPositions.push_back(glm::vec3( 3.0f, -0.5f,  3.0f));

    for (unsigned int i = 0; i < g_NumberOfLights; i++)
    {
        float xPos = static_cast<float>(((rand() % 100) / 100.0f) * 6.0f - 3.0f);
        float yPos = static_cast<float>(((rand() % 100) / 100.0f) * 6.0f - 4.0f);
        float zPos = static_cast<float>(((rand() % 100) / 100.0f) * 6.0f - 3.0f);

        g_LightPositions.push_back(glm::vec3(xPos, yPos, zPos));

        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f); // between 0.5 and 1.0
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f); // between 0.5 and 1.0
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f); // between 0.5 and 1.0

        g_LightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }

    g_TextRenderer = new TextRenderer("assets/fonts/Roboto-Regular.ttf");

    g_TextRendererSP = new ShaderProgram("scripts/18_text_rendering_vs.glsl", "scripts/18_text_rendering_fs.glsl");

    g_TextRendererSP->bind();
    g_TextRendererSP->setUniformMatrix4fv("uProjectionMatrix", g_UIProjectionMatrix);
    g_TextRendererSP->unbind();
}

/*
 * Drawing...
 *
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 1. Geometry pass (DS): Render scene's geometry/color data into gBuffer.
    {
        g_GBufferFB->bind();
        g_DeferredGPassSP->bind();
        g_CubeVAO->bind();

        g_DeferredGPassSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
        g_DeferredGPassSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

        g_DeferredGPassSP->setUniform1i("uDiffuseMap", 3);
        g_DeferredGPassSP->setUniform1i("uSpecularMap", 4);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (unsigned int i = 0; i < g_ObjectPositions.size(); i++)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, g_ObjectPositions[i]);

            g_DeferredGPassSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        g_CubeVAO->unbind();
        g_DeferredGPassSP->unbind();
        g_GBufferFB->unbind();
    }

    // DEBUG.
    if (g_DebugMode)
    {
        g_RenderQuadSP->bind();
        g_QuadVAO->bind();

        g_RenderQuadSP->setUniform1i("uScreenTexture", 0); // Rendering position as texture.

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        g_QuadVAO->unbind();
        g_RenderQuadSP->unbind();
    
        return;
    }

    // 2. Lighting pass (DS): Calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    {
        g_DeferredLPassSP->bind();
        g_QuadVAO->bind();

        g_DeferredLPassSP->setUniform1i("gPosition", 0);
        g_DeferredLPassSP->setUniform1i("gNormal", 1);
        g_DeferredLPassSP->setUniform1i("gAlbedoAndSpecular", 2);

        for (unsigned int i = 0; i < g_NumberOfLights; i++)
        {
            float constant  = 1.0f;
            float linear    = 0.7f;
            float quadratic = 1.8f;
            float maximum   = std::fmaxf(std::fmaxf(g_LightColors[i].r, g_LightColors[i].g), g_LightColors[i].b);
            float radius    = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * maximum))) / (2 * quadratic);

            g_DeferredLPassSP->setUniform3f(("uLights[" + std::to_string(i) + "].position").c_str(), g_LightPositions[i]);
            g_DeferredLPassSP->setUniform3f(("uLights[" + std::to_string(i) + "].color").c_str(), g_LightColors[i]);
            g_DeferredLPassSP->setUniform1f(("uLights[" + std::to_string(i) + "].constant").c_str(), constant);
            g_DeferredLPassSP->setUniform1f(("uLights[" + std::to_string(i) + "].linear").c_str(), linear);
            g_DeferredLPassSP->setUniform1f(("uLights[" + std::to_string(i) + "].quadratic").c_str(), quadratic);
            g_DeferredLPassSP->setUniform1f(("uLights[" + std::to_string(i) + "].radius").c_str(), radius);
        }

        g_DeferredLPassSP->setUniform1i("uActivateLighting", g_ActivateLighting);
        g_DeferredLPassSP->setUniform3f("uViewPos", g_MainCamera->getPosition());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_FRAMEBUFFER_SRGB); // Enable gamma correction.
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisable(GL_FRAMEBUFFER_SRGB);

        g_QuadVAO->unbind();
        g_DeferredLPassSP->unbind();
    }

    // 3. Copy content of geometry's depth buffer to default framebuffer's depth buffer.
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_GBufferFB->getID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, g_WindowWidth, g_WindowHeight, 0, 0, g_WindowWidth, g_WindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // 4. Forward rendering: Render the lights on top of the scene.
    {
        g_ForwardRenderingSP->bind();
        g_CubeVAO->bind();

        g_ForwardRenderingSP->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
        g_ForwardRenderingSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

        for (unsigned int i = 0; i < g_NumberOfLights; i++)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, g_LightPositions[i]);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.125f));

            g_ForwardRenderingSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
            g_ForwardRenderingSP->setUniform3f("uLightColor", g_LightColors[i]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        g_CubeVAO->unbind();
        g_ForwardRenderingSP->unbind();
    }

    // Text rendering.
    {
        g_TextRenderer->write(*g_TextRendererSP, "(C) LearnOpenGL.com", 980.0f, 680.0f, 0.5f, glm::vec3(0.3, 0.75f, 0.8f));
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
    // glEnable(GL_BLEND);         // Enable blending.
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