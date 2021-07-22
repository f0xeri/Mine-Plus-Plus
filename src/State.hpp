//
// Created by Yaroslav on 13.02.2021.
//

#ifndef MINE_STATE_HPP
#define MINE_STATE_HPP

#include "Camera/Camera.h"
#include "ChunkManager/ChunkManager.hpp"
#include "MineNetClient/MineNetClient.hpp"

struct State
{
    float deltaTime;
    float speed = 8.f;

    float upSpeed;
    bool inAir;

    glm::vec3 velocity;

    GLFWwindow *window;
    Camera *camera;
    ChunkManager *chunks;
    MineNetClient *netClient;

    float deltaX = 0.0f;
    float deltaY = 0.0f;

    float x = 0.0f;
    float y = 0.0f;

    glm::vec3 lightDir;
    glm::vec3 calculatedLightPosition;
    int viewDistance;

    bool cursorLocked = false;
    bool cursorStarted = false;
    bool showDebug = false;
    bool showInventory = false;
    bool showPolygons = false;
    bool physicsEnabled = true;
    bool freeFlightMode = false;
    bool useShadows = true;
    bool thirdPersonView = false;
    bool offlineSessionStarted = false;
    float camX = 0.0f;
    float camY = 0.0f;
    bool vsync = false;

    int nbFrames = 0;

    double lastTime;
    int currentBlockId = 0;
};

struct GuiState
{

};

#endif //MINE_STATE_HPP
