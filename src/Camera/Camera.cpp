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
    rotX = x; rotY = y; rotZ = z;

    rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
    rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
    rotation = glm::rotate(rotation, x, glm::vec3(1, 0, 0));

    updateVectors();
}

std::vector<glm::vec3> Camera::getFrustrumPoints(float farDist) {
    float nearDist = 0.0f;
    // float farDist = farDist;
    float aspectRatio = float(Window::_width) / float(Window::_height);
    auto tanFOV = glm::tan(FOV / 2);
    auto hNear = 2 * tanFOV * nearDist;
    auto wNear = hNear * aspectRatio;
    auto hFar = 2 * tanFOV * farDist;
    auto wFar = hFar * aspectRatio;

    glm::vec3 fc = pos + front * farDist;

    glm::vec3 ftl = fc + (up * glm::vec3(hFar) / glm::vec3(2.0f)) - (right * glm::vec3(wFar) / glm::vec3(2.0f));
    glm::vec3 ftr = fc + (up * glm::vec3(hFar) / glm::vec3(2.0f)) + (right * glm::vec3(wFar) / glm::vec3(2.0f));
    glm::vec3 fbl = fc - (up * glm::vec3(hFar) / glm::vec3(2.0f)) - (right * glm::vec3(wFar) / glm::vec3(2.0f));
    glm::vec3 fbr = fc - (up * glm::vec3(hFar) / glm::vec3(2.0f)) + (right * glm::vec3(wFar) / glm::vec3(2.0f));

    glm::vec3 nc = pos + front * nearDist;

    glm::vec3 ntl = nc + (up * glm::vec3(hNear) / glm::vec3(2.0f)) - (right * glm::vec3(wNear) / glm::vec3(2.0f));
    glm::vec3 ntr = nc + (up * glm::vec3(hNear) / glm::vec3(2.0f)) + (right * glm::vec3(wNear) / glm::vec3(2.0f));
    glm::vec3 nbl = nc - (up * glm::vec3(hNear) / glm::vec3(2.0f)) - (right * glm::vec3(wNear) / glm::vec3(2.0f));
    glm::vec3 nbr = nc - (up * glm::vec3(hNear) / glm::vec3(2.0f)) + (right * glm::vec3(wNear) / glm::vec3(2.0f));

    return std::vector{ftl, ftr, fbl, fbr, ntl, ntr, nbl, nbr};
}
