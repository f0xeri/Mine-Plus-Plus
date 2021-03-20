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
    state->chunks = new ChunkManager(6, 1, 6);
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

std::vector<glm::vec3> getFrustrumPoints(Camera *camera)
{
    float nearDist = 0.1f;
    float farDist = 50.0f;
    auto tanFOV = glm::tan(camera->FOV / 2);
    auto hNear = 2 * tanFOV * nearDist;
    auto wNear = hNear * ASPECT_RATIO;
    auto hFar = 2 * tanFOV * farDist;
    auto wFar = hFar * ASPECT_RATIO;

    glm::vec3 fc = camera->pos + camera->front * farDist;

    glm::vec3 ftl = fc + (camera->up * vec3(hFar) / vec3(2.0f)) - (camera->right * vec3(wFar) / vec3(2.0f));
    glm::vec3 ftr = fc + (camera->up * vec3(hFar) / vec3(2.0f)) + (camera->right * vec3(wFar) / vec3(2.0f));
    glm::vec3 fbl = fc - (camera->up * vec3(hFar) / vec3(2.0f)) - (camera->right * vec3(wFar) / vec3(2.0f));
    glm::vec3 fbr = fc - (camera->up * vec3(hFar) / vec3(2.0f)) + (camera->right * vec3(wFar) / vec3(2.0f));

    glm::vec3 nc = camera->pos + camera->front * nearDist;

    glm::vec3 ntl = nc + (camera->up * vec3(hNear) / vec3(2.0f)) - (camera->right * vec3(wNear) / vec3(2.0f));
    glm::vec3 ntr = nc + (camera->up * vec3(hNear) / vec3(2.0f)) + (camera->right * vec3(wNear) / vec3(2.0f));
    glm::vec3 nbl = nc - (camera->up * vec3(hNear) / vec3(2.0f)) - (camera->right * vec3(wNear) / vec3(2.0f));
    glm::vec3 nbr = nc - (camera->up * vec3(hNear) / vec3(2.0f)) + (camera->right * vec3(wNear) / vec3(2.0f));

    return std::vector{ftl, ftr, fbl, fbr, ntl, ntr, nbl, nbr};
}

