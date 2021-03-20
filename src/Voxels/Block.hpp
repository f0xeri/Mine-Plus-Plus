//
// Created by Yaroslav on 30.07.2020.
//

#ifndef OPENGLTEST_BLOCK_HPP
#define OPENGLTEST_BLOCK_HPP


#include <cstdint>
#include <system_error>
#include <glm/glm.hpp>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Block
{
    int id;
    Block();
    Block(int id);
    Block(Block *pBlock);
};


#endif //OPENGLTEST_BLOCK_HPP
