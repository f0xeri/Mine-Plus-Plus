//
// Created by Yaroslav on 19.07.2021.
//

#include <glm/gtx/transform.hpp>
#include "PlayerRenderer.hpp"

PlayerRenderer::PlayerRenderer(Player *player)
{
    this->player = player;
    createMesh();
}

Mesh *PlayerRenderer::createMesh()
{
    glm::vec3 position = player->pos;
    glm::vec3 size{player->sizeX, player->sizeY, player->sizeZ};
    position.y -= 2;
    position.x -= 0.4f;
    position.z -= 0.4f;
    std::vector<glm::vec3> rectVert = {
            // top
            {0 + position.x, size.y + position.y, size.z + position.z},
            {size.x + position.x, size.y + position.y, size.z + position.z},
            {size.x + position.x, size.y + position.y, 0 + position.z},
            {0 + position.x, size.y + position.y, 0 + position.z},

            // back
            {size.x + position.x, 0 + position.y, 0 + position.z},
            {0 + position.x, 0 + position.y, 0 + position.z},
            {0 + position.x, size.y + position.y, 0 + position.z},
            {size.x + position.x, size.y + position.y, 0 + position.z},

            {0 + position.x, 0 + position.y, size.z + position.z},
            {size.x + position.x, 0 + position.y, size.z + position.z},
            {size.x + position.x, size.y + position.y, size.z + position.z},
            {0 + position.x, size.y + position.y, size.z + position.z},

            {0 + position.x, 0 + position.y, 0 + position.z},
            {size.x + position.x, 0 + position.y, 0 + position.z},
            {size.x + position.x, 0 + position.y, size.z + position.z},
            {0 + position.x, 0 + position.y, size.z + position.z},

            {size.x + position.x, 0 + position.y, size.z + position.z},
            {size.x + position.x, 0 + position.y, 0 + position.z},
            {size.x + position.x, size.y + position.y, 0 + position.z},
            {size.x + position.x, size.y + position.y, size.z + position.z},

            {0 + position.x, 0 + position.y, 0 + position.z},
            {0 + position.x, 0 + position.y, size.z + position.z},
            {0 + position.x, size.y + position.y, size.z + position.z},
            {0 + position.x, size.y + position.y, 0 + position.z}

    };

    std::vector<glm::vec2> texCoords;
    texCoords.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        texCoords.insert(texCoords.end(), {
                {0, 1},
                {1, 1},
                {1, 0},
                {0, 0}
        });
    }

    float dImin = 0.0f;  // diffuseIntesitivityMin
    float dImax = 1.0f;  // diffuseIntesitivityMax
    std::vector<glm::vec3> normals = {
            {dImin, dImax, dImin},
            {dImin, dImax, dImin},
            {dImin, dImax, dImin},
            {dImin, dImax, dImin},

            {dImin, dImin, -dImax},
            {dImin, dImin, -dImax},
            {dImin, dImin, -dImax},
            {dImin, dImin, -dImax},

            {dImin, dImin, dImax},
            {dImin, dImin, dImax},
            {dImin, dImin, dImax},
            {dImin, dImin, dImax},

            {dImin, -dImax, dImin},
            {dImin, -dImax, dImin},
            {dImin, -dImax, dImin},
            {dImin, -dImax, dImin},

            {dImax, dImin, dImin},
            {dImax, dImin, dImin},
            {dImax, dImin, dImin},
            {dImax, dImin, dImin},

            {-dImax, dImin, dImin},
            {-dImax, dImin, dImin},
            {-dImax, dImin, dImin},
            {-dImax, dImin, dImin}
    };

    std::vector<int> indices;
    indices.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        indices.insert(indices.end(),
                       {0 + i * 4, 1 + i * 4, 2 + i * 4,
                            2 + i * 4, 3 + i * 4, 0 + i * 4});
    }

    GLuint VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, rectVert.size() * sizeof(glm::vec3), rectVert.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);

    glGenBuffers(1, &mIndicesBuffer);
    mIndicesCount = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
    return nullptr;
}

void PlayerRenderer::render()
{
    assert(mIndicesBuffer != 0);
    glBindVertexArray(VAO);
    for (size_t i = 0; i < mBuffers.size(); ++i)
    {
        glEnableVertexAttribArray(i);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBuffer);
    glDrawElements(GL_TRIANGLES, mIndicesCount, GL_UNSIGNED_INT, nullptr);
    for (size_t i = 0; i < mBuffers.size(); ++i)
    {
        glDisableVertexAttribArray(i);
    }
}
