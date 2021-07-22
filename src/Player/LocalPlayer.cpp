//
// Created by Yaroslav on 20.07.2021.
//

#include "LocalPlayer.hpp"

void LocalPlayer::update(double dt, State *state) {
    if (state->thirdPersonView)
    {
        state->camera->pos.y = pos.y + 3;
        state->camera->pos.x = pos.x + glm::sin(state->camera->rotY) * 3;
        state->camera->pos.z = pos.z + glm::cos(state->camera->rotY) * 3;
    }
    else
    {
        state->camera->pos = pos;
    }
    rotY = state->camera->rotY;
}

LocalPlayer::LocalPlayer(glm::vec3 pos) : Player(pos) {

}
