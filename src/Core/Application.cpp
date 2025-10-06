#include "Application.hpp"

#include <vector>
#include <chrono>

#include <imgui.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Logger.hpp"
#include "Vertex.hpp"

// Cube vertices
const std::vector<OpenGLUtils::Vertex_t> CUBE_VERTICES = {
    // Front face
    {{1, 1, 1},   {0, 0, 1}, {1, 1}},
    {{0, 1, 1},   {0, 0, 1}, {0, 1}},
    {{0, 0, 1},   {0, 0, 1}, {0, 0}},
    {{1, 0, 1},   {0, 0, 1}, {1, 0}},
    // Back face
    {{1, 1, 0},   {0, 0, -1}, {0, 1}},
    {{1, 0, 0},   {0, 0, -1}, {0, 0}},
    {{0, 0, 0},   {0, 0, -1}, {1, 0}},
    {{0, 1, 0},   {0, 0, -1}, {1, 1}},
    // Left face
    {{0, 1, 1},   {-1, 0, 0}, {1, 1}},
    {{0, 1, 0},   {-1, 0, 0}, {0, 1}},
    {{0, 0, 0},   {-1, 0, 0}, {0, 0}},
    {{0, 0, 1},   {-1, 0, 0}, {1, 0}},
    // Right face
    {{1, 1, 1},   {1, 0, 0}, {0, 1}},
    {{1, 0, 1},   {1, 0, 0}, {0, 0}},
    {{1, 0, 0},   {1, 0, 0}, {1, 0}},
    {{1, 1, 0},   {1, 0, 0}, {1, 1}},
    // Top face
    {{1, 1, 1},   {0, 1, 0}, {1, 0}},
    {{1, 1, 0},   {0, 1, 0}, {1, 1}},
    {{0, 1, 0},   {0, 1, 0}, {0, 1}},
    {{0, 1, 1},   {0, 1, 0}, {0, 0}},
    // Bottom face
    {{1, 0, 1},   {0, -1, 0}, {1, 0}},
    {{0, 0, 1},   {0, -1, 0}, {0, 0}},
    {{0, 0, 0},   {0, -1, 0}, {0, 1}},
    {{1, 0, 0},   {0, -1, 0}, {1, 1}},
};

// Cube indices
const std::vector<unsigned int> CUBE_INDICES = {
    // Front face
    0, 1, 2, 0, 2, 3,
    // Back face
    4, 5, 6, 4, 6, 7,
    // Left face
    8, 9,10, 8,10,11,
    // Right face
   12,13,14,12,14,15,
    // Top face
   16,17,18,16,18,19,
    // Bottom face
   20,21,22,20,22,23
};

const size_t WindowWidth = 1080;
const size_t WindowHeight = 720;
const std::string_view WindowName = "Gump";

Gump::Application::Application()
{
    try {
        LOG_DEBUG("Creating window ...");
        _window = std::make_unique<OpenGLUtils::Window>(WindowWidth, WindowHeight, std::string(WindowName));
        LOG_DEBUG("Loading assets ...");
        _demoCube = std::make_unique<OpenGLUtils::Mesh>(CUBE_VERTICES, CUBE_INDICES);
        _demoLight = std::make_unique<OpenGLUtils::Mesh>(CUBE_VERTICES, CUBE_INDICES);
        _demoCubeShader = std::make_unique<OpenGLUtils::Shader>("../shaders/lightning.vert", "../shaders/lightning.frag");
        _demoLightShader = std::make_unique<OpenGLUtils::Shader>("../shaders/cube.vert", "../shaders/cube.frag");
    } catch (std::exception e) {
        LOG_ERROR("Could not create window: {}", e.what());
        throw std::runtime_error("Could not create window");
    }
}

void Gump::Application::run()
{
    while (_running) {
        processInput();
        update();

        _window->beginFrame();
        render();
        _window->endFrame();

        if (_window->shouldClose()) _running = false;
    }
}

