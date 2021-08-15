//
// Created by Yaroslav on 28.07.2020.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <sstream>
#include <thread>
#include <fstream>
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
#include "../Player/Player.hpp"
#include "../PlayerRenderer/PlayerRenderer.hpp"
#include "../MineNetClient/MineNetClient.hpp"
#include "../Player/LocalPlayer.hpp"
#include "../Player/NetPlayer.hpp"

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

    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;

    // буфер позиций
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::_width, Window::_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // буфер нормалей
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::_width, Window::_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // буфер для цвета + коэффициента зеркального отражения
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window::_width, Window::_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // укажем OpenGL, какие буферы мы будем использовать при рендеринге
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::_width, Window::_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader gbufferShader("gbufferVert", "gbufferFrag");
    gbufferShader.link();

    Shader defferedLightningPassShader("defferedLightingVert", "defferedLightingFrag");
    defferedLightningPassShader.link();

    Shader crosshairShader("crosshairVert", "crosshairFrag");
    crosshairShader.bindAttribute(0, "position");
    crosshairShader.link();

    Shader simpleDepthShader("vertDepthShader", "fragDepthShader");
    simpleDepthShader.link();

    Shader debugQuad("vertDebugQuad", "fragDebugQuad");
    debugQuad.link();

    Shader playerShader("playerVert", "playerFrag");
    playerShader.link();

    double lastTime = glfwGetTime();

    int chunksOnSceneCounter = 0;

    state->lightDir = {-0.2f, -1.0f,-0.2f};

    mat4 orthoMatrix = glm::ortho(ASPECT_RATIO, -ASPECT_RATIO, 1.0f, -1.0f, 1.0f, -1.0f);
    state->viewDistance = 6;

    ShadowMap shadowMap(state, 2048, 2048, 25);
    LocalPlayer player(glm::vec3(0, 50, 0));
    //NetPlayer netPlayer(glm::vec3(2, 6, 2), state);
    //
    //PlayerRenderer netPlayerRenderer(&netPlayer);

    std::ifstream cfgFile("settings.cfg");
    std::string host = "127.0.0.1";
    if (cfgFile.is_open())
        std::getline(cfgFile, host);

    state->netClient = new MineNetClient();
    state->netClient->connect(host, 60000);

    std::unordered_map<uint32_t, Player*> players;
    std::unordered_map<uint32_t, PlayerRenderer*> playerRenderers;

    while (!glfwWindowShouldClose(mainWindow))
    {
        if (state->netClient->isConnected())
        {
            while (!state->netClient->incoming().empty())
            {
                auto msg = state->netClient->incoming().pop_front().msg;
                switch (msg.header.id)
                {
                    case MineMsgTypes::ClientAccepted:
                    {
                        LOG("[SERVER] Connected to server.");
                        MineNet::message<MineMsgTypes> msg;
                        msg.header.id = MineMsgTypes::ClientRegisterWithServer;
                        playerNetData data{};
                        data.posX = player.pos.x;
                        data.posY = player.pos.y;
                        data.posZ = player.pos.z;

                        msg << data;
                        state->netClient->send(msg);
                        break;
                    }

                    case MineMsgTypes::ClientAssignID:
                    {
                        msg >> player.id;
                        state->netClient->playerID = player.id;
                        LOG("[SERVER] Your ID is " << player.id << ".");
                        break;
                    }

                    case MineMsgTypes::WorldAddPlayer:
                    {
                        playerNetData data{};
                        msg >> data;
                        //LOG(data.id)
                        if (data.id == state->netClient->playerID)
                        {
                            players.insert_or_assign(data.id, new LocalPlayer(glm::vec3(data.posX, data.posY, data.posZ)));
                            //players[data.id] = new LocalPlayer(glm::vec3(data.posX, data.posY, data.posZ));
                            state->netClient->waitingForConnection = false;
                        }
                        else
                        {
                            players.insert_or_assign(data.id, new NetPlayer(glm::vec3(data.posX, data.posY, data.posZ)));
                        }
                        playerRenderers[data.id] = new PlayerRenderer(players[data.id]);
                        break;
                    }

                    case MineMsgTypes::WorldRemovePlayer:
                    {
                        uint32_t removalID = 0;
                        msg >> removalID;
                        players.erase(removalID);
                        playerRenderers.erase(removalID);
                        break;
                    }

                    case MineMsgTypes::WorldUpdatePlayer:
                    {
                        playerNetData data{};
                        msg >> data;
                        if (players.find(data.id) != players.end())
                            players[data.id]->pos = {data.posX, data.posY, data.posZ};
                        break;
                    }

                    case MineMsgTypes::WorldChunkModified:
                    {
                        ChunkModifyData data{};
                        msg >> data;
                        state->chunks->set(data.iendx + data.normx, data.iendy + data.normy, data.iendz + data.normz, data.blockId);
                        break;
                    }

                    case MineMsgTypes::WorldChanges:
                    {
                        ChunkChangesSave mapChangesData{};
                        msg >> mapChangesData;
                        if (state->chunks->chunksDict.count({mapChangesData.cx, mapChangesData.cz}) == 0)
                        {
                            state->chunks->add(mapChangesData.cx, mapChangesData.cy, mapChangesData.cz);
                        }
                        state->chunks->chunksDict.at({mapChangesData.cx, mapChangesData.cz})->blocks[mapChangesData.blockNumber] = mapChangesData.newBlockId;
                        state->chunks->chunksDict.at({mapChangesData.cx, mapChangesData.cz})->modified = true;
                        break;
                    }
                }
            }
        }
        else
        {
            if (!state->offlineSessionStarted)
            {
                state->netClient->waitingForConnection = false;
                players.insert_or_assign(0, new LocalPlayer(glm::vec3(0, 50, 0)));
                playerRenderers[0] = new PlayerRenderer(players[0]);
                state->offlineSessionStarted = true;
            }
        }

        if (state->netClient->waitingForConnection)
        {
            glClearColor(0.5, 0.8, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwSwapBuffers(mainWindow);
            glfwPollEvents();
            continue;
        }

        double currentTime = glfwGetTime();
        state->deltaTime = glfwGetTime() - lastTime;
        lastTime = currentTime;

        updateInputs(mainWindow, players[state->netClient->playerID]);
        //LOG(players.size());
        for (auto &p : players)
        {
            p.second->update(state->deltaTime, state);
        }

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

        // GEOMETRY PASS
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gbufferShader.use();
        gbufferShader.uniformMatrix(state->camera->getProjectionMatrix() * state->camera->getViewMatrix(), "projView");
        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        glCullFace(GL_BACK);
        chunkRenderer.render(*state->chunks, cx, cz, state->viewDistance, gbufferShader, chunksOnSceneCounter);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // SHADOW MAP PASS
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

        if (state->showPolygons) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // LIGHTING PASS
        glViewport(0, 0, Window::_width, Window::_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        defferedLightningPassShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glActiveTexture(GL_TEXTURE3);
        shadowMap.bind();

        glUniform3f(glGetUniformLocation(defferedLightningPassShader.mProgram, "lightDir"), state->lightDir.x, state->lightDir.y, state->lightDir.z);
        glUniform1i(glGetUniformLocation(defferedLightningPassShader.mProgram, "useShadows"), state->useShadows);
        defferedLightningPassShader.uniformMatrix(lightSpaceMatrix, "lightSpaceMatrix");
        defferedLightningPassShader.uniformMatrix(lightProjection, "lightProjection");
        //glCullFace(GL_BACK);
        //chunkRenderer.render(*state->chunks, cx, cz, state->viewDistance, defferedLightningPassShader, chunksOnSceneCounter);
        renderQuad();

        // main render
        /*shader.use();
        shader.uniformMatrix(state->camera->getProjectionMatrix() * state->camera->getViewMatrix(), "projView");
        glUniform3f(glGetUniformLocation(shader.mProgram, "viewPos"), state->camera->pos.x, state->camera->pos.y, state->camera->pos.z);
        glUniform3f(glGetUniformLocation(shader.mProgram, "lightDir"), state->lightDir.x, state->lightDir.y, state->lightDir.z);
        glUniform1i(glGetUniformLocation(shader.mProgram, "useShadows"), state->useShadows);
        shader.uniformMatrix(lightSpaceMatrix, "lightSpaceMatrix");
        shader.uniformMatrix(lightProjection, "lightProjection");
        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        glActiveTexture(GL_TEXTURE1);
        shadowMap.bind();
        glCullFace(GL_BACK);
        chunkRenderer.render(*state->chunks, cx, cz, state->viewDistance, shader, chunksOnSceneCounter);
        */
        //LOG("\n")

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, Window::_width, Window::_height, 0, 0, Window::_width, Window::_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (auto &playerRenderer : playerRenderers)
        {
            glm::vec3 translate = players[playerRenderer.first]->pos - players[playerRenderer.first]->spawnPos;
            model = glm::mat4(1.0f);
            model = glm::translate(model, translate);
            model = glm::rotate(model, players[playerRenderer.first]->rotY, glm::vec3(0, 1, 0));
            //std::cout << players[playerRenderer.first]->id << " " << playerRenderer.first << " " << players[playerRenderer.first]->rotY << "\n";
            playerShader.use();
            playerShader.uniformMatrix(state->camera->getProjectionMatrix() * state->camera->getViewMatrix(), "projView");
            playerShader.uniformMatrix(model, "model");
            if (playerRenderer.first != state->netClient->playerID)
                playerRenderer.second->render();
            else
            if (state->thirdPersonView)
                playerRenderer.second->render();

        }

        // crosshair render
        crosshairShader.use();
        crosshairShader.uniformMatrix(mat4(1.0f), "model");
        crosshairShader.uniformMatrix(orthoMatrix, "projView");
        crosshairMesh.draw(GL_LINES);


        /*debugQuad.use();
        debugQuad.uniformMatrix(state->camera->getProjectionMatrix() * state->camera->getViewMatrix(), "projView");
        model = glm::mat4(1.0f);
        debugQuad.uniformMatrix(model, "model");
        //shadowMap.bind();
        renderQuad();*/

        gui.render(state);

        playerNetData data{};
        MineNet::message<MineMsgTypes> msg;
        msg.header.id = MineMsgTypes::WorldUpdatePlayer;
        //LOG(players[state->netClient->playerID]->pos.y)
        data.posX = players[state->netClient->playerID]->pos.x;
        data.posY = players[state->netClient->playerID]->pos.y;
        data.posZ = players[state->netClient->playerID]->pos.z;
        data.rotY = players[state->netClient->playerID]->rotY;
        data.id = state->netClient->playerID;
        msg << data;
        state->netClient->send(msg);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(mainWindow);
}
