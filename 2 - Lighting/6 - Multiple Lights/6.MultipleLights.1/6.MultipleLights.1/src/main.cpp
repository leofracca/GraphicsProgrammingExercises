/*
* Can you (sort of) re-create the different atmospheres of the last image
* by tweaking the light's attribute values?
* 
* This program uses ImGui to create a window where the values of the lights can be changed
*/
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <string>

const std::string TITLE = "MultipleLights1";

// Window settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static bool isPaused = false;

void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Press the space key to pause the scene and make the 
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        isPaused = !isPaused;
        if (isPaused)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (isPaused)
        return;

    // Handle the camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Actual position of the mouse (initialized at the center of the window)
    static float lastX = SCR_WIDTH / 2.0;
    static float lastY = SCR_HEIGHT / 2.0;

    // Rotation on axes
    static float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    static float pitch = 0.0f;

    // To avoid big jump at the beginning
    static bool firstMouse = true;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    if (!isPaused)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollMouse_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    if (!isPaused)
        camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scrollMouse_callback);

    // Tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Shader lightingShader("res/shaders/colors.vs", "res/shaders/colors.fs");
    Shader lightCubeShader("res/shaders/light_cube.vs", "res/shaders/light_cube.fs");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    // Configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Load the diffuse map and the specular map
    Texture diffuseMap("res/textures/container2.png", GL_CLAMP_TO_EDGE, true); // We pass true as transparent argument because the file is a .png
    Texture specularMap("res/textures/container2_specular.png", GL_CLAMP_TO_EDGE, true);
    lightingShader.use();
    lightingShader.setInt("u_material.diffuse", 0);
    lightingShader.setInt("u_material.specular", 1);

    ImVec4 clearColorValues = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    struct
    {
        bool active = true;
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
    } directionalLight;

    struct
    {
        bool active = true;
        glm::vec3 position;
        glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    } pointLights[4];
    // Initialize point lights positions
    for (int i = 0; i < 4; i++)
    {
        pointLights[i].position = pointLightPositions[i];
    }

    struct
    {
        bool active = true;
        glm::vec3 position = camera.GetPosition();
        glm::vec3 direction = camera.GetFront();
        float cutoff = glm::cos(glm::radians(12.5f));
        float outerCutoff = glm::cos(glm::radians(17.5f));
        glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    } spotLight;

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // Calculate the time elapsed between this frame and the previous one
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame; // time between current frame and last frame
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(clearColorValues.x, clearColorValues.y, clearColorValues.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the object

        lightingShader.use();
        // Set the object properties
        diffuseMap.bind();
        specularMap.bind(1);
        lightingShader.setFloat("u_material.shininess", 32.0f);
        // Set the lights
        // 1. Directional light
        lightingShader.setVec3("u_directionalLight.direction", directionalLight.direction);
        if (directionalLight.active)
        {
            lightingShader.setVec3("u_directionalLight.ambient", directionalLight.ambient);
            lightingShader.setVec3("u_directionalLight.diffuse", directionalLight.diffuse);
            lightingShader.setVec3("u_directionalLight.specular", directionalLight.specular);
        }
        else
        {
            lightingShader.setVec3("u_directionalLight.ambient", glm::vec3(0.0f));
            lightingShader.setVec3("u_directionalLight.diffuse", glm::vec3(0.0f));
            lightingShader.setVec3("u_directionalLight.specular", glm::vec3(0.0f));
        }
        
        // 2. Point lights (4 in this example)
        for (int i = 0; i < 4; i++)
        {
            std::string index = std::to_string(i);
            lightingShader.setVec3("u_pointLights[" + index + "].position", pointLights[i].position);
            lightingShader.setFloat("u_pointLights[" + index + "].constant", pointLights[i].constant);
            lightingShader.setFloat("u_pointLights[" + index + "].linear", pointLights[i].linear);
            lightingShader.setFloat("u_pointLights[" + index + "].quadratic", pointLights[i].quadratic);
            if (pointLights[i].active)
            {
                lightingShader.setVec3("u_pointLights[" + index + "].ambient", pointLights[i].ambient);
                lightingShader.setVec3("u_pointLights[" + index + "].diffuse", pointLights[i].diffuse);
                lightingShader.setVec3("u_pointLights[" + index + "].specular", pointLights[i].specular);
            }
            else
            {
                lightingShader.setVec3("u_pointLights[" + index + "].ambient", glm::vec3(0.0f));
                lightingShader.setVec3("u_pointLights[" + index + "].diffuse", glm::vec3(0.0f));
                lightingShader.setVec3("u_pointLights[" + index + "].specular", glm::vec3(0.0f));
            }
        }
        // 3. Spot light
        lightingShader.setVec3("u_spotLight.position", camera.GetPosition());
        lightingShader.setVec3("u_spotLight.direction", camera.GetFront());
        lightingShader.setFloat("u_spotLight.cutoff", spotLight.cutoff);
        lightingShader.setFloat("u_spotLight.outerCutoff", spotLight.outerCutoff);
        lightingShader.setFloat("u_spotLight.constant", spotLight.constant);
        lightingShader.setFloat("u_spotLight.linear", spotLight.linear);
        lightingShader.setFloat("u_spotLight.quadratic", spotLight.quadratic);
        if (spotLight.active)
        {
            lightingShader.setVec3("u_spotLight.ambient", spotLight.ambient);
            lightingShader.setVec3("u_spotLight.diffuse", spotLight.diffuse);
            lightingShader.setVec3("u_spotLight.specular", spotLight.specular);
        }
        else
        {
            lightingShader.setVec3("u_spotLight.ambient", glm::vec3(0.0f));
            lightingShader.setVec3("u_spotLight.diffuse", glm::vec3(0.0f));
            lightingShader.setVec3("u_spotLight.specular", glm::vec3(0.0f));
        }

        // Set the camera position
        lightingShader.setVec3("u_viewPos", camera.GetPosition());

        glm::mat4 proj = glm::perspective(glm::radians(camera.GetZoom()), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Render containers
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // Calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMatrix4f("u_model", model);
            glm::mat4 mvp = proj * view * model;
            lightingShader.setMatrix4f("u_mvp", mvp);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Render the light sources
        for (int i = 0; i < 4; i++)
        {
            lightCubeShader.use();
            // Set the color of the lighting cube to the color of the light it is making
            lightCubeShader.setVec3("u_lightColor", pointLights[i].diffuse);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLights[i].position);
            model = glm::scale(model, glm::vec3(0.2f));
            glm::mat4 mvp = proj * view * model;
            lightCubeShader.setMatrix4f("u_mvp", mvp);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /*bool show_demo_window = true;
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);*/
        {
            ImGui::Begin("Lights Properties");
            ImGui::ColorEdit3("Clear Color", (float*)&clearColorValues);

            if (ImGui::CollapsingHeader("Directional Light"))
            {
                ImGui::Checkbox("Active", &directionalLight.active);
                ImGui::SliderFloat3("Direction", &directionalLight.direction.x, -1.0f, 1.0f);
                ImGui::ColorEdit3("Ambient", &directionalLight.ambient.x);
                ImGui::ColorEdit3("Diffuse", &directionalLight.diffuse.x);
                ImGui::ColorEdit3("Specular", &directionalLight.specular.x);
            }

            if (ImGui::CollapsingHeader("Point Lights"))
            {
                if (ImGui::TreeNode("Point Light 1"))
                {
                    ImGui::Checkbox("Active", &pointLights[0].active);
                    ImGui::SliderFloat3("Position", &pointLights[0].position.x, -10.0f, 10.0f);
                    ImGui::ColorEdit3("Ambient", &pointLights[0].ambient.x);
                    ImGui::ColorEdit3("Diffuse", &pointLights[0].diffuse.x);
                    ImGui::ColorEdit3("Specular", &pointLights[0].specular.x);
                    ImGui::SliderFloat("Constant", &pointLights[0].constant, 0.0f, 1.0f);
                    ImGui::SliderFloat("Linear", &pointLights[0].linear, 0.0f, 1.0f);
                    ImGui::SliderFloat("Quadratic", &pointLights[0].quadratic, 0.0f, 1.0f);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Point Light 2"))
                {
                    ImGui::Checkbox("Active", &pointLights[1].active);
                    ImGui::SliderFloat3("Position", &pointLights[1].position.x, -10.0f, 10.0f);
                    ImGui::ColorEdit3("Ambient", &pointLights[1].ambient.x);
                    ImGui::ColorEdit3("Diffuse", &pointLights[1].diffuse.x);
                    ImGui::ColorEdit3("Specular", &pointLights[1].specular.x);
                    ImGui::SliderFloat("Constant", &pointLights[1].constant, 0.0f, 1.0f);
                    ImGui::SliderFloat("Linear", &pointLights[1].linear, 0.0f, 1.0f);
                    ImGui::SliderFloat("Quadratic", &pointLights[1].quadratic, 0.0f, 1.0f);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Point Light 3"))
                {
                    ImGui::Checkbox("Active", &pointLights[2].active);
                    ImGui::SliderFloat3("Position", &pointLights[2].position.x, -10.0f, 10.0f);
                    ImGui::ColorEdit3("Ambient", &pointLights[2].ambient.x);
                    ImGui::ColorEdit3("Diffuse", &pointLights[2].diffuse.x);
                    ImGui::ColorEdit3("Specular", &pointLights[2].specular.x);
                    ImGui::SliderFloat("Constant", &pointLights[2].constant, 0.0f, 1.0f);
                    ImGui::SliderFloat("Linear", &pointLights[2].linear, 0.0f, 1.0f);
                    ImGui::SliderFloat("Quadratic", &pointLights[2].quadratic, 0.0f, 1.0f);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Point Light 4"))
                {
                    ImGui::Checkbox("Active", &pointLights[3].active);
                    ImGui::SliderFloat3("Position", &pointLights[3].position.x, -10.0f, 10.0f);
                    ImGui::ColorEdit3("Ambient", &pointLights[3].ambient.x);
                    ImGui::ColorEdit3("Diffuse", &pointLights[3].diffuse.x);
                    ImGui::ColorEdit3("Specular", &pointLights[3].specular.x);
                    ImGui::SliderFloat("Constant", &pointLights[3].constant, 0.0f, 1.0f);
                    ImGui::SliderFloat("Linear", &pointLights[3].linear, 0.0f, 1.0f);
                    ImGui::SliderFloat("Quadratic", &pointLights[3].quadratic, 0.0f, 1.0f);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }

            if (ImGui::CollapsingHeader("Spot Light"))
            {
                ImGui::Checkbox("Active", &spotLight.active);
                ImGui::ColorEdit3("Ambient", &spotLight.ambient.x);
                ImGui::ColorEdit3("Diffuse", &spotLight.diffuse.x);
                ImGui::ColorEdit3("Specular", &spotLight.specular.x);
                ImGui::SliderFloat("Constant", &spotLight.constant, 0.0f, 1.0f);
                ImGui::SliderFloat("Linear", &spotLight.linear, 0.0f, 1.0f);
                ImGui::SliderFloat("Quadratic", &spotLight.quadratic, 0.0f, 1.0f);
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // Rendering (ImGui)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}