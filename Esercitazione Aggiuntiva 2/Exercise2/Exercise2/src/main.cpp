/*
* Exercise 2 - Shadow Mapping
* 
* This program uses Dear ImGui to change the properties of the light and the objects.
* Press SPACE to see the cursor and interact with the Gui.
*/
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <string>

const std::string TITLE = "Exercise 2 - Shadow Mapping";

// Window settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static bool isPaused = false;

struct Cube
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    float rotationAngle;
};

unsigned int planeVAO; // Meshes

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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // Initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
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
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // Render cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// Renders the 3D scene
void renderScene(const Shader& shader, const Cube cubes[3])
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMatrix4f("u_model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Draw some cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubes[0].position);
    model = glm::rotate(model, cubes[0].rotationAngle, cubes[0].rotation);
    model = glm::scale(model, cubes[0].scale);
    shader.setMatrix4f("u_model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubes[1].position);
    model = glm::rotate(model, cubes[1].rotationAngle, cubes[1].rotation);
    model = glm::scale(model, cubes[1].scale);
    shader.setMatrix4f("u_model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubes[2].position);
    model = glm::rotate(model, cubes[2].rotationAngle, cubes[2].rotation);
    model = glm::scale(model, cubes[2].scale);
    shader.setMatrix4f("u_model", model);
    renderCube();
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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Shader simpleDepthShader("res/shaders/depth.vs", "res/shaders/depth.fs");
    Shader debugDepthQuad("res/shaders/debug_quad.vs", "res/shaders/debug_quad_depth.fs");
    Shader drawShader("res/shaders/shadow_map.vs", "res/shaders/shadow_map.fs");

    // Set up vertex data (and buffer(s)) and configure vertex attributes
    float planeVertices[] = {
         // positions         // normals          // texcoords
         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,

         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   0.0f,  5.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,   5.0f,  5.0f
    };
    // Plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    Texture woodTexture("res/textures/wood.png", GL_CLAMP_TO_EDGE, true);

    // Configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // Create depth texture
    Texture depthMap(SHADOW_WIDTH, SHADOW_HEIGHT);
    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.getRendererID(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    drawShader.use();
    drawShader.setInt("u_diffuseTexture", 0);
    drawShader.setInt("u_shadowMap", 1);
    debugDepthQuad.use();
    debugDepthQuad.setInt("u_depthMap", 0);

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    ImVec4 clearColorValues = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    struct
    {
        bool active = true;
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
    } directionalLight;

    Cube cubes[3];
    cubes[0].position = glm::vec3(0.0f, 1.5f, 0.0);
    cubes[0].scale = glm::vec3(0.5f);
    cubes[0].rotation = glm::vec3(1.0f);
    cubes[0].rotationAngle = 0.0f;
    cubes[1].position = glm::vec3(2.0f, 0.0f, 1.0);
    cubes[1].scale = glm::vec3(0.5f);
    cubes[1].rotation = glm::vec3(1.0f);
    cubes[1].rotationAngle = 0.0f;
    cubes[2].position = glm::vec3(-1.0f, 0.0f, 2.0);
    cubes[2].scale = glm::vec3(0.25);
    cubes[2].rotation = glm::vec3(1.0, 0.0, 1.0);
    cubes[2].rotationAngle = 60.0f;

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

        // 1. Render depth of scene to texture (from light's perspective)
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        // Render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMatrix4f("u_lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        woodTexture.bind();
        renderScene(simpleDepthShader, cubes);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. Render scene as normal using the generated depth/shadow map from previous step
        drawShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        drawShader.setMatrix4f("u_projection", projection);
        drawShader.setMatrix4f("u_view", view);
        // set light uniforms
        drawShader.setVec3("u_viewPos", camera.GetPosition());
        drawShader.setVec3("u_lightPos", lightPos);
        drawShader.setMatrix4f("u_lightSpaceMatrix", lightSpaceMatrix);
        woodTexture.bind();
        depthMap.bind(1);
        renderScene(drawShader, cubes);

        // Set the lights
        // 1. Directional light
        simpleDepthShader.setVec3("u_directionalLight.direction", directionalLight.direction);
        if (directionalLight.active)
        {
            drawShader.setVec3("u_directionalLight.ambient", directionalLight.ambient);
            drawShader.setVec3("u_directionalLight.diffuse", directionalLight.diffuse);
            drawShader.setVec3("u_directionalLight.specular", directionalLight.specular);
        }
        else
        {
            drawShader.setVec3("u_directionalLight.ambient", glm::vec3(0.0f));
            drawShader.setVec3("u_directionalLight.diffuse", glm::vec3(0.0f));
            drawShader.setVec3("u_directionalLight.specular", glm::vec3(0.0f));
        }

        // Control lights with Dear ImGui
        {
            ImGui::Begin("Lights Properties");
            ImGui::ColorEdit3("Clear Color", (float*)&clearColorValues);

            ImGui::Checkbox("Active", &directionalLight.active);
            ImGui::SliderFloat3("Direction", &lightPos.x, 0.0f, 10.0f);
            ImGui::ColorEdit3("Ambient", &directionalLight.ambient.x);
            ImGui::ColorEdit3("Diffuse", &directionalLight.diffuse.x);
            ImGui::ColorEdit3("Specular", &directionalLight.specular.x);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // Control objects with Dear ImGui
        {
            ImGui::Begin("Objects Properties");

            if (ImGui::BeginTabBar("CubesProperties", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Cube 1"))
                {
                    ImGui::SliderFloat3("Position", &cubes[0].position.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Scale", &cubes[0].scale.x, 0.1f, 10.0f);
                    ImGui::SliderFloat3("RotationAxis", &cubes[0].rotation.x, 0.0f, 1.0f);
                    ImGui::SliderAngle("RotationAngle", &cubes[0].rotationAngle); // The slider values go from -360 to 360 (degrees), but the value is saved in radians
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Cube 2"))
                {
                    ImGui::SliderFloat3("Position", &cubes[1].position.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Scale", &cubes[1].scale.x, 0.1f, 10.0f);
                    ImGui::SliderFloat3("RotationAxis", &cubes[1].rotation.x, 0.0f, 1.0f);
                    ImGui::SliderAngle("RotationAngle", &cubes[1].rotationAngle); // The slider values go from -360 to 360 (degrees), but the value is saved in radians
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Cube 3"))
                {
                    ImGui::SliderFloat3("Position", &cubes[2].position.x, -10.0f, 10.0f);
                    ImGui::SliderFloat3("Scale", &cubes[2].scale.x, 0.1f, 10.0f);
                    ImGui::SliderFloat3("RotationAxis", &cubes[2].rotation.x, 0.0f, 1.0f);
                    ImGui::SliderAngle("RotationAngle", &cubes[2].rotationAngle); // The slider values go from -360 to 360 (degrees), but the value is saved in radians
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        // Rendering (ImGui)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Render Depth map to quad for visual debugging
        debugDepthQuad.use();
        debugDepthQuad.setFloat("u_near_plane", 1.0f);
        debugDepthQuad.setFloat("u_far_plane", 15.0f);
        depthMap.bind();
        // renderQuad();

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