void Gump::Application::stop()
{
    _running = false;
}

void Gump::Application::processInput()
{

}

void Gump::Application::update()
{
    
}

void Gump::Application::render()
{
    static ImVec4 demoCubeColor = {1, 1, 1, 1};
    static ImVec4 demoLightColor = {1, 1, 1, 1};
    static float specStrenght = 0.5f;
    static float shininess = 32;
    static bool cubeRotates = true;
    static bool lightMoves = true;
    static glm::vec3 lightPos;
    static float angle;

    auto now = std::chrono::steady_clock::now();
    float time = std::chrono::duration<float>(now.time_since_epoch()).count();
    if (cubeRotates)
        angle = glm::radians(time * 10.0f);

    // Position cube
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, glm::vec3(0.5f, 0.5f, 0.5f));
    cubeModel = glm::rotate(cubeModel, angle, glm::vec3(1.0f, 0.1f, 0.5f));
    cubeModel = glm::translate(cubeModel, glm::vec3(-0.5f, -0.5f, -0.5f));


    // Position light cube
    // Light cube orbit parameters
    float orbitRadius = 2.0f;
    float orbitSpeed = glm::radians(60.0f); // degrees per second
    float orbitAngle = time * orbitSpeed;

    // Compute orbit position around cube center (0.5,0.5,0.5)
    if (lightMoves) {
        glm::vec3 orbitCenter(0.5f, 0.5f, 0.5f);
        lightPos = orbitCenter + glm::vec3(
            orbitRadius * std::cos(orbitAngle),
            0.25,  // keep some height above the cube
            orbitRadius * std::sin(orbitAngle)
        );
    }

    // Light cube model
    
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f)); 

    
    // View and projection matrices
    glm::mat4 view = glm::lookAt(
        glm::vec3(3.0f, 3.0f, 6.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight),
        0.1f,
        100.0f
    );

    {
        ImGui::Begin("Gump menu");

        ImGui::Text("Gump menu !");
        ImGui::ColorEdit3("Change cube1 color", (float *)(&demoCubeColor));
        ImGui::ColorEdit4("Change scene light", (float *)(&demoLightColor));
        ImGui::SliderFloat("Shininess", &shininess, 0, 256);
        ImGui::SliderFloat("Specular strenght", &specStrenght, 0, 1);
        ImGui::Checkbox("Automatic cube rotation", &cubeRotates);
        ImGui::Checkbox("Automatic light movement", &lightMoves);
        ImGui::SliderFloat("Light position X", &lightPos.x, -3, 3);
        ImGui::SliderFloat("Light position Y", &lightPos.y, -3, 3);
        ImGui::SliderFloat("Light position Z", &lightPos.z, -3, 3);

        ImGui::End();
    }

    // Pass matrices to shader (assuming uniform names "model", "view", "projection")
    _demoCubeShader->use();
    _demoCubeShader->set("uModel", cubeModel);
    _demoCubeShader->set("uView", view);
    _demoCubeShader->set("uProjection", projection);
    _demoCubeShader->set("uCubeColor", glm::vec3(demoCubeColor.x, demoCubeColor.y, demoCubeColor.z));
    _demoCubeShader->set("uLightColor", glm::vec3(demoLightColor.x, demoLightColor.y, demoLightColor.z));
    _demoCubeShader->set("uLightPos", lightPos);
    _demoCubeShader->set("uViewPos", lightPos);
    _demoCubeShader->set("uShininess", shininess);
    _demoCubeShader->set("uSpecStrenght", specStrenght);

    _demoCube->draw();

    _demoLightShader->use();
    _demoLightShader->set("uModel", lightModel);
    _demoLightShader->set("uView", view);
    _demoLightShader->set("uProjection", projection);
    _demoLightShader->set("uCubeColor", glm::vec3(demoLightColor.x, demoLightColor.y, demoLightColor.z));

    _demoLight->draw();
}
