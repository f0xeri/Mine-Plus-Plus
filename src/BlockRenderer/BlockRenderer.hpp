//
// Created by Yaroslav on 31.07.2020.
//

#ifndef OPENGLTEST_BLOCKRENDERER_HPP
#define OPENGLTEST_BLOCKRENDERER_HPP


#include "../Voxels/Chunk.hpp"

class BlockRenderer
{
    int capacity;
public:
    explicit BlockRenderer(int capacity);
    ~BlockRenderer();
    nModel::Model *createMesh(Chunk *chunk, const Chunk **pChunk);
};


#endif //OPENGLTEST_BLOCKRENDERER_HPP
