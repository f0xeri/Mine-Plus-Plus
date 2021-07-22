//
// Created by Yaroslav on 20.07.2021.
//

#ifndef MINE_NETPLAYER_HPP
#define MINE_NETPLAYER_HPP


#include <glm/glm.hpp>
#include "Player.hpp"

class NetPlayer : public Player
{
public:
    NetPlayer(glm::vec3 pos);
    void update(double dt, State *state);
};


#endif //MINE_NETPLAYER_HPP
