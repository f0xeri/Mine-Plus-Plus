//
// Created by Yaroslav on 30.07.2020.
//

#ifndef OPENGLTEST_CAMERA_H
#define OPENGLTEST_CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
private:
    void updateVectors();
public:
    float FOV;
    glm::vec3 pos;
    glm::mat4 rotation;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    Camera(glm::vec3 pos, float FOV);
    ~Camera();

    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix();
    void rotate(float x, float y, float z);
};


#endif //OPENGLTEST_CAMERA_H
