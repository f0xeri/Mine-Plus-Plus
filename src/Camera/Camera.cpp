//
// Created by Yaroslav on 30.07.2020.
//

#include <glm/ext.hpp>
#include "Camera.h"
#include "../Window/Window.hpp"

Camera::Camera(glm::vec3 pos, float FOV) : pos(pos), FOV(FOV), rotation(1.0f)
{
    updateVectors();
}

Camera::~Camera()
{

}

glm::mat4 Camera::getProjectionMatrix()
{
    return glm::perspective(FOV, float(Window::_width) / float(Window::_height), 0.1f, 500.0f);
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(pos, pos + front, up);
}

void Camera::updateVectors()
{
    front = glm::vec3(rotation * glm::vec4(0, 0, -1, 1));
    up = glm::vec3(rotation * glm::vec4(0, 1, 0, 1));
    right = glm::vec3(rotation * glm::vec4(1, 0, 0, 1));
}

void Camera::rotate(float x, float y, float z)
{
    rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
    rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
    rotation = glm::rotate(rotation, x, glm::vec3(1, 0, 0));

    updateVectors();
}
