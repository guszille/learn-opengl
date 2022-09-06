#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
float g_DeltaTime   = 0.0f;
float g_LastFrame   = 0.0f;
float g_CameraSpeed = 7.5f;

int g_CursorMode     = GLFW_CURSOR_DISABLED;
int g_DrawMode       = GL_FILL;
int g_ActivateLights = 0;

glm::mat4  g_ProjectionMatrix = glm::perspective(glm::radians(g_FieldOfView), g_WindowAspectRatio, 0.1f, 1000.0f);
glm::mat4* g_ModelMatrices;

ShaderProgram* g_PhongLightingModelSP;
ShaderProgram* g_InstancingModelsSP;
ShaderProgram* g_CubeMapSP;

Camera*        g_MainCamera;

Model*         g_PlanetModel;
Model*         g_RockModel;

VertexBuffer*  g_ModelMatrixVBO;

CubeMap*       g_SkyboxCM;

VertexArray*   g_SkyboxVAO;
VertexBuffer*  g_SkyboxVBO;

const int      g_AmountOfRocks = 100000;

void genModelMatrices(unsigned int amount)
{
    g_ModelMatrices = new glm::mat4[amount];

    srand((unsigned int)glfwGetTime());

    float radius = 150.0f;
    float offset = 25.0f;

    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        // 1. Translation: displace along circle with 'radius' in range [-offset, offset].
        float angle = (float)i / (float)amount * 360.0f;
        float d1 = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float d2 = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float d3 = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

        float x = sin(angle) * radius + d1;
        float y = d2 * 0.4f; // Keep height of asteroid field smaller compared to width of x and z.
        float z = cos(angle) * radius + d3;

        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. Scale: between 0.05 and 0.25f.
        float scale = (float)((rand() % 20) / 100.0f + 0.05f);

        model = glm::scale(model, glm::vec3(scale));

        // 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector.
        float rotationAngle = (float)((rand() % 360));

        model = glm::rotate(model, rotationAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. Now add to list of matrices.
        g_ModelMatrices[i] = model;
    }
}

