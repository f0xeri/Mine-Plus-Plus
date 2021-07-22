//
// Created by Yaroslav on 19.07.2021.
//

#include "Player.hpp"
#include "../Logger.hpp"

Player::Player(glm::vec3 pos)
{
    this->spawnPos = pos;
    this->pos = pos;
}

void Player::update(double dt, State *state)
{

}
