//
// Created by Yaroslav on 05.04.2021.
//

#include <GL/glew.h>
#include "ShadowMap.hpp"

ShadowMap::ShadowMap(State *state, int width, int height, float shadowDistance)
{
    this->width = width;
    this->height = height;
    this->state = state;
    this->shadowDistance = shadowDistance;

    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 ShadowMap::calculateLightViewMatrix(std::vector<glm::vec3> &frustrumPoints)
{
    glm::vec3 centroid(0.0f);
    for (auto p : frustrumPoints) {
        centroid += p;
    }
    centroid /= frustrumPoints.size();
    float distFromCentroid = 1.0f;
    glm::vec3 lightCamPos = centroid - state->lightDir * distFromCentroid;
    state->calculatedLightPosition = lightCamPos;

    return glm::lookAt(lightCamPos, centroid, glm::vec3(0.0, 1.0, 0.0));
}

glm::mat4 ShadowMap::calculateLightProjectionMatrix(std::vector<glm::vec3> &frustrumPoints, glm::mat4 lightView)
{
    glm::vec3 min(0.0f);
    glm::vec3 max(0.0f);
    glm::vec3 result(0.0f);
    bool first = true;

    for (auto point : frustrumPoints) {
        point = glm::vec4(point, 1.0f) * lightView;
        if (first) {
            min.x = point.x;
            max.x = point.x;
            min.y = point.y;
            max.y = point.y;
            min.z = point.z;
            max.z = point.z;
            first = false;
            continue;
        }
        if (point.x > max.x) {
            max.x = point.x;
        } else if (point.x < min.x) {
            min.x = point.x;
        }
        if (point.y > max.y) {
            max.y = point.y;
        } else if (point.y < min.y) {
            min.y = point.y;
        }
        if (point.z > max.z) {
            max.z = point.z;
        } else if (point.z < min.z) {
            min.z = point.z;
        }
    }

    glm::mat4 lightProjection = glm::mat4(1.0f);
    lightProjection[0][0] = 2.0f / (max.x - min.x);
    lightProjection[1][1] = 2.0f / (max.y - min.y);
    lightProjection[2][2] = -2.0f / (max.z - min.z);
    lightProjection[3][3] = 1;
    return lightProjection;
}

// I DON'T KNOW WHY IT'S NOT WORKING
glm::mat4 ShadowMap::getLightSpaceMatrix() {
    auto frustrumPoints = state->camera->getFrustrumPoints(25);
    auto lightView = calculateLightViewMatrix(frustrumPoints);
    auto lightProjection = calculateLightProjectionMatrix(frustrumPoints, lightView);
    return lightView * lightProjection;
}

void ShadowMap::bindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
}

unsigned int ShadowMap::getTexture()
{
    return depthMap;
}

void ShadowMap::bind()
{
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

