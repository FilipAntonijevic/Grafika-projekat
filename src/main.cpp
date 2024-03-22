#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//up and down movement
#include <chrono>
#include <cmath>
//za svetlo
#include <GL/gl.h>
#include <GL/glu.h>


#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

float calculateHeight(float initialHeight, float amplitude, float frequency, std::chrono::steady_clock::time_point startTime);

unsigned int loadCubemap(vector<std::string> faces);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadTexture(char const * path);

//light delay
void resetTimer(std::string s);
double getElapsedTime(std::string s);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// getElapsedTime()
std::chrono::steady_clock::time_point startTimeFire;
std::chrono::steady_clock::time_point startTimeTree;
std::chrono::steady_clock::time_point currentTime;

//variables and constants
auto startTime1_1 = std::chrono::steady_clock::now();
auto startTime1_2 = std::chrono::steady_clock::now() + std::chrono::milliseconds (234);
auto startTime1_3 = std::chrono::steady_clock::now() + std::chrono::milliseconds (792);
auto startTime1_4 = std::chrono::steady_clock::now() + std::chrono::milliseconds (132);
auto startTime1_5 = std::chrono::steady_clock::now() + std::chrono::milliseconds (764);
auto startTime1_6 = std::chrono::steady_clock::now() + std::chrono::milliseconds (332);

auto startTime2_1 = std::chrono::steady_clock::now() + std::chrono::milliseconds (542);
auto startTime2_2 = std::chrono::steady_clock::now() + std::chrono::milliseconds (212);
auto startTime2_3 = std::chrono::steady_clock::now() + std::chrono::milliseconds (934);
auto startTime2_4 = std::chrono::steady_clock::now() + std::chrono::milliseconds (532);
auto startTime2_5 = std::chrono::steady_clock::now() + std::chrono::milliseconds (223);
auto startTime2_6 = std::chrono::steady_clock::now() + std::chrono::milliseconds (811);
float newStairHeight;
float stairsMovementFrequency = 0.36;
float stairsMovementAmplitude = 1.2;

