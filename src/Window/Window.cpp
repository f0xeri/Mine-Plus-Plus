//
// Created by Yaroslav on 28.07.2020.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <sstream>
#include "Window.hpp"
#include "../Logger.hpp"
#include "../Model/Model.hpp"
#include "../Program/Program.hpp"
#include "../Texture/Texture.hpp"
#include "../Camera/Camera.h"
#include "../BlockRenderer/BlockRenderer.hpp"
#include "../State.hpp"
#include "../Controls/Controls.h"

#include "../../lib/imgui/imgui.h"
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_opengl3.h"

#define ASPECT_RATIO float(Window::_width) / float(Window::_height)

using namespace glm;

int Window::_height = 0;
int Window::_width = 0;
State *state;
Controls *controls;


Window::Window(const char *title, int width, int height)
{
    if (!glfwInit())
    {
        LOG("[ERROR] Failed to init GLFW!");
        throw std::runtime_error("GLFW init failed.");
    }
    else LOG("[INFO] GLFW inited.");
    //glfwWindowHint(GLFW_SAMPLES, 8);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_MULTISAMPLE);
    mainWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (mainWindow)
    {
        LOG("[INFO] Window opened.");
    }
    else
    {
        LOG("[ERROR] Window open failed!");
        throw std::runtime_error("Window init failed.");
    }
    makeContextCurrent();

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        LOG("[ERROR] Failed to init GLEW!");
        throw std::runtime_error("GLEW init failed.");
    }
    else LOG("[INFO] GLEW inited.");
    Window::_width = width;
    Window::_height = height;

    state = new State();
    state->camera = new Camera({0, 10, 0}, 60.0f);
    state->chunks = new ChunkManager(12, 1, 12);
    state->window = mainWindow;
    glfwSwapInterval(state->vsync);
    controls = new Controls(state);

    glfwSetWindowSizeCallback(mainWindow, resizeCallback);
    glfwSetKeyCallback(mainWindow, keyCallback);
    glfwSetMouseButtonCallback(mainWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(mainWindow, cursorCallback);

    toggleCursor(mainWindow);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    const char* glsl_version = "#version 430";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    LOG("[INFO] Window closed.");
}

void showFPS(GLFWwindow *pWindow, int chunksOnSceneCounter)
{
    double currentTime = glfwGetTime();
    double delta = currentTime - state->lastTime;
    state->nbFrames++;
    if ( delta >= 1.0 )
    {
        double fps = double(state->nbFrames) / delta;
        std::stringstream ss;
        ss << "Mine++ " << " [" << fps << " FPS] [" << chunksOnSceneCounter << " CHUNKS]";
        glfwSetWindowTitle(pWindow, ss.str().c_str());
        state->nbFrames = 0;
        state->lastTime = currentTime;
    }
}

