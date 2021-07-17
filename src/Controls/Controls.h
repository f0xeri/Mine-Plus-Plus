//
// Created by Yaroslav on 10.02.2021.
//

#ifndef MINE_CONTROLS_H
#define MINE_CONTROLS_H

#include "../State.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/perpendicular.hpp>

class Controls {
public:
    Controls(State *s);
};

void toggleCursor(GLFWwindow *window);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void updateInputs(GLFWwindow *window);
void cursorCallback(GLFWwindow *window, double xpos, double ypos);
void resizeCallback(GLFWwindow *window, int width, int height);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);


#endif //MINE_CONTROLS_H
