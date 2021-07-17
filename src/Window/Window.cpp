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
#include "../Model/Mesh.hpp"
#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"
#include "../Camera/Camera.h"
#include "../BlockRenderer/BlockRenderer.hpp"
#include "../State.hpp"
#include "../Controls/Controls.h"

#include "../../lib/imgui/imgui.h"
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_opengl3.h"
#include "../GUI/GUIRenderer.hpp"
#include "../Shadows/ShadowMap.hpp"

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
    //glEnable(GL_MULTISAMPLE);
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
    state->chunks = new ChunkManager(6, 1, 6);
    state->window = mainWindow;
    glfwSwapInterval(state->vsync);
    controls = new Controls(state);

    glfwSetWindowSizeCallback(mainWindow, resizeCallback);
    glfwSetKeyCallback(mainWindow, keyCallback);
    glfwSetMouseButtonCallback(mainWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(mainWindow, cursorCallback);

    toggleCursor(mainWindow);
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
        ss << "Mine++ " << " [" << fps << " FPS] [" << round((state->deltaTime * 1000) * 100) / 100 << " MS] [" << chunksOnSceneCounter << " CHUNKS]";
        glfwSetWindowTitle(pWindow, ss.str().c_str());
        state->nbFrames = 0;
        state->lastTime = currentTime;
    }
}

void generateNewChunksIfNeeded(int currentX, int currentZ)
{
    int newChunksCount = 0;
    for (int x_ = currentX - state->viewDistance; x_ <= currentX + state->viewDistance; x_++)
    {
        for (int z_ = currentZ - state->viewDistance; z_ <= currentZ + state->viewDistance; z_++)
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

void Window::startLoop()
{
    GUIRenderer gui(mainWindow);

    mat4 model(1.0f);
    state->camera = new Camera(vec3(0, 10, 0), radians(60.0f));
    Texture *texture_atlas = new Texture("res/textures/texture_atlas.png");
    texture_atlas->loadTexture();
    BlockRenderer chunkRenderer(1);

    for (auto &chunk : state->chunks->chunksDict)
    {
        Mesh *mesh = chunkRenderer.createMesh(chunk.second);
        chunk.second->mesh = mesh;
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Shader shader("vert", "frag");
    shader.bindAttribute(0, "position");
    shader.bindAttribute(1, "color");
    shader.bindAttribute(2, "texCoord");
    shader.bindAttribute(3, "normal");
    shader.link();
    shader.use();
    texture_atlas->bind();

    Mesh crosshairMesh;
    crosshairMesh.addVertexBufferObject({
                                                {0.0f, -0.02f, 0.0f},
                                                {0.0f, 0.02f, 0.0f},
                                                {-0.02f, 0.0f, 0.0f},
                                                {0.02f, 0.0f, 0.0f},
                                        });

    crosshairMesh.addIndices({0, 1, 2, 3});

    Shader crosshairShader("crosshairVert", "crosshairFrag");
    crosshairShader.bindAttribute(0, "position");
    crosshairShader.link();

    Shader simpleDepthShader("vertDepthShader", "fragDepthShader");
    simpleDepthShader.link();

    Shader debugQuad("vertDebugQuad", "fragDebugQuad");
    debugQuad.link();

    double lastTime = glfwGetTime();

    int chunksOnSceneCounter = 0;

    state->lightDir = {-0.2f, -1.0f,-0.2f};

    mat4 orthoMatrix = glm::ortho(ASPECT_RATIO, -ASPECT_RATIO, 1.0f, -1.0f, 1.0f, -1.0f);
    state->viewDistance = 6;

    ShadowMap shadowMap(state, 2048, 2048, 25);

    while (!glfwWindowShouldClose(mainWindow))
    {
        double currentTime = glfwGetTime();
        state->deltaTime = glfwGetTime() - lastTime;
        lastTime = currentTime;

        updateInputs(mainWindow);

        glClearColor(0.5, 0.8, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Chunk *chunk;
        int cx = state->camera->pos.x / CHUNK_SIZE;
        int cz = state->camera->pos.z / CHUNK_SIZE;

        generateNewChunksIfNeeded(cx, cz);

        showFPS(mainWindow, chunksOnSceneCounter);

        chunksOnSceneCounter = 0;

        for (auto &ichunk : state->chunks->chunksDict)
        {
            chunk = ichunk.second;
            if (!chunk->modified)
                continue;
            chunk->modified = false;
            chunk->mesh = chunkRenderer.createMesh(chunk);
        }

        auto mainCamPoints = state->camera->getFrustrumPoints(25);
        glm::mat4 lightView = shadowMap.calculateLightViewMatrix(mainCamPoints);
        glm::mat4 lightProjection = shadowMap.calculateLightProjectionMatrix(mainCamPoints, lightView);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        simpleDepthShader.use();
        simpleDepthShader.uniformMatrix(lightSpaceMatrix, "lightSpaceMatrix");

        glViewport(0, 0,  shadowMap.width, shadowMap.height);
        shadowMap.bindFBO();
        glClear(GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        glCullFace(GL_FRONT);
        chunkRenderer.render(*state->chunks, cx, cz, state->viewDistance, simpleDepthShader, chunksOnSceneCounter);

        chunksOnSceneCounter = 0;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, Window::_width, Window::_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (state->showPolygons) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // main render
        shader.use();
        shader.uniformMatrix(state->camera->getProjectionMatrix() * state->camera->getViewMatrix(), "projView");
        glUniform3f(glGetUniformLocation(shader.mProgram, "viewPos"), state->camera->pos.x, state->camera->pos.y, state->camera->pos.z);
        glUniform3f(glGetUniformLocation(shader.mProgram, "lightDir"), state->lightDir.x, state->lightDir.y, state->lightDir.z);
        glUniform1i(glGetUniformLocation(shader.mProgram, "useShadows"), state->useShadows);
        shader.uniformMatrix(lightSpaceMatrix, "lightSpaceMatrix");
        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        glActiveTexture(GL_TEXTURE1);
        shadowMap.bind();
        glCullFace(GL_BACK);
        chunkRenderer.render(*state->chunks, cx, cz, state->viewDistance, shader, chunksOnSceneCounter);

        // crosshair render
        crosshairShader.use();
        crosshairShader.uniformMatrix(mat4(1.0f), "model");
        crosshairShader.uniformMatrix(orthoMatrix, "projView");
        crosshairMesh.draw(GL_LINES);

        debugQuad.use();
        glActiveTexture(GL_TEXTURE0);
        shadowMap.bind();
        //renderQuad();

        gui.render(state);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(mainWindow);
}
