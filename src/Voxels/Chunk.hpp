//
// Created by Yaroslav on 30.07.2020.
//

#ifndef OPENGLTEST_CHUNK_HPP
#define OPENGLTEST_CHUNK_HPP


#include <map>
#include "../CubeRenderer/CubeRenderer.hpp"
#include "block.hpp"

#define CHUNK_SIZE 16
#define CHUNK_Y 256  // experimental
// #define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_Y * CHUNK_SIZE

class Chunk{
public:
        int x = 0, y = 0, z = 0;
    block *blocks;
    // std::map<std::vector<int>, block> blocksDict;
    nModel::Model *mesh = nullptr;
    bool modified = true;

    Chunk(int x, int y, int z);
    ~Chunk();
};
#endif //OPENGLTEST_CHUNK_HPP
