//
// Created by Yaroslav on 20.07.2021.
//

#include "NetPlayer.hpp"

void NetPlayer::update(double dt, State *state)
{
    Player::update(dt, state);
}

NetPlayer::NetPlayer(glm::vec3 pos) : Player(pos) {

}
