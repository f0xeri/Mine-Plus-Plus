//
// Created by Yaroslav on 10.02.2021.
//

#include <thread>
#include <iostream>
#include "Controls.h"
#include "../Window/Window.hpp"
#include "../Logger.hpp"

State *localState;

Controls::Controls(State *s) {
    localState = s;
}

void toggleCursor(GLFWwindow *window)
{
    localState->cursorLocked = !localState->cursorLocked;
    glfwSetInputMode(window, GLFW_CURSOR, localState->cursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        toggleCursor(window);
    }
    if (key == GLFW_KEY_TAB)
        switch (action)
        {
            case GLFW_PRESS:
            {
                localState->freeFlightMode = !localState->freeFlightMode;
                localState->speed = 8.0f * (localState->freeFlightMode + 1);
                break;
            }
            /*case GLFW_RELEASE:
            {
                localState->speed = localState->speed / 2;
                localState->freeFlightMode = false;
                break;
            }*/
        }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        for (auto &chunk : localState->chunks->chunksDict)
            std::cout << chunk.second->x << " " << chunk.second->z << std::endl;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        localState->vsync = !localState->vsync;
        glfwSwapInterval(localState->vsync);
        //LOG("[INFO] VSync " << (localState->vsync ? "on" : "off") << ".");
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        std::thread saveThread(&ChunkManager::saveMap, localState->chunks);
        saveThread.detach();
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        std::thread loadThread(&ChunkManager::loadMap, localState->chunks);
        loadThread.detach();
    }
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
    {
        localState->showPolygons = !localState->showPolygons;
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
    {
        localState->showDebug = !localState->showDebug;
        localState->showInventory = !localState->showInventory;
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 1;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 2;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 3;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 6;
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 7;
    }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 0;
    }
}