void generateNewChunksIfNeeded(int currentX, int currentZ)
{
    int newChunksCount = 0;
    for (int x_ = currentX - 12; x_ <= currentX + 12; x_++)
    {
        for (int z_ = currentZ - 12; z_ <= currentZ + 12; z_++)
        {
            if (state->chunks->chunksDict.find({x_, z_}) == state->chunks->chunksDict.end())
            {
                state->chunks->add(x_, 0, z_);
                newChunksCount++;
            }
        }
    }
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

int renderScene(nModel::Program &shader, Texture *texture_atlas, int cx, int cz)
{
    mat4 model = mat4(1.0f);
    Chunk *chunk;
    nModel::Model *mesh;
    int chunksOnSceneCounter = 0;
    for (int x_ = cx - 12; x_ <= cx + 12; x_++)
    {
        for (int z_ = cz - 12; z_ <= cz + 12; z_++)
        {
            chunk = state->chunks->chunksDict.at({x_, z_});
            mesh = chunk->mesh;
            model = mat4(1.0f);
            model = translate(model, vec3(chunk->x * CHUNK_SIZE, chunk->y * CHUNK_SIZE, chunk->z * CHUNK_SIZE));
            shader.uniformMatrix(model, "model");
            mesh->draw();
            chunksOnSceneCounter++;
        }
    }
    return chunksOnSceneCounter;
}

void Window::startLoop()
{
    mat4 model(1.0f);
    state->camera = new Camera(vec3(0, 10, 0), radians(60.0f));
    Texture *texture_atlas = new Texture("res/textures/texture_atlas.png");
    texture_atlas->loadTexture();
    BlockRenderer newChunkRenderer(1);

    for (auto &chunk : state->chunks->chunksDict)
    {
        nModel::Model *mesh = newChunkRenderer.createMesh(chunk.second, nullptr);
        chunk.second->mesh = mesh;
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    nModel::Program shader("vert", "frag");
    shader.bindAttribute(0, "position");
    shader.bindAttribute(1, "color");
    shader.bindAttribute(2, "texCoord");
    shader.bindAttribute(3, "normal");
    shader.link();
    shader.use();
    texture_atlas->bind();

    nModel::Model crosshairMesh;
    crosshairMesh.addVertexBufferObject({
                                                {0.0f, -0.02f, 0.0f},
                                                {0.0f, 0.02f, 0.0f},
                                                {-0.02f, 0.0f, 0.0f},
                                                {0.02f, 0.0f, 0.0f},
                                        });

    crosshairMesh.addIndices({0, 1, 2, 3});

    nModel::Program crosshairShader("crosshairVert", "crosshairFrag");
    crosshairShader.bindAttribute(0, "position");
    crosshairShader.link();

    Chunk* closes[27];
    double lastTime = glfwGetTime();

    int chunksOnSceneCounter = 0;

    vec3 lightPos = {state->camera->pos.x, 50, state->camera->pos.z};

    nModel::Program simpleDepthShader("vertDepthShader", "fragDepthShader");
    simpleDepthShader.link();

    nModel::Program debugQuad("vertDebugQuad", "fragDebugQuad");
    debugQuad.link();

    mat4 orthoMatrix = glm::ortho(ASPECT_RATIO, -ASPECT_RATIO, 1.0f, -1.0f, 1.0f, -1.0f);

    while (!glfwWindowShouldClose(mainWindow))
    {
        double currentTime = glfwGetTime();
        state->deltaTime = glfwGetTime() - lastTime;
        lastTime = currentTime;

        updateInputs(mainWindow);

        glClearColor(0.5, 0.8, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Chunk *chunk;
        nModel::Model *mesh;
        int cx = state->camera->pos.x / CHUNK_SIZE;
        int cz = state->camera->pos.z / CHUNK_SIZE;

        lightPos = {state->camera->pos.x, 50, state->camera->pos.z};
        generateNewChunksIfNeeded(cx, cz);

        showFPS(mainWindow, chunksOnSceneCounter);

        int i = 0;
        int count = 0;
        for (auto &ichunk : state->chunks->chunksDict)
        {
            chunk = ichunk.second;
            if (!chunk->modified)
                continue;
            chunk->modified = false;
            count++;
            delete chunk->mesh;

            mesh = newChunkRenderer.createMesh(chunk, (const Chunk **) closes);
            chunk->mesh = mesh;
            i++;
        }

        // reset viewport
        glViewport(0, 0, Window::_width, Window::_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (state->showPolygons) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // main render
        shader.use();
        shader.uniformMatrix(state->camera->getProjectionMatrix() * state->camera->getViewMatrix(), "projView");
        glUniform3f(glGetUniformLocation(shader.mProgram, "viewPos"), state->camera->pos.x, state->camera->pos.y, state->camera->pos.z);
        glUniform3f(glGetUniformLocation(shader.mProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        chunksOnSceneCounter = renderScene(shader, texture_atlas, cx, cz);

        crosshairShader.use();
        crosshairShader.uniformMatrix(mat4(1.0f), "model");
        crosshairShader.uniformMatrix(orthoMatrix, "projView");
        crosshairMesh.draw(GL_LINES);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (state->showDebug)
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
            ImGui::Begin("Debug", &state->showDebug, window_flags);
            std::stringstream debugSS;
            debugSS << "Global player position:\n" << "X: " << state->camera->pos.x << " Y: " << state->camera->pos.y << " Z: " << state->camera->pos.z << "\n\n";
            debugSS << "Current chunk position:\n" << "X: " << cx << " Z: " << cz << "\n\n";
            ImGui::Text("%s", debugSS.str().c_str());
            /*if (ImGui::Button("Close (F3)"))
                show_debug = false;*/
            ImGui::End();
        }

        if (state->showInventory)
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            ImGui::SetNextWindowPos({Window::_width / 2.0f - 80, Window::_height - (Window::_height / 12.0f)});
            ImGui::SetNextWindowSize({160, 60}, ImGuiCond_Once);
            ImGui::Begin("Inventory", &state->showInventory, window_flags);
            std::stringstream ss;
            switch (state->currentBlockId) {
                case 0: ss << "Air"; break;
                case 1: ss << "Stone"; break;
                case 2: ss << "Sand"; break;
                case 3: ss << "Grass"; break;
                case 6: ss << "Bricks"; break;
            }
            ImGui::SetWindowFontScale(1.8f);
            float font_size = ImGui::GetFontSize() * ss.str().size() / 2;
            ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
            ImGui::Text("%s", ss.str().c_str());
            ImGui::End();
        }

        if (state->showDebug || state->showInventory)
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(mainWindow);
}
