#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/VertexArray.h"
#include "core/VertexBuffer.h"
#include "core/ElementBuffer.h"
#include "core/ShaderProgram.h"

// Global window properties.
const int g_WindowWidth  = 800;
const int g_WindowHeight = 600;

// GLFW window callbacks.
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

// Global variables.
VertexArray*   g_RectangleVAO;
VertexBuffer*  g_RectangleVBO;
ElementBuffer* g_RectangleEBO;
ShaderProgram* g_BasicSP;

int g_DrawMode = GL_FILL;

void setup()
{
    float vertices[] = {
         0.5f,  0.5f,  0.0f,  // top right
         0.5f, -0.5f,  0.0f,  // bottom right
        -0.5f, -0.5f,  0.0f,  // bottom left
        -0.5f,  0.5f,  0.0f   // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    g_BasicSP = new ShaderProgram("scripts/basic_vs.glsl", "scripts/basic_fs.glsl");

    g_RectangleVAO = new VertexArray();
    g_RectangleVBO = new VertexBuffer(vertices, sizeof(vertices));
    g_RectangleEBO = new ElementBuffer(indices, sizeof(indices));

    g_RectangleVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    g_RectangleVAO->unbind(); // Unbind VAO before another buffer.
    g_RectangleVBO->unbind();
    g_RectangleEBO->unbind();
}

void render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Drawing */

    g_BasicSP->bind();
    g_RectangleVAO->bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    g_BasicSP->unbind();
    g_RectangleVAO->unbind();
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

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Configure OpenGL context */
    glViewport(0, 0, g_WindowWidth, g_WindowHeight);

    /* Define window callbacks. */
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyboardCallback);

    setup();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        render();

        /* Process keyboard/mouse inputs */
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
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        g_DrawMode = g_DrawMode == GL_FILL ? GL_LINE : GL_FILL;

        glPolygonMode(GL_FRONT_AND_BACK, g_DrawMode);
    }
}

void processInput(GLFWwindow* window)
{
    /* Nothing for while */
}