//
// Created by Yaroslav on 31.07.2020.
//

#include "CubeRenderer.hpp"
#include "../Model//Model.hpp"
#include "../Program/Program.hpp"
#include "../Logger.hpp"

CubeRenderer::CubeRenderer(Texture &texture, glm::vec3 pos_) : texture(texture) {
    float s = 0.5f;
    std::vector<glm::vec3> _cube = {
            // top
            {0 + pos_[0], s + pos_[1], s + pos_[2]},
            {s + pos_[0], s + pos_[1], s + pos_[2]},
            {s + pos_[0], s + pos_[1], 0 + pos_[2]},
            {0 + pos_[0], s + pos_[1], 0 + pos_[2]},

            // back
            {s + pos_[0], 0 + pos_[1], 0 + pos_[2]},
            {0 + pos_[0], 0 + pos_[1], 0 + pos_[2]},
            {0 + pos_[0], s + pos_[1], 0 + pos_[2]},
            {s + pos_[0], s + pos_[1], 0 + pos_[2]},

            {0 + pos_[0], 0 + pos_[1], s + pos_[2]},
            {s + pos_[0], 0 + pos_[1], s + pos_[2]},
            {s + pos_[0], s + pos_[1], s + pos_[2]},
            {0 + pos_[0], s + pos_[1], s + pos_[2]},

            {0 + pos_[0], 0 + pos_[1], 0 + pos_[2]},
            {s + pos_[0], 0 + pos_[1], 0 + pos_[2]},
            {s + pos_[0], 0 + pos_[1], s + pos_[2]},
            {0 + pos_[0], 0 + pos_[1], s + pos_[2]},

            {s + pos_[0], 0 + pos_[1], s + pos_[2]},
            {s + pos_[0], 0 + pos_[1], 0 + pos_[2]},
            {s + pos_[0], s + pos_[1], 0 + pos_[2]},
            {s + pos_[0], s + pos_[1], s + pos_[2]},

            {0 + pos_[0], 0 + pos_[1], 0 + pos_[2]},
            {0 + pos_[0], 0 + pos_[1], s + pos_[2]},
            {0 + pos_[0], s + pos_[1], s + pos_[2]},
            {0 + pos_[0], s + pos_[1], 0 + pos_[2]}
    };

    std::vector<glm::vec2> _cubeTexCoords;
    _cubeTexCoords.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        _cubeTexCoords.insert(_cubeTexCoords.end(), {
                {0, 1},
                {1, 1},
                {1, 0},
                {0, 0}
        });
    }

    std::vector<int> _cubeIndices;
    _cubeIndices.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        _cubeIndices.insert(_cubeIndices.end(),
                            {0 + i * 4, 1 + i * 4, 2 + i * 4,
                             2 + i * 4, 3 + i * 4, 0 + i * 4});
    }


    vao.addVertexBufferObject(_cube);
    vao.addVertexBufferObject(_cubeTexCoords);
    vao.addIndices(_cubeIndices);
}

nModel::Model *CubeRenderer::getVAO()
{
    return &vao;
}

void CubeRenderer::add(const glm::vec3 &position)
{
    quads.push_back(position);
}

void CubeRenderer::render()
{
    texture.bind();
    vao.draw();
}
