//
// Created by Yaroslav on 05.04.2021.
//

#ifndef MINE_SHADOWMAP_HPP
#define MINE_SHADOWMAP_HPP


#include <vector>
#include "../Camera/Camera.h"
#include "../State.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class ShadowMap
{
    unsigned int depthMapFBO;
    unsigned int depthMap;
    State *state;

public:
    unsigned int width, height;
    float shadowDistance;

    ShadowMap(State *state, int width, int height, float shadowDistance, int layersNumber);
    glm::mat4 calculateLightViewMatrix(std::vector<glm::vec4> &frustrumPoints);
    glm::mat4 calculateLightProjectionMatrix(std::vector<glm::vec4> &frustrumPoints, glm::mat4 lightView);
    void bindFBO();
    void bind();
    unsigned int getTexture();
};


#endif //MINE_SHADOWMAP_HPP
