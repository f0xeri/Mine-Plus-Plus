//
// Created by Yaroslav on 28.07.2020.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <sstream>
#include <thread>
#include "Window.hpp"
#include "../Logger.hpp"
#include "../Model/Model.hpp"
#include "../Program/Program.hpp"
#include "../Texture/Texture.hpp"
#include "../Camera/Camera.h"
#include "../Voxels/Chunk.hpp"
#include "../BlockRenderer/BlockRenderer.hpp"
#include "../ChunkManager/ChunkManager.hpp"

#include "../../lib/imgui/imgui.h"
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_opengl3.h"


#define ASPECT_RATIO float(Window::_width) / float(Window::_height)

using namespace glm;

ChunkManager *chunks = new ChunkManager(12, 1, 12);

int Window::_height = 0;
int Window::_width = 0;

float deltaTime;
float speed = 12.f;
Camera *camera;

float deltaX = 0.0f;
float deltaY = 0.0f;

float x = 0.0f;
float y = 0.0f;

bool cursor_locked = false;
bool cursor_started = false;
bool show_debug = false;
bool show_inventory = false;

float camX = 0.0f;
float camY = 0.0f;

bool vsync = false;

int nbFrames = 0;

double lastTime;
int currentBlockId = 0;

void toggleCursor(GLFWwindow *window)
{
    cursor_locked = !cursor_locked;
    glfwSetInputMode(window, GLFW_CURSOR, cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        toggleCursor(window);
    }
    if (key == GLFW_KEY_SPACE)
        switch (action)
        {
            case GLFW_PRESS:
            {
                speed = speed * 2;
                break;
            }
            case GLFW_RELEASE:
            {
                speed = speed / 2;
                break;
            }
        }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        for (auto &chunk : chunks->chunksDict)
            std::cout << chunk.second->x << " " << chunk.second->z << std::endl;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        vsync = !vsync;
        glfwSwapInterval(vsync);
        LOG("[INFO] VSync " << (vsync ? "on" : "off") << ".");
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        std::thread saveThread(&ChunkManager::saveMap, chunks);
        saveThread.detach();
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        std::thread loadThread(&ChunkManager::loadMap, chunks);
        loadThread.detach();
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
    {
        show_debug = !show_debug;
        show_inventory = !show_inventory;
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        currentBlockId = 1;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        currentBlockId = 2;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        currentBlockId = 3;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        currentBlockId = 6;
    }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        currentBlockId = 0;
    }
}

void updateInputs(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->pos += camera->front * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->pos -= camera->right * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->pos -= camera->front * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->pos += camera->right * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera->pos += camera->up * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->pos -= camera->up * speed * deltaTime;
}

void cursorCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (cursor_started){
        deltaX += xpos - x;
        deltaY += ypos - y;
    }
    else cursor_started = true;
    x = xpos;
    y = ypos;
    if (cursor_locked)
    {
        camY += -deltaY / Window::_height * 2;
        camX += -deltaX / Window::_height * 2;

        if (camY < -radians(89.0f)){
            camY = -radians(89.0f);
        }
        if (camY > radians(89.0f)){
            camY = radians(89.0f);
        }

        camera->rotation = mat4(1.0f);
        camera->rotate(camY, camX, 0);
    }
    deltaX = deltaY = 0.0f;
}

void resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    Window::_width = width;
    Window::_height = height;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    vec3 end;
    vec3 norm;
    vec3 iend;
    block *block = chunks->rayCast(camera->pos, camera->front, 10.0f, end, norm, iend);
    //vec3 dir = chunks->rayCast2(camera);
    if (block != nullptr)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            chunks->set(int(iend.x), int(iend.y), int(iend.z), 0);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            chunks->set(int(iend.x) + int(norm.x), int(iend.y) + int(norm.y), int(iend.z) + int(norm.z), currentBlockId);
        }
    }
}

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
    glfwSwapInterval(vsync);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        LOG("[ERROR] Failed to init GLEW!");
        throw std::runtime_error("GLEW init failed.");
    }
    else LOG("[INFO] GLEW inited.");
    Window::_width = width;
    Window::_height = height;
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
    LOG("[INFO] Window closed.");
}

void showFPS(GLFWwindow *pWindow, int chunksOnSceneCounter)
{
    double currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    nbFrames++;
    if ( delta >= 1.0 )
    {
        double fps = double(nbFrames) / delta;
        std::stringstream ss;
        ss << "Mine++ " << " [" << fps << " FPS] [" << chunksOnSceneCounter << " CHUNKS]";
        glfwSetWindowTitle(pWindow, ss.str().c_str());
        nbFrames = 0;
        lastTime = currentTime;
    }
}

