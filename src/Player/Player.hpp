//
// Created by Yaroslav on 19.07.2021.
//

#ifndef MINE_PLAYER_HPP
#define MINE_PLAYER_HPP

#include <glm/glm.hpp>
#include "../State.hpp"

struct playerNetData
{
    uint32_t id;
    double posX, posY, posZ;
    float rotY;
    //double spawnPosX, spawnPosY, spawnPosZ;
};

class Player
{
protected:
public:
    Player(glm::vec3 pos);
    Player() = default;

    virtual void update(double dt, State *state);
    glm::vec3 spawnPos;
    glm::vec3 pos;
    float rotY;
    uint32_t id;
    const double sizeX = 0.80f;
    const double sizeY = 2.00f;
    const double sizeZ = 0.80f;
    const float headOffset = 1.0f;
};


#endif //MINE_PLAYER_HPP
