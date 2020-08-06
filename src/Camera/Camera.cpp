//
// Created by Yaroslav on 30.07.2020.
//

#include <glm/ext.hpp>
#include "Camera.h"
#include "../Window/Window.hpp"

Camera::Camera(vec3 pos, float FOV) : pos(pos), FOV(FOV), rotation(1.0f)
{
    updateVectors();
}

Camera::~Camera()
{

}

mat4 Camera::getProjectionMatrix()
{
    return perspective(FOV, float(Window::_width) / float(Window::_height), 0.1f, 500.0f);
}

mat4 Camera::getViewMatrix()
{
    return lookAt(pos, pos + front, up);
}

void Camera::updateVectors()
{
    front = vec3(rotation * vec4(0, 0, -1, 1));
    up = vec3(rotation * vec4(0, 1, 0, 1));
    right = vec3(rotation * vec4(1, 0, 0, 1));
}

void Camera::rotate(float x, float y, float z)
{
    rotation = glm::rotate(rotation, z, vec3(0, 0, 1));
    rotation = glm::rotate(rotation, y, vec3(0, 1, 0));
    rotation = glm::rotate(rotation, x, vec3(1, 0, 0));

    updateVectors();
}
