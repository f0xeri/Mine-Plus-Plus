//
// Created by Yaroslav on 20.07.2021.
//

#ifndef MINE_LOCALPLAYER_HPP
#define MINE_LOCALPLAYER_HPP


#include "Player.hpp"

class LocalPlayer : public Player
{
public:
    void update(double dt, State *state);
    LocalPlayer(glm::vec3 pos);
};


#endif //MINE_LOCALPLAYER_HPP