void generateNewChunksIfNeeded(int currentX, int currentZ)
{
    int newChunksCount = 0;
    for (int x_ = currentX - 12; x_ <= currentX + 12; x_++)
    {
        for (int z_ = currentZ - 12; z_ <= currentZ + 12; z_++)
        {
            if (chunks->chunksDict.find({x_, z_}) == chunks->chunksDict.end())
            {
                chunks->add(x_, 0, z_);
                newChunksCount++;
                //lightPos = {camera->pos.x + 50, 150, camera->pos.z + 30};
            }
        }
    }
}


void Window::startLoop()
{
    mat4 model(1.0f);
    camera = new Camera(vec3(0, 10, 0), radians(60.0f));
    Texture *texture_atlas = new Texture("res/textures/texture_atlas.png");
    texture_atlas->loadTexture();
    BlockRenderer newChunkRenderer(1);

    for (auto &chunk : chunks->chunksDict)
    {
        nModel::Model *mesh = newChunkRenderer.render(chunk.second, nullptr);
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

    vec3 lightPos = {camera->pos.x, 150, camera->pos.z};

    mat4 orthoMatrix = glm::ortho(ASPECT_RATIO, -ASPECT_RATIO, 1.0f, -1.0f, 1.0f, -1.0f);

    while (!glfwWindowShouldClose(mainWindow))
    {
        glClearColor(0.5, 0.8, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Chunk *chunk;
        nModel::Model *mesh;
        int cx = camera->pos.x / CHUNK_SIZE;
        int cz = camera->pos.z / CHUNK_SIZE;

        lightPos = {camera->pos.x, 150, camera->pos.z};
        generateNewChunksIfNeeded(cx, cz);

        shader.use();
        showFPS(mainWindow, chunksOnSceneCounter);

        double currentTime = glfwGetTime();
        deltaTime = glfwGetTime() - lastTime;
        lastTime = currentTime;

        shader.uniformMatrix(camera->getProjectionMatrix() * camera->getViewMatrix(), "projView");
        GLint lightPosLoc = glGetUniformLocation(shader.mProgram, "lightPos");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        updateInputs(mainWindow);
        int i = 0;
        int count = 0;
        for (auto &ichunk : chunks->chunksDict)
        {
            Chunk* chunk = ichunk.second;
            if (!chunk->modified)
                continue;
            chunk->modified = false;
            count++;
            delete chunk->mesh;

            mesh = newChunkRenderer.render(chunk, (const Chunk **) closes);
            chunk->mesh = mesh;
            i++;
        }
        chunksOnSceneCounter = 0;
        for (int x_ = cx - 12; x_ <= cx + 12; x_++)
        {
            for (int z_ = cz - 12; z_ <= cz + 12; z_++)
            {
                chunk = chunks->chunksDict.at({x_, z_});
                mesh = chunk->mesh;
                model = mat4(1.0f);
                model = translate(model, vec3(chunk->x * CHUNK_SIZE, chunk->y * CHUNK_SIZE, chunk->z * CHUNK_SIZE));
                shader.uniformMatrix(model, "model");
                mesh->draw();
                chunksOnSceneCounter++;
            }
        }

        crosshairShader.use();
        crosshairShader.uniformMatrix(mat4(1.0f), "model");
        crosshairShader.uniformMatrix(orthoMatrix, "projView");
        crosshairMesh.draw(GL_LINES);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_debug)
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
            ImGui::Begin("Debug", &show_debug, window_flags); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            std::stringstream debugSS;
            debugSS << "Global player position:\n" << "X: " << camera->pos.x << " Y: " << camera->pos.y << " Z: " << camera->pos.z << "\n\n";
            debugSS << "Current chunk position:\n" << "X: " << cx << " Z: " << cz << "\n\n";
            ImGui::Text("%s", debugSS.str().c_str());
            /*if (ImGui::Button("Close (F3)"))
                show_debug = false;*/
            ImGui::End();
        }

        if (show_inventory)
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            ImGui::SetNextWindowPos({Window::_width / 2.0f - 80, Window::_height - (Window::_height / 12.0f)});
            ImGui::SetNextWindowSize({160, 60}, ImGuiCond_Once);
            ImGui::Begin("Inventory", &show_debug, window_flags); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            std::stringstream ss;
            switch (currentBlockId) {
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

        if (show_debug || show_inventory)
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
