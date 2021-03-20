//
// Created by Yaroslav on 13.02.2021.
//

#ifndef MINE_STATE_HPP
#define MINE_STATE_HPP

#include "Camera/Camera.h"
#include "ChunkManager/ChunkManager.hpp"

struct State
{
    float deltaTime;
    float speed = 12.f;
    GLFWwindow *window;
    Camera *camera;
    ChunkManager *chunks;

    float deltaX = 0.0f;
    float deltaY = 0.0f;

    float x = 0.0f;
    float y = 0.0f;

    bool cursorLocked = false;
    bool cursorStarted = false;
    bool showDebug = false;
    bool showInventory = false;
    bool showPolygons = false;
    float camX = 0.0f;
    float camY = 0.0f;
    bool vsync = false;

    int nbFrames = 0;

    double lastTime;
    int currentBlockId = 0;
};

#endif //MINE_STATE_HPP
