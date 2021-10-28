//
// Created by Yaroslav on 05.04.2021.
//

#include <GL/glew.h>
#include "ShadowMap.hpp"
#include "../Logger.hpp"

ShadowMap::ShadowMap(State *state, int width, int height, float shadowDistance, int layersNumber)
{
    this->width = width;
    this->height = height;
    this->state = state;
    this->shadowDistance = shadowDistance;

    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
    glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            GL_DEPTH_COMPONENT32F,
            width,
            height,
            layersNumber,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("[ERROR] Shadow map framebuffer is not complete")
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 ShadowMap::calculateLightViewMatrix(std::vector<glm::vec4> &frustrumPoints)
{
    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : frustrumPoints)
    {
        center += glm::vec3(v);
    }
    center /= frustrumPoints.size();
    float distFromCentroid = 1.0f;
    glm::vec3 lightCamPos = center - state->lightDir * distFromCentroid;
    glm::mat4 lightView = glm::lookAt(
            lightCamPos,
            center,
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
    state->calculatedLightPosition = center + state->lightDir;

    return lightView;
}

glm::mat4 ShadowMap::calculateLightProjectionMatrix(std::vector<glm::vec4> &frustrumPoints, glm::mat4 lightView)
{
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& v : frustrumPoints)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection;
}


void ShadowMap::bindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, depthMap, 0);
}

unsigned int ShadowMap::getTexture()
{
    return depthMap;
}

void ShadowMap::bind()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
}

