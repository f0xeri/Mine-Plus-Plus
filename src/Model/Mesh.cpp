//
// Created by Yaroslav on 28.07.2020.
//

#include "Mesh.hpp"

Mesh::Mesh()
{
    glGenVertexArrays(1, &mVAO);
    bind();
}

Mesh::~Mesh()
{
    glDeleteBuffers(mBuffers.size(), mBuffers.data());
    glDeleteBuffers(1, &mIndicesBuffer);
    glDeleteVertexArrays(1, &mVAO);
}

void Mesh::bind()
{
    glBindVertexArray(mVAO);
}

void Mesh::addData(const std::vector<glm::vec3> &vertex,
                   const std::vector<glm::vec2> &texCoords,
                   const std::vector<glm::vec3> &normal,
                   const std::vector<int>& indices)
{
    addVertexBufferObject(vertex);
    addVertexBufferObject(texCoords);
    addVertexBufferObject(normal);
    addIndices(indices);
}

void Mesh::addVertexBufferObject(const std::vector<float> &data)
{
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);
}

void Mesh::addVertexBufferObject(const std::vector<glm::vec2> &data)
{
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec2), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);
}

void Mesh::addVertexBufferObject(const std::vector<glm::vec3> &data)
{
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);
}

void Mesh::addVertexBufferObject(const std::vector<glm::vec4> &data)
{
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec4), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mBuffers.size(), 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    mBuffers.push_back(VBO);
}

void Mesh::draw(unsigned type)
{
    assert(mIndicesBuffer != 0);
    bind();
    for (size_t i = 0; i < mBuffers.size(); ++i)
    {
        glEnableVertexAttribArray(i);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBuffer);
    glDrawElements(type, mIndicesCount, GL_UNSIGNED_INT, nullptr);
    for (size_t i = 0; i < mBuffers.size(); ++i)
    {
        glDisableVertexAttribArray(i);
    }
}

void Mesh::addIndices(const std::vector<int> &data)
{
    assert(mIndicesBuffer == 0);
    glGenBuffers(1, &mIndicesBuffer);
    mIndicesCount = data.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned), data.data(), GL_STATIC_DRAW);
}
