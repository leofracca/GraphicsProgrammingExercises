/*
* Try drawing a second container with another call to glDrawElements but place it at
* a different position using transformations only.
* Make sure this second container is placed at the top-left of the window and instead
* of rotating, scale it over time (using the sin function is useful here; note that
* using sin will cause the object to invert as soon as a negative scale is applied)
*/
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "Shader.h"
#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

const std::string TITLE = "Transformations3";

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, TITLE.c_str(), NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("res/shaders/vertex.vs",
                  "res/shaders/fragment.fs");
    shader.use();

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO; // EBO to store indices data
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned long long dataSize = sizeof(vertices) / sizeof(*vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, dataSize, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, dataSize, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, dataSize, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Initialize textures
    Texture texture1("res/textures/container.jpg", GL_CLAMP_TO_EDGE),
            texture2("res/textures/awesomeface.png", GL_REPEAT, true);
    shader.setInt("u_texture1", 0);
    shader.setInt("u_texture2", 1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        texture1.bind();
        texture2.bind(1);

        shader.use();

        // First container
        glm::mat4 transform1(1.0f);
        transform1 = glm::translate(transform1, glm::vec3(0.5f, -0.5f, 0.0f));
        transform1 = glm::rotate(transform1, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMatrix4f("u_transform", transform1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Second container
        glm::mat4 transform2(1.0f);
        transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));
        double scale = sin(glfwGetTime());
        transform2 = glm::scale(transform2, glm::vec3(scale, scale, scale));
        shader.setMatrix4f("u_transform", transform2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}