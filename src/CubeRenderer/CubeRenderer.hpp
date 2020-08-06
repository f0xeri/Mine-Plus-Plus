//
// Created by Yaroslav on 31.07.2020.
//

#ifndef OPENGLTEST_CUBERENDERER_HPP
#define OPENGLTEST_CUBERENDERER_HPP

#include <glm/glm.hpp>
#include <vector>
#include "../Texture/Texture.hpp"
#include "../Model/Model.hpp"
#include "../Program/Program.hpp"


class CubeRenderer
{
private:
    std::vector<glm::vec3> quads;
public:
    CubeRenderer(Texture &texture, glm::vec3 pos_);
    Texture texture;
    nModel::Model vao;
    void add(const glm::vec3& position);
    void render();


    nModel::Model * getVAO();
};


#endif //OPENGLTEST_CUBERENDERER_HPP
