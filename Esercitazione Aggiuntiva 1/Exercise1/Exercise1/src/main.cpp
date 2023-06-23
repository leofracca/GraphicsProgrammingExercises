/*
* Exercise 1 - Instantiate multiple objects and multiple lights in the same scene and play with them.
* 
* In this scene there are 2 objects and 2 lights (1 directional light and 1 spotlight attached to the camera).
* The user can move the objects and change their properties and the properties of the lights using a GUI
* and move the camera around the scene.
* The Gui is made using Dear ImGui. To modify the properties press SPACE to see the cursor.
*/
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <string>

const std::string TITLE = "Exercise 1";

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

    stbi_set_flip_vertically_on_load(1);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Shader shader("res/shaders/vertex.vs", "res/shaders/fragment.fs");

    Model backpackObj1("res/objects/backpack/backpack.obj");
    Model backpackObj2("res/objects/backpack/backpack.obj");

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

    struct ObjectTransformation
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    ObjectTransformation backpack1{
        { -2.0f, 0.0f, -3.0f },
        { 1.0f, 0.0f,  0.0f },
        { 1.0f, 1.0f,  1.0f }
    };

    ObjectTransformation backpack2{
        { 2.0f, 0.0f, -3.0f },
        { 1.0f, 0.0f,  0.0f },
        { 1.0f, 1.0f,  1.0f }
    };

    float lastFrame = 0.0f;
    float angle1 = 0.0f, angle2 = 0.0f;
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

        shader.use();
        // Set the lights
        // 1. Directional light
        shader.setVec3("u_directionalLight.direction", directionalLight.direction);
        if (directionalLight.active)
        {
            shader.setVec3("u_directionalLight.ambient", directionalLight.ambient);
            shader.setVec3("u_directionalLight.diffuse", directionalLight.diffuse);
            shader.setVec3("u_directionalLight.specular", directionalLight.specular);
        }
        else
        {
            shader.setVec3("u_directionalLight.ambient", glm::vec3(0.0f));
            shader.setVec3("u_directionalLight.diffuse", glm::vec3(0.0f));
            shader.setVec3("u_directionalLight.specular", glm::vec3(0.0f));
        }
        // 2. Spot light
        shader.setVec3("u_spotLight.position", camera.GetPosition());
        shader.setVec3("u_spotLight.direction", camera.GetFront());
        shader.setFloat("u_spotLight.cutoff", spotLight.cutoff);
        shader.setFloat("u_spotLight.outerCutoff", spotLight.outerCutoff);
        shader.setFloat("u_spotLight.constant", spotLight.constant);
        shader.setFloat("u_spotLight.linear", spotLight.linear);
        shader.setFloat("u_spotLight.quadratic", spotLight.quadratic);
        if (spotLight.active)
        {
            shader.setVec3("u_spotLight.ambient", spotLight.ambient);
            shader.setVec3("u_spotLight.diffuse", spotLight.diffuse);
            shader.setVec3("u_spotLight.specular", spotLight.specular);
        }
        else
        {
            shader.setVec3("u_spotLight.ambient", glm::vec3(0.0f));
            shader.setVec3("u_spotLight.diffuse", glm::vec3(0.0f));
            shader.setVec3("u_spotLight.specular", glm::vec3(0.0f));
        }

        // Set the camera position
        shader.setVec3("u_viewPos", camera.GetPosition());

        glm::mat4 proj = glm::perspective(glm::radians(camera.GetZoom()), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Render the first backpack
        glm::mat4 model(1.0f);
        model = glm::translate(model, backpack1.position);
        model = glm::rotate(model, angle1, backpack1.rotation);
        model = glm::scale(model, backpack1.scale);
        shader.setMatrix4f("u_model", model);
        glm::mat4 mvp = proj * view * model;
        shader.setMatrix4f("u_mvp", mvp);

        backpackObj1.draw(shader);

        // Render the second backpack
        model = glm::mat4(1.0f);
        model = glm::translate(model, backpack2.position);
        model = glm::rotate(model, angle2, backpack2.rotation);
        model = glm::scale(model, backpack2.scale);
        shader.setMatrix4f("u_model", model);
        mvp = proj * view * model;
        shader.setMatrix4f("u_mvp", mvp);

        backpackObj2.draw(shader);

        {
            ImGui::Begin("Lights Properties");
            ImGui::ColorEdit3("Clear Color", (float*)&clearColorValues);

            if (ImGui::BeginTabBar("LightsProperties", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Directional Light"))
                {
                    ImGui::Checkbox("Active", &directionalLight.active);
                    ImGui::SliderFloat3("Direction", &directionalLight.direction.x, -1.0f, 1.0f);
                    ImGui::ColorEdit3("Ambient", &directionalLight.ambient.x);
                    ImGui::ColorEdit3("Diffuse", &directionalLight.diffuse.x);
                    ImGui::ColorEdit3("Specular", &directionalLight.specular.x);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Spot Light"))
                {
                    ImGui::Checkbox("Active", &spotLight.active);
                    ImGui::ColorEdit3("Ambient", &spotLight.ambient.x);
                    ImGui::ColorEdit3("Diffuse", &spotLight.diffuse.x);
                    ImGui::ColorEdit3("Specular", &spotLight.specular.x);
                    ImGui::SliderFloat("Constant", &spotLight.constant, 0.0f, 1.0f);
                    ImGui::SliderFloat("Linear", &spotLight.linear, 0.0f, 1.0f);
                    ImGui::SliderFloat("Quadratic", &spotLight.quadratic, 0.0f, 1.0f);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        {
            ImGui::Begin("Objects Properties");

            if (ImGui::BeginTabBar("ObjectsProperties", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Object 1"))
                {
                    ImGui::SliderFloat3("Position", &backpack1.position.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("RotationAxis", &backpack1.rotation.x, 0.0f, 1.0f);
                    ImGui::SliderAngle("RotationAngle", &angle1); // The slider values go from -360 to 360 (degrees), but the value is saved in radians
                    ImGui::SliderFloat3("Scale", &backpack1.scale.x, 1.0f, 10.0f);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Object 2"))
                {
                    ImGui::SliderFloat3("Position", &backpack2.position.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("RotationAxis", &backpack2.rotation.x, 0.0f, 1.0f);
                    ImGui::SliderAngle("RotationAngle", &angle2); // The slider values go from -360 to 360 (degrees), but the value is saved in radians
                    ImGui::SliderFloat3("Scale", &backpack2.scale.x, 1.0f, 10.0f);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        // Rendering (ImGui)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}