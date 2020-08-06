//
// Created by Yaroslav on 30.07.2020.
//

#ifndef OPENGLTEST_CAMERA_H
#define OPENGLTEST_CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;

class Camera
{
private:
    void updateVectors();
public:
    float FOV;
    vec3 pos;
    mat4 rotation;
    vec3 front;
    vec3 up;
    vec3 right;

    Camera(vec3 pos, float FOV);
    ~Camera();

    mat4 getProjectionMatrix();
    mat4 getViewMatrix();
    void rotate(float x, float y, float z);
};


#endif //OPENGLTEST_CAMERA_H
