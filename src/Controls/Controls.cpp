//
// Created by Yaroslav on 10.02.2021.
//

#include <thread>
#include <iostream>
#include "Controls.h"
#include "../Window/Window.hpp"

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
    if (key == GLFW_KEY_SPACE)
        switch (action)
        {
            case GLFW_PRESS:
            {
                localState->speed = localState->speed * 2;
                break;
            }
            case GLFW_RELEASE:
            {
                localState->speed = localState->speed / 2;
                break;
            }
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
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        localState->currentBlockId = 0;
    }
}

void updateInputs(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) localState->camera->pos += localState->camera->front * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) localState->camera->pos -= localState->camera->right * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) localState->camera->pos -= localState->camera->front * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) localState->camera->pos += localState->camera->right * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) localState->camera->pos += localState->camera->up * localState->speed * localState->deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) localState->camera->pos -= localState->camera->up * localState->speed * localState->deltaTime;
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

        if (localState->camY < -radians(89.0f)){
            localState->camY = -radians(89.0f);
        }
        if (localState->camY > radians(89.0f)){
            localState->camY = radians(89.0f);
        }

        localState->camera->rotation = mat4(1.0f);
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
    vec3 end;
    vec3 norm;
    vec3 iend;
    block *block = localState->chunks->rayCast(localState->camera->pos, localState->camera->front, 10.0f, end, norm, iend);

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