bool no_mouse = true;
bool light_on_off = false;
bool tree_red = false;
double elapsedTimeFire;
double elapsedTimeTree;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 mammoth1Position = glm::vec3(-15, -0.7, -9);
    glm::vec3 mammoth2Position = glm::vec3(-41, -23.5, -8);
    glm::vec3 treePosition = glm::vec3(20, -19, 5);
    glm::vec3 islandPosition = glm::vec3(-60, 215.0f, -43);
    glm::vec3 toro1Position = glm::vec3(8, -0.7, 34);
    glm::vec3 fire1Position = glm::vec3(8, 4.7, 34);
    glm::vec3 toro2Position = glm::vec3(-25, -23.4, -22);
    glm::vec3 fire2Position = glm::vec3(-25, -18, -22);
    glm::vec3 toro3Position = glm::vec3(12, -49.3, -27);
    glm::vec3 fire3Position = glm::vec3(12, -43.9, -27);

    float treeScale = 0.19;
    float mammoth1Scale = 0.1f;
    float mammoth2Scale = 0.08f;
    float islandScale = 1.5f;
    float toro1Scale = 9;
    float toro2Scale = 9;
    float toro3Scale = 9;
    float fireScale = 0.017;
    PointLight pointLight1;
    PointLight pointLight2;
    PointLight pointLight3;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "grafika", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // build and compile shaders
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader blendingShader("resources/shaders/blending.vs", "resources/shaders/blending.fs");
    // load models
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Model tree1("resources/objects/tree/tree.obj");
    Model tree2("resources/objects/tree2/tree.obj");
    Model tree3("resources/objects/tree3/tree.obj");
    Model tree4("resources/objects/tree4/tree.obj");
    Model island("resources/objects/island1/4.obj");
    Model mammoth1("resources/objects/mammoth/mammoth.obj");
    Model mammoth2("resources/objects/mammoth/mammoth.obj");
    Model toro1("resources/objects/toro/Toro.obj");
    Model toro2("resources/objects/toro/Toro.obj");
    Model toro3("resources/objects/toro/Toro.obj");
    Model fire2("resources/objects/fire/campfire.obj");
    Model fire3("resources/objects/fire/campfire.obj");
    Model fire1("resources/objects/fire/campfire.obj");

    tree1.SetShaderTextureNamePrefix("material.");
    tree2.SetShaderTextureNamePrefix("material.");
    tree3.SetShaderTextureNamePrefix("material.");
    tree4.SetShaderTextureNamePrefix("material.");
    island.SetShaderTextureNamePrefix("material.");
    mammoth1.SetShaderTextureNamePrefix("material.");
    mammoth2.SetShaderTextureNamePrefix("material.");
    toro1.SetShaderTextureNamePrefix("material.");
    toro2.SetShaderTextureNamePrefix("material.");
    toro3.SetShaderTextureNamePrefix("material.");
    fire1.SetShaderTextureNamePrefix("material.");
    fire2.SetShaderTextureNamePrefix("material.");
    fire3.SetShaderTextureNamePrefix("material.");
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Lights
    PointLight& Light1 = programState->pointLight1;
    Light1.position = glm::vec3(8, 4.7, 34);
    Light1.ambient = glm::vec3(0.0, 0.0, 0.0);
    Light1.diffuse = glm::vec3(0.9, 0.9, 0.9);
    Light1.specular = glm::vec3(1.0, 1.0, 1.0);
    Light1.constant = 17.0f;
    Light1.linear = 0.09f;
    Light1.quadratic = 0.032f;

    PointLight& Light2 = programState->pointLight2;
    Light2.position = glm::vec3(-25, -18, -22);
    Light2.ambient = glm::vec3(0.0, 0.0, 0.0);
    Light2.diffuse = glm::vec3(0.9, 0.9, 0.9);
    Light2.specular = glm::vec3(1.0, 1.0, 1.0);
    Light2.constant = 17.0f;
    Light2.linear = 0.09f;
    Light2.quadratic = 0.032f;

    PointLight& Light3 = programState->pointLight3;
    Light3.position = glm::vec3(12, -43.9, -27);
    Light3.ambient = glm::vec3(0.0, 0.0, 0.0);
    Light3.diffuse = glm::vec3(0.9, 0.9, 0.9);
    Light3.specular = glm::vec3(1.0, 1.0, 1.0);
    Light3.constant = 17.0f;
    Light3.linear = 0.09f;
    Light3.quadratic = 0.032f;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Grass vertices
    float transparentVertices[] = {
            // positions
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    // transparent grass location
    vector<glm::vec3> grassPositions1;
    vector<float> grassRotation1;

    vector<glm::vec3> grassPositions2;
    vector<float> grassRotation2;

    vector<glm::vec3> grassPositions3;
    vector<float> grassRotation3;

    vector<glm::vec3> grassPositions4;
    vector<float> grassRotation4;
// Define parameters for the circle
    int numGrass = 1799;
    float radius = 5.3f; // Adjust the radius as needed
// Generate grass positions in a circle
//1
    for (int i = 0; i < numGrass; i+=2) {
        float angle = glm::two_pi<float>() * i / numGrass; // Calculate angle for each grass element
        float x = (radius * cos(angle))*rand()/(RAND_MAX);
        if(abs(x) >4.7 || abs(x) < 2){
            x = (radius * cos(angle))*rand()/(RAND_MAX);
        }
        float z = (radius * sin(angle))*rand()/(RAND_MAX);
        if(abs(z) >4.7 || abs(z) < 2){
            z = (radius * sin(angle))*rand()/(RAND_MAX);
        }
        float y = rand() % 5 * 0.1f;
        if(y < 0.3){
            y = 0.3;
        }// Randomize the height slightly
        grassPositions1.push_back(glm::vec3(x+0.4, y, z+0.64));
        grassRotation1.push_back(rand() % 180);
    }
    for (int i =1; i < numGrass; i+=2) {
        float angle = glm::two_pi<float>() * i / numGrass; // Calculate angle for each grass element
        float x = (radius * cos(angle));
        float z = (radius * sin(angle));
        float y = rand() % 5 * 0.1f;
        if(y < 0.3){
            y = 0.3;
        }// Randomize the height slightly
        grassPositions1.push_back(glm::vec3(x+0.4, y, z+0.64));
        grassRotation1.push_back(rand() % 180);
    }
    //2
    radius = 2.9;
    numGrass = 699;
    for (int i = 0; i < numGrass; i+=2) {
        float angle = glm::two_pi<float>() * i / numGrass; // Calculate angle for each grass element
        float x = (radius * cos(angle))*rand()/(RAND_MAX);
        float z = (radius * sin(angle))*rand()/(RAND_MAX);
        float y = rand() % 5 * 0.1f;
        if(y < 0.3){
            y = 0.3;
        }// Randomize the height slightly
        grassPositions2.push_back(glm::vec3(x-5.05, y-3.7, z-0.88));
        grassRotation2.push_back(rand() % 180);
    }
    for (int i =1; i < numGrass; i+=2) {
        float angle = glm::two_pi<float>() * i / numGrass; // Calculate angle for each grass element
        float x = (radius * cos(angle));
        float z = (radius * sin(angle));
        float y = rand() % 5 * 0.1f;
        if(y < 0.3){
            y = 0.3;
        }// Randomize the height slightly
        grassPositions2.push_back(glm::vec3(x-5.05, y-3.7, z-0.88));
        grassRotation2.push_back(rand() % 180);
    }
    //3
    radius = 2.8;
    numGrass = 599;
    for (int i = 0; i < numGrass; i+=2) {
        float angle = glm::two_pi<float>() * i / numGrass; // Calculate angle for each grass element
        float x = (radius * cos(angle))*rand()/(RAND_MAX);
        float z = (radius * sin(angle))*rand()/(RAND_MAX);
        float y = rand() % 5 * 0.1f;
        if(y < 0.3){
            y = 0.3;
        }// Randomize the height slightly
        grassPositions3.push_back(glm::vec3(x-0.3, y-8.05, z-2.7));
        grassRotation3.push_back(rand() % 180);
    }
    for (int i =1; i < numGrass; i+=2) {
        float angle = glm::two_pi<float>() * i / numGrass; // Calculate angle for each grass element
        float x = (radius * cos(angle));
        float z = (radius * sin(angle));
        float y = rand() % 5 * 0.1f;
        if(y < 0.3){
            y = 0.3;
        }// Randomize the height slightly
        grassPositions3.push_back(glm::vec3(x-0.3, y-8.05, z-2.7));
        grassRotation3.push_back(rand() % 180);
    }
    //4 dodatak
    for(int i = 0; i< 30; i++){
        grassPositions4.push_back(glm::vec3(-4-(i/10), -3.4, 2.5));
        grassRotation4.push_back(rand() % 180);
    }
    for(int i = 0; i< 20; i++){
        grassPositions4.push_back(glm::vec3(-4-(i/10), -3.4, 3));
        grassRotation4.push_back(rand() % 180);
    }
    for(int i = 0; i< 20; i++){
        grassPositions4.push_back(glm::vec3(-4.3-(i/10), -3.4, 3.2));
        grassRotation4.push_back(rand() % 180);
    }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //stairs
    float cubeVertices[] = {
            // back face
            -1.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
            1.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
            1.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            1.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            -1.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
            -1.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
            // front face
            -1.5f, -0.5f,  0.9f,  0.0f, 0.0f, // bottom-left
            1.5f,  0.5f,  0.9f,  1.0f, 1.0f, // top-right
            1.5f, -0.5f,  0.9f,  1.0f, 0.0f, // bottom-right
            1.5f,  0.5f,  0.9f,  1.0f, 1.0f, // top-right
            -1.5f, -0.5f,  0.9f,  0.0f, 0.0f, // bottom-left
            -1.5f,  0.5f,  0.9f,  0.0f, 1.0f, // top-left
            // left face
            -1.5f,  0.5f,  0.9f,  1.0f, 0.0f, // top-right
            -1.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
            -1.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
            -1.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
            -1.5f,  0.5f,  0.9f,  1.0f, 0.0f, // top-right
            -1.5f, -0.5f,  0.9f,  0.0f, 0.0f, // bottom-right
            // right face
            1.5f,  0.5f,  0.9f,  1.0f, 0.0f, // top-left
            1.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            1.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
            1.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
            1.5f, -0.5f,  0.9f,  0.0f, 0.0f, // bottom-left
            1.5f,  0.5f,  0.9f,  1.0f, 0.0f, // top-left
            // bottom face
            -1.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
            1.5f, -0.5f,  0.9f,  1.0f, 0.0f, // bottom-left
            1.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
            1.5f, -0.5f,  0.9f,  1.0f, 0.0f, // bottom-left
            -1.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
            -1.5f, -0.5f,  0.9f,  0.0f, 0.0f, // bottom-right
            // top face
            -1.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
            1.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            1.5f,  0.5f,  0.9f,  1.0f, 0.0f, // bottom-right
            1.5f,  0.5f,  0.9f,  1.0f, 0.0f, // bottom-right
            -1.5f,  0.5f,  0.9f,  0.0f, 0.0f, // bottom-left
            -1.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left
    };
    glad_glFrontFace(GL_CW);
    float transparentVerticesCube[] = {
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
//  stairs VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVerticesCube), transparentVerticesCube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load stairs texture
    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/marble3.jpg").c_str());
    // load grass texture
    unsigned int grassTexture = loadTexture(FileSystem::getPath("resources/textures/grass.png").c_str());
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float skyboxVertices[] = {
            // positions
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
    //skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vector<std::string> faces
            {
                    FileSystem::getPath("/resources/textures/skybox/nx.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/px.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/py.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/ny.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/nz.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/pz.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox",0);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);
        if(no_mouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)) {
            no_mouse = false;
        }
        if ((glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)) {
            no_mouse = true;
        }
        // render
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // directional light
        ourShader.setVec3("dirLight.direction", -4, 5.0f, 5);
        ourShader.setVec3("dirLight.ambient", 0.55f, 0.55f, 0.55f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

        //Point light 1
        ourShader.setVec3("pointLight1.position", Light1.position);
        ourShader.setVec3("pointLight1.ambient", Light1.ambient);
        ourShader.setVec3("pointLight1.diffuse", Light1.diffuse);
        ourShader.setVec3("pointLight1.specular", Light1.specular);
        ourShader.setFloat("pointLight1.constant", Light1.constant);
        ourShader.setFloat("pointLight1.linear", Light1.linear);
        ourShader.setFloat("pointLight1.quadratic", Light1.quadratic);
        //Point light 2
        ourShader.setVec3("pointLight2.position", Light2.position);
        ourShader.setVec3("pointLight2.ambient", Light2.ambient);
        ourShader.setVec3("pointLight2.diffuse", Light2.diffuse);
        ourShader.setVec3("pointLight2.specular", Light2.specular);
        ourShader.setFloat("pointLight2.constant", Light2.constant);
        ourShader.setFloat("pointLight2.linear", Light2.linear);
        ourShader.setFloat("pointLight2.quadratic", Light2.quadratic);
        //Point light 3
        ourShader.setVec3("pointLight3.position", Light3.position);
        ourShader.setVec3("pointLight3.ambient", Light3.ambient);
        ourShader.setVec3("pointLight3.diffuse", Light3.diffuse);
        ourShader.setVec3("pointLight3.specular", Light3.specular);
        ourShader.setFloat("pointLight3.constant", Light3.constant);
        ourShader.setFloat("pointLight3.linear", Light3.linear);
        ourShader.setFloat("pointLight3.quadratic", Light3.quadratic);


        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // render the loaded model
        glm::mat4 model;
        //island
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->islandPosition);
        model = glm::scale(model, glm::vec3(programState->islandScale));
        ourShader.setMat4("model", model);
        island.Draw(ourShader);

        //mammoth1
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->mammoth1Position);
        model = glm::scale(model, glm::vec3(programState->mammoth1Scale));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3 (1.0, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(160.0f), glm::vec3 (0.0, 0.0f, 1.0f));
        ourShader.setMat4("model", model);
        mammoth1.Draw(ourShader);

        //mammoth2
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->mammoth2Position);
        model = glm::scale(model, glm::vec3(programState->mammoth2Scale));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3 (1.0, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(100.0f), glm::vec3 (0.0, 0.0f, 1.0f));
        ourShader.setMat4("model", model);
        mammoth2.Draw(ourShader);

        //toro1
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->toro1Position);
        model = glm::scale(model, glm::vec3(programState->toro1Scale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        toro1.Draw(ourShader);

        //toro2
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->toro2Position);
        model = glm::scale(model, glm::vec3(programState->toro2Scale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        toro2.Draw(ourShader);

        //toro3
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->toro3Position);
        model = glm::scale(model, glm::vec3(programState->toro3Scale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        toro3.Draw(ourShader);

        //fire
        if ((glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) && light_on_off == false){
            light_on_off = true;
            resetTimer("Fire");
        }
        if(light_on_off == true){

            elapsedTimeFire = getElapsedTime("Fire");
            //fire1
            if (elapsedTimeFire >= 0.0) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, programState->fire1Position);
                model = glm::scale(model, glm::vec3(programState->fireScale));
                ourShader.setMat4("model", model);
                fire1.Draw(ourShader);
                Light1.ambient = glm::vec3(16.0, 13.0, 13.0);
            }
            //fire2
            if (elapsedTimeFire >= 0.45) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, programState->fire2Position);
                model = glm::scale(model, glm::vec3(programState->fireScale));
                ourShader.setMat4("model", model);
                fire2.Draw(ourShader);
                Light2.ambient = glm::vec3(16.0, 13.0, 13.0);
            }
            //fire3
            if (elapsedTimeFire >= 0.9) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, programState->fire3Position);
                model = glm::scale(model, glm::vec3(programState->fireScale));
                ourShader.setMat4("model", model);
                fire3.Draw(ourShader);
                Light3.ambient = glm::vec3(16.0, 13.0, 13.0);
            }
        }
        if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) and light_on_off == true){
            light_on_off = false;
            Light1.ambient = glm::vec3(0.0, 0.00, 0.0);
            Light2.ambient = glm::vec3(0.0, 0.00, 0.0);
            Light3.ambient = glm::vec3(0.0, 0.00, 0.0);
        }

        // Tree
        if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && tree_red == false){
            tree_red = true;
            resetTimer("Tree");

        }
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->treePosition);
        model = glm::scale(model, glm::vec3(programState->treeScale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0.0, 0.0f, 1.0f));
        ourShader.setMat4("model", model);
        if(tree_red){
            elapsedTimeTree = getElapsedTime("Tree");

            if (elapsedTimeTree < 2.5 && elapsedTimeTree >= 0.0) {
                //tree2
                tree2.Draw(ourShader);
            }
            if (elapsedTimeTree<5 && elapsedTimeTree >= 2.5) {
                //tree3
                tree3.Draw(ourShader);
            }
            if (elapsedTimeTree >= 5) {
                //tree4
                tree4.Draw(ourShader);
            }
        }else {
            //tree
            tree1.Draw(ourShader);
        }
        if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && tree_red == true){
            tree_red = false;
        }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //blending
        blendingShader.use();

        model = glm::mat4(1.0f);
        blendingShader.setMat4("projection", projection);
        blendingShader.setMat4("view", view);

        glEnable(GL_CULL_FACE);

        //stairs
        //stair1.1
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-3,stairsMovementAmplitude,stairsMovementFrequency,startTime1_1);
        model = glm::translate(model, glm::vec3(-14.0f, newStairHeight, 36.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-24.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair1.2
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-7,stairsMovementAmplitude,stairsMovementFrequency,startTime1_2);
        model = glm::translate(model, glm::vec3(-18.3f, newStairHeight, 40.7f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-60.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair1.3
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-11,stairsMovementAmplitude,stairsMovementFrequency,startTime1_3);
        model = glm::translate(model, glm::vec3(-25.0f, newStairHeight, 41.5f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-105.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair1.4
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-15,stairsMovementAmplitude,stairsMovementFrequency,startTime1_4);
        model = glm::translate(model, glm::vec3(-31.5f, newStairHeight, 37.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(40.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair1.5
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-19,stairsMovementAmplitude,stairsMovementFrequency,startTime1_5);
        model = glm::translate(model, glm::vec3(-33.5f, newStairHeight, 29.5f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(10.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair1.6
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-23,stairsMovementAmplitude,stairsMovementFrequency,startTime1_6);
        model = glm::translate(model, glm::vec3(-33.0f, newStairHeight, 22.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-15.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //stair2.1
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-26,stairsMovementAmplitude,stairsMovementFrequency,startTime2_1);
        model = glm::translate(model, glm::vec3(-38, newStairHeight, -29.9f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(14.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair2.2
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-30.4,stairsMovementAmplitude,stairsMovementFrequency,startTime2_2);
        model = glm::translate(model, glm::vec3(-37.5, newStairHeight, -35.9f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair2.3
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-34.8,stairsMovementAmplitude,stairsMovementFrequency,startTime2_3);
        model = glm::translate(model, glm::vec3(-33, newStairHeight, -40.25));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-62.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair2.4
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-39.2,stairsMovementAmplitude,stairsMovementFrequency,startTime2_4);
        model = glm::translate(model, glm::vec3(-27.5, newStairHeight, -40.8f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair2.5
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-43.6,stairsMovementAmplitude,stairsMovementFrequency,startTime2_5);
        model = glm::translate(model, glm::vec3(-22.1f, newStairHeight, -38.99f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(-120.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //stair2.6
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        newStairHeight = calculateHeight(-48,stairsMovementAmplitude,stairsMovementFrequency,startTime2_6);
        model = glm::translate(model, glm::vec3(-18.5f, newStairHeight, -35.7f));
        model = glm::scale(model, glm::vec3(2.0f));
        model = glm::rotate(model, glm::radians(40.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisable(GL_CULL_FACE);

        // grass
        blendingShader.use();
        glm::mat4 grassM = glm::mat4(1.0f);
        blendingShader.setMat4("projection", projection);
        blendingShader.setMat4("view", view);
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        for (unsigned int i = 0; i < 1799; i++)
        {
            grassM = glm::mat4(1.0f);
            grassM = glm::scale(grassM, glm::vec3(6.0f));
            grassM = glm::translate(grassM, grassPositions1[i]);
            grassM = glm::rotate(grassM ,glm::radians(grassRotation1[i]), glm::vec3(0.0f ,1.0f, 0.0f));
            blendingShader.setMat4("model", grassM);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        for (unsigned int i = 0; i < 699; i++)
        {
            grassM = glm::mat4(1.0f);
            grassM = glm::scale(grassM, glm::vec3(6.0f));
            grassM = glm::translate(grassM, grassPositions2[i]);
            grassM = glm::rotate(grassM ,glm::radians(grassRotation2[i]), glm::vec3(0.0f ,1.0f, 0.0f));
            blendingShader.setMat4("model", grassM);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        for (unsigned int i = 0; i < 599; i++)
        {
            grassM = glm::mat4(1.0f);
            grassM = glm::scale(grassM, glm::vec3(6.0f));
            grassM = glm::translate(grassM, grassPositions3[i]);
            grassM = glm::rotate(grassM ,glm::radians(grassRotation3[i]), glm::vec3(0.0f ,1.0f, 0.0f));
            blendingShader.setMat4("model", grassM);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        for(unsigned int i = 0; i < 60; i++){
            grassM = glm::mat4(1.0f);
            grassM = glm::scale(grassM, glm::vec3(6.0f));
            grassM = glm::translate(grassM, grassPositions4[i]);
            grassM = glm::rotate(grassM ,glm::radians(grassRotation3[i]), glm::vec3(0.0f ,1.0f, 0.0f));
            blendingShader.setMat4("model", grassM);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        //skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        //skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (programState->ImGuiEnabled)
            DrawImGui(programState);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, 0.2);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, 0.2);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, 0.2);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, 0.2);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(UP, 0.18);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(DOWN, 0.18);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("mammoth1 position", (float*)&programState->mammoth1Position);
        ImGui::DragFloat("mammoth1 scale", &programState->mammoth1Scale, 0.05, 0.1, 4.0);
        ImGui::DragFloat("island scale", &programState->islandScale, 0.05, 0.1, 4.0);


        ImGui::DragFloat("pointLight.constant", &programState->pointLight1.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight1.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight1.quadratic, 0.05, 0.0, 1.0);
        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

float calculateHeight(float initialHeight, float amplitude, float frequency, std::chrono::steady_clock::time_point startTime) {
    auto currentTime = std::chrono::steady_clock::now(); // Get current time
    float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f; // Convert to seconds

    // Calculate vertical displacement using a sinusoidal function
    float verticalDisplacement = amplitude * sin(2 * M_PI * frequency * elapsedTime);

    // Calculate the new height of the model
    float newHeight = initialHeight + verticalDisplacement;

    return newHeight;
}

void resetTimer(std::string s) {
    if(s == "Fire") {
        startTimeFire = std::chrono::steady_clock::now();
    }else if(s == "Tree"){
        startTimeTree = std::chrono::steady_clock::now();
    }
}

double getElapsedTime(std::string s) {
    currentTime = std::chrono::steady_clock::now();
    if(s == "Fire") {
        return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTimeFire).count() / 1000.0;
    }else if(s == "Tree"){
        return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTimeTree).count() / 1000.0;
    }
}
