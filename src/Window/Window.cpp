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
#include "../Voxels/Chunk.hpp"
#include "../BlockRenderer/BlockRenderer.hpp"
#include "../ChunkManager/ChunkManager.hpp"

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

float camX = 0.0f;
float camY = 0.0f;

bool vsync = false;

int nbFrames = 0;

double lastTime;

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
        chunks->saveMap();
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        chunks->loadMap();
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
            chunks->set(int(iend.x) + int(norm.x), int(iend.y) + int(norm.y), int(iend.z) + int(norm.z), 2);
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

void Window::startLoop()
{
    mat4 model(1.0f);
    camera = new Camera(vec3(10, 10, 10), radians(60.0f));
    Texture *texture_atlas = new Texture("res/textures/texture_atlas.png");
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
    Chunk* closes[27];
    double lastTime = glfwGetTime();

    int chunksOnSceneCounter = 0;
    while (!glfwWindowShouldClose(mainWindow))
    {
        Chunk *chunk;
        nModel::Model *mesh;
        int cx = camera->pos.x / CHUNK_SIZE;
        int cz = camera->pos.z / CHUNK_SIZE;
        vec3 lightPos = {(chunks->w * 16) / 2, 300, (chunks->d * 16) / 2};
        shader.use();
        showFPS(mainWindow, chunksOnSceneCounter);
        int newChunksCount = 0;
        for (int x_ = cx - 12; x_ <= cx + 12; x_++)
        {
            for (int z_ = cz - 12; z_ <= cz + 12; z_++)
            {
                if (chunks->chunksDict.find({x_, z_}) == chunks->chunksDict.end())
                {
                    chunks->add(x_, 0, z_);
                    newChunksCount++;
                }
            }
        }
        float currentTime = glfwGetTime();
        deltaTime = glfwGetTime() - lastTime;
        lastTime = currentTime;
        glClearColor(0.5, 0.8, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //shader.uniformMatrix(model, "model");
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
                chunk = chunks->chunksDict[{x_, z_}];
                mesh = chunk->mesh;
                model = mat4(1.0f);
                model = translate(model, vec3(chunk->x * CHUNK_SIZE, chunk->y * CHUNK_SIZE, chunk->z * CHUNK_SIZE));
                shader.uniformMatrix(model, "model");
                mesh->draw();
                chunksOnSceneCounter++;
            }
        }
        /*for (auto &chunk_ : chunks->chunksDict)
        {
            chunk = chunk_.second;
            if (cx - 12 < chunk->x <= cx + 12 && cz - 12 < chunk->z <= cz + 12)
            //if (cx - 12 < chunk->x <= cx + 12 && cz - 12 < chunk->z <= cz + 12)
            {
                mesh = chunk->mesh;
                //LOG("[INFO] Chunk found in (" << x_ << "; " << z_ << ")");
                model = mat4(1.0f);
                model = translate(model, vec3(chunk->x * CHUNK_SIZE, chunk->y * CHUNK_SIZE, chunk->z * CHUNK_SIZE));
                shader.uniformMatrix(model, "model");
                mesh->draw();
            }
        }*/
        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(mainWindow);
}