bool testHitbox(glm::vec3 center, glm::vec3 dimensions) {

    int bounds[6];
    for (int i = 0; i < 3; i++) {
        int low = std::floor(center[i] - (dimensions[i] / 2.0f));
        int high = std::floor(center[i] + (dimensions[i] / 2.0f));
        bounds[i * 2] = low;
        bounds[i * 2 + 1] = high;
    }

    for (int x = bounds[0]; x <= bounds[1]; x++) {
        for (int y = bounds[2]; y <= bounds[3]; y++) {
            for (int z = bounds[4]; z <= bounds[5]; z++) {
                auto block = localState->chunks->get(x, y, z);
                if (block != nullptr)
                {
                    if (block->id != 0) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

glm::vec3 goodHitboxCenter(glm::vec3 currentCenter, glm::vec3 dimensions, glm::vec3 motionVec, glm::vec3 lookDirection) {

    glm::vec3 result = currentCenter;

    bool flipHorizPrecedence = false;
    if (abs(lookDirection.y) > abs(lookDirection.x)) {
        flipHorizPrecedence = true;
    }

    for (int index = 0; index < 3; index++) {
        int i = index;
        if (flipHorizPrecedence && i == 0) {
            i = 1;
        }
        else if (flipHorizPrecedence && i == 1) {
            i = 0;
        }
        if (motionVec[i] != 0.0f) {
            glm::vec3 singleAxisMotion = glm::vec3();
            singleAxisMotion[i] = motionVec[i];
            if (testHitbox(result + singleAxisMotion, dimensions)) {
                result[i] += motionVec[i];
            }
            else if (abs(motionVec[i]) < 1.0f) {
                //block below
                if (motionVec[i] < 0.0f) {
                    int bound = std::floor(result[i] + motionVec[i] - (dimensions[i] / 2.0f));
                    result[i] = (bound + 1) + dimensions[i] / 2.0f;
                }
                    //block above
                else {
                    int bound = std::floor(result[i] + motionVec[i] + (dimensions[i] / 2.0f));
                    result[i] = (bound) - dimensions[i] / 2.0f - 0.001f;
                }
            }
        }
    }

    return result;
}

const glm::vec3 playerDimensions = glm::vec3(1.00f, 2.0f, 1.00f);
const float playerHeadOffset = 1.0f;

void updateInputs(GLFWwindow *window)
{
    glm::vec3 moveVector(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveVector.x -= glm::sin(localState->camera->rotY) * localState->speed * localState->deltaTime;
        moveVector.z -= glm::cos(localState->camera->rotY) * localState->speed * localState->deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveVector -= localState->camera->right * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveVector.x += glm::sin(localState->camera->rotY) * localState->speed * localState->deltaTime;
        moveVector.z += glm::cos(localState->camera->rotY) * localState->speed * localState->deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveVector += localState->camera->right * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && localState->freeFlightMode)
        moveVector += localState->camera->up * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && localState->freeFlightMode)
        moveVector -= localState->camera->up * localState->speed * localState->deltaTime;
    //LOG(localState->camera->front.x << " " << localState->camera->front.y << " " << localState->camera->front.z)
    if (!localState->freeFlightMode)
    {
        glm::vec3 dend;
        glm::vec3 dnorm;
        glm::vec3 diend;
        Block *dblock = localState->chunks->rayCast(localState->camera->pos, glm::vec3(0, -1, 0), 2.0f, dend, dnorm, diend);

        auto block = localState->chunks->get(std::floor(localState->camera->pos.x), localState->camera->pos.y - 2, std::floor(localState->camera->pos.z));

        //bool collision = collisionCheck();
        //LOG(collision)

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !localState->inAir)
        {
            localState->upSpeed = 19.0;
            localState->inAir = true;
        }

        moveVector.y += localState->upSpeed * localState->deltaTime;

        if (dblock != nullptr)
        {
            localState->inAir = false;
        }
        else if (dend.y < localState->camera->pos.y - 2)
        {
            moveVector.y -= localState->speed * localState->deltaTime;
            localState->inAir = true;
        }

        if (localState->inAir)
            localState->upSpeed -= 45 * localState->deltaTime;
        else
            localState->upSpeed = 0;

        glm::vec3 hitCenter = glm::vec3(localState->camera->pos.x, localState->camera->pos.y - playerHeadOffset, localState->camera->pos.z);
        glm::vec3 hitDim = playerDimensions;
        glm::vec3 goodCenter = goodHitboxCenter(hitCenter, hitDim, moveVector, localState->camera->front - localState->camera->pos);
        goodCenter.y += playerHeadOffset;
        moveVector = goodCenter - localState->camera->pos;
    }
    localState->camera->pos += moveVector;
}


void cursorCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (localState->cursorStarted){
        localState->deltaX += xpos - localState->x;
        localState->deltaY += ypos - localState->y;
    }
    else localState->cursorStarted = true;
    localState->x = xpos;
    localState->y = ypos;
    if (localState->cursorLocked)
    {
        localState->camY += -localState->deltaY / Window::_height * 2;
        localState->camX += -localState->deltaX / Window::_height * 2;

        if (localState->camY < -glm::radians(89.0f)){
            localState->camY = -glm::radians(89.0f);
        }
        if (localState->camY > glm::radians(89.0f)){
            localState->camY = glm::radians(89.0f);
        }

        localState->camera->rotation = glm::mat4(1.0f);
        localState->camera->rotate(localState->camY, localState->camX, 0);
    }
    localState->deltaX = localState->deltaY = 0.0f;
}

void resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    Window::_width = width;
    Window::_height = height;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (!localState->cursorLocked) return;
    glm::vec3 end;
    glm::vec3 norm;
    glm::vec3 iend;
    Block *block = localState->chunks->rayCast(localState->camera->pos, localState->camera->front, 10.0f, end, norm, iend);

    if (block != nullptr)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            localState->chunks->set(int(iend.x), int(iend.y), int(iend.z), 0);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            localState->chunks->set(int(iend.x) + int(norm.x), int(iend.y) + int(norm.y), int(iend.z) + int(norm.z), localState->currentBlockId);
        }
    }
}