void setup()
{
    std::size_t vec4Size = sizeof(glm::vec4);

    float skyboxVertices[] = {
        // positions and texture coords         
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    std::array<const char*, 6> cubeMapFaces = {"right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png"};

    g_MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    g_PhongLightingModelSP = new ShaderProgram("scripts/3_phong_lighting_model_vs.glsl", "scripts/10_model_loading_fs.glsl");
    g_InstancingModelsSP = new ShaderProgram("scripts/11_instancing_models_vs.glsl", "scripts/10_model_loading_fs.glsl");
    g_CubeMapSP = new ShaderProgram("scripts/6_cube_map_vs.glsl", "scripts/6_cube_map_fs.glsl");

    g_PlanetModel = new Model("assets/objects/planet/planet.obj");
    g_RockModel = new Model("assets/objects/rock/rock.obj");

    genModelMatrices(g_AmountOfRocks);

    g_ModelMatrixVBO = new VertexBuffer(&g_ModelMatrices[0], g_AmountOfRocks * sizeof(glm::mat4));

    g_ModelMatrixVBO->bind();

    for (const Mesh& mesh : g_RockModel->getMeshes())
    {
        glBindVertexArray(mesh.getVAO());

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(0));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    g_ModelMatrixVBO->unbind();

    g_SkyboxCM = new CubeMap("assets/textures/skybox_space", cubeMapFaces);

    g_SkyboxCM->bind(8); // Setting unit value to prevent conflict with models textures.

    g_SkyboxVAO = new VertexArray();
    g_SkyboxVBO = new VertexBuffer(skyboxVertices, sizeof(skyboxVertices));

    g_SkyboxVAO->bind();
    g_SkyboxVBO->bind();
    g_SkyboxVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
    g_SkyboxVAO->unbind(); // Unbind VAO before another buffer.
    g_SkyboxVBO->unbind();
}

void configureShader(ShaderProgram* shaderProgram)
{
    glm::vec3 pointLightPositions[] = {
        glm::vec3(-10.0f, 0.0f, -10.0f),
        glm::vec3(-10.0f, 0.0f,  10.0f),
        glm::vec3( 10.0f, 0.0f, -10.0f),
        glm::vec3( 10.0f, 0.0f,  10.0f)
    };

    shaderProgram->bind();

    // Defining general uniforms.
    shaderProgram->setUniformMatrix4fv("uViewMatrix", g_MainCamera->getViewMatrix());
    shaderProgram->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);

    shaderProgram->setUniform1i("uActivateLights", g_ActivateLights);

    // Defining directional light uniorms.
    shaderProgram->setUniform3f("uDirectionalLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shaderProgram->setUniform3f("uDirectionalLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shaderProgram->setUniform3f("uDirectionalLight.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
    shaderProgram->setUniform3f("uDirectionalLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    // Defining point lights uniforms.
    for (unsigned int i = 0; i < 4; i++)
    {
        shaderProgram->setUniform3f(("uPointLights[" + std::to_string(i) + "].position").c_str(), pointLightPositions[i]);
        shaderProgram->setUniform3f(("uPointLights[" + std::to_string(i) + "].ambient").c_str(), glm::vec3(0.05f, 0.05f, 0.05f));
        shaderProgram->setUniform3f(("uPointLights[" + std::to_string(i) + "].diffuse").c_str(), glm::vec3(0.8f, 0.8f, 0.8f));
        shaderProgram->setUniform3f(("uPointLights[" + std::to_string(i) + "].specular").c_str(), glm::vec3(1.0f, 1.0f, 1.0f));
        shaderProgram->setUniform1f(("uPointLights[" + std::to_string(i) + "].constant").c_str(), 1.0f);
        shaderProgram->setUniform1f(("uPointLights[" + std::to_string(i) + "].linear").c_str(), 0.09f);
        shaderProgram->setUniform1f(("uPointLights[" + std::to_string(i) + "].quadratic").c_str(), 0.032f);
    }

    // Defining spot light uniforms.
    shaderProgram->setUniform3f("uSpotLight.position", g_MainCamera->getPosition());
    shaderProgram->setUniform3f("uSpotLight.direction", g_MainCamera->getDirection());
    shaderProgram->setUniform3f("uSpotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    shaderProgram->setUniform3f("uSpotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shaderProgram->setUniform3f("uSpotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shaderProgram->setUniform1f("uSpotLight.constant", 1.0f);
    shaderProgram->setUniform1f("uSpotLight.linear", 0.045f);
    shaderProgram->setUniform1f("uSpotLight.quadratic", 0.0075f);
    shaderProgram->setUniform1f("uSpotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shaderProgram->setUniform1f("uSpotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    // Defining common material components.
    shaderProgram->setUniform1f("uMaterial.shininess", 64.0f);

    shaderProgram->unbind();
}

/*
 * Drawing...
 *
 * When performing transformations with matrices,
 * use the sequence of operations: translate -> rotate -> scale.
 */
void render()
{
    glClearColor(0.15f, 0.30f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup shaders.
    {
        configureShader(g_PhongLightingModelSP);
        configureShader(g_InstancingModelsSP);
    }

    // Draw objects.
    {
        glEnable(GL_CULL_FACE); // Enable face culling only for rendering closed shapes.

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f, 2.5f, 2.5f));

        g_PhongLightingModelSP->bind();
        g_PhongLightingModelSP->setUniformMatrix4fv("uModelMatrix", modelMatrix);
        g_PhongLightingModelSP->unbind();

        // Draw planet.
        g_PlanetModel->draw(g_PhongLightingModelSP);

        // Draw rocks.
        {
            g_InstancingModelsSP->bind();

            for (const Mesh& mesh : g_RockModel->getMeshes())
            {
                glBindVertexArray(mesh.getVAO());
                glDrawElementsInstanced(GL_TRIANGLES, mesh.m_Indices.size(), GL_UNSIGNED_INT, 0, g_AmountOfRocks);
                glBindVertexArray(0);
            }

            g_InstancingModelsSP->unbind();
        }

        glDisable(GL_CULL_FACE);
    }

    // Draw skybox.
    //
    // This way, the depth buffer is completely filled with all the scene's depth values so we only
    // have to render the skybox's fragments wherever the early depth test passes, greatly reducing
    // the number of fragment shader calls.
    //
    {
        glDepthFunc(GL_LEQUAL);

        g_CubeMapSP->bind();
        g_SkyboxVAO->bind();

        // We can remove the translation section of transformation matrices by taking
        // the upper-left 3x3 matrix of the 4x4 matrix.
        //
        glm::mat4 viewMatrix = glm::mat4(glm::mat3(g_MainCamera->getViewMatrix()));

        g_CubeMapSP->setUniformMatrix4fv("uViewMatrix", viewMatrix);
        g_CubeMapSP->setUniformMatrix4fv("uProjectionMatrix", g_ProjectionMatrix);
        g_CubeMapSP->setUniform1i("uCubeMap", 8);
        g_CubeMapSP->setUniform1i("uTrickDepthBuffer", 1);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        g_CubeMapSP->unbind();
        g_SkyboxVAO->unbind();

        glDepthFunc(GL_LESS);
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
            ImGui::Text("Around 57 million VPS");
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
    io.WantCaptureMouse = true; // FIXME: Forcing flag value to try fixing focus ImGui/GLFW problem.

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable geyboard controls.
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable gamepad controls.

    ImGui::StyleColorsDark();

    // Setup platform/renderer backends.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    /* Configure OpenGL context */
    glViewport(0, 0, g_WindowWidth, g_WindowHeight);

    /* Enable OpenGL features */
    glEnable(GL_DEPTH_TEST);   // Enable depth test.
    glEnable(GL_BLEND);        // Enable blending.
    glEnable(GL_MULTISAMPLE);  // Enable multisampling.
    // glEnable(GL_CULL_FACE); // Enable face culling only for rendering closed shapes.

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
        else
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