void Window::startLoop()
{
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

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    double lastTime = glfwGetTime();

    int chunksOnSceneCounter = 0;

    vec3 lightDir = {state->camera->pos.x, 50, state->camera->pos.z};

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
        Mesh *mesh;
        int cx = state->camera->pos.x / CHUNK_SIZE;
        int cz = state->camera->pos.z / CHUNK_SIZE;

        lightDir = {-0.2f, -1.0f, -0.3f};
        generateNewChunksIfNeeded(cx, cz);

        showFPS(mainWindow, chunksOnSceneCounter);

        chunksOnSceneCounter = 0;

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

            mesh = chunkRenderer.createMesh(chunk);
            chunk->mesh = mesh;
            i++;
        }

        auto mainCamPoints = getFrustrumPoints(state->camera);
        glm::vec3 centroid(0.0f);
        for (auto p : mainCamPoints) {
            centroid += p;
        }
        centroid /= mainCamPoints.size();
        //LOG(centroid.x << " " << centroid.y << " " << centroid.z)
        float distFromCentroid = 10.0f;
        glm::vec3 lightCamPos = centroid - lightDir * distFromCentroid;
        //lightView = glm::lookAt(lightCamPos, centroid, glm::vec3(0.0, 1.0, 0.0));

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        //float near_plane = 1.0f, far_plane = 180.5f;
        //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        //lightView = glm::lookAt({0, 0, 0}, lightDir, glm::vec3(0.0, 1.0, 0.0));
        glm::vec3 min(10000);
        glm::vec3 max(-10000);
        lightView = glm::lookAt(lightCamPos, centroid, glm::vec3(0.0, 1.0, 0.0));

        glm::vec3 result(0.0f);
        for (auto p : mainCamPoints)
        {
            //result = vec4(p, 1.0f) * lightView;
            result = p;

            if(result.x < min.x)
                min.x = result.x;
            else if(result.x > max.x)
                max.x = result.x;

            if(result.y < min.y)
                min.y = result.y;
            else if(result.y > max.y)
                max.y = result.y;

            if(result.z < min.z)
                min.z = result.z;
            else if(result.z > max.z)
                max.z = result.z;
        }
        float xh = (glm::abs(max.x) + glm::abs(min.x)) * 0.5f;
        float yh = (glm::abs(max.y) + glm::abs(min.y)) * 0.5f;
        lightProjection = glm::ortho(-xh, xh, -yh, yh, -max.z, -min.z);
        //lightProjection = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);

        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.uniformMatrix(lightSpaceMatrix, "lightSpaceMatrix");

        glViewport(0, 0,  SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        chunkRenderer.render(*state->chunks, cx, cz, 6, simpleDepthShader, chunksOnSceneCounter);

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
        glUniform3f(glGetUniformLocation(shader.mProgram, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
        shader.uniformMatrix(lightSpaceMatrix, "lightSpaceMatrix");
        glActiveTexture(GL_TEXTURE0);
        texture_atlas->bind();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        chunkRenderer.render(*state->chunks, cx, cz, 6, shader, chunksOnSceneCounter);

        // crosshair render
        crosshairShader.use();
        crosshairShader.uniformMatrix(mat4(1.0f), "model");
        crosshairShader.uniformMatrix(orthoMatrix, "projView");
        crosshairMesh.draw(GL_LINES);

        debugQuad.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        //renderQuad();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI render
        if (state->showDebug)
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            //window_flags |= ImGuiWindowFlags_NoResize;
            ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
            ImGui::Begin("Debug", &state->showDebug, window_flags);
            std::stringstream debugSS;
            debugSS << "Global player position:\n" << "X: " << state->camera->pos.x << " Y: " << state->camera->pos.y << " Z: " << state->camera->pos.z << "\n\n";
            debugSS << "Current chunk position:\n" << "X: " << cx << " Z: " << cz << "\n\n";
            ImGui::Text("%s", debugSS.str().c_str());
            /*if (ImGui::Button("Close (F3)"))
                show_debug = false;*/
            std::stringstream resSS;

            resSS << "CAM FRUSTRUM POINTS\n";
            resSS << mainCamPoints[0].x << " " << mainCamPoints[0].y << " " << mainCamPoints[0].z << "\n";
            resSS << mainCamPoints[1].x << " " << mainCamPoints[1].y << " " << mainCamPoints[1].z << "\n";
            resSS << mainCamPoints[2].x << " " << mainCamPoints[2].y << " " << mainCamPoints[2].z << "\n";
            resSS << mainCamPoints[3].x << " " << mainCamPoints[3].y << " " << mainCamPoints[3].z << "\n\n";

            resSS << mainCamPoints[4].x << " " << mainCamPoints[4].y << " " << mainCamPoints[4].z << "\n";
            resSS << mainCamPoints[5].x << " " << mainCamPoints[5].y << " " << mainCamPoints[5].z << "\n";
            resSS << mainCamPoints[6].x << " " << mainCamPoints[6].y << " " << mainCamPoints[6].z << "\n";
            resSS << mainCamPoints[7].x << " " << mainCamPoints[7].y << " " << mainCamPoints[7].z << "\n\n";

            resSS << "CENTROID\n";
            resSS << centroid.x << " " << centroid.y << " " << centroid.z << "\n\n";
            resSS << "LIGHTCAM POS\n";
            resSS << lightCamPos.x << " " << lightCamPos.y << " " << lightCamPos.z << "\n\n";
            //resSS << -xh << " " <<  xh << " " <<  -yh << " " <<  yh << " " <<  -max.z << " " <<  -min.z << "\n\n";
            resSS << "MIN MAX LIGHTCAM\n";
            resSS << min.x << " " <<  min.y << " " <<  min.z << "\n";
            resSS << max.x << " " <<  max.y << " " <<  max.z << "\n\n\n";


            resSS << "LIGHT PROJECTION\n";
            for (int i = 0; i < 4; i++)
            {
                resSS << lightProjection[i][0] << " " << lightProjection[i][1] << " " << lightProjection[i][2] << " " << lightProjection[i][3] << "\n";
            }
            ImGui::Text("%s", resSS.str().c_str());
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
        ImGui::EndFrame();
        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(mainWindow);
}
