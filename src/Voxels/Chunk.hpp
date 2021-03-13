//
// Created by Yaroslav on 30.07.2020.
//

#ifndef OPENGLTEST_CHUNK_HPP
#define OPENGLTEST_CHUNK_HPP


#include <map>
#include "block.hpp"
#include "../Model/Mesh.hpp"

#define CHUNK_SIZE 32
#define CHUNK_Y 256  // experimental
// #define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_Y * CHUNK_SIZE

class Chunk {
public:
    int x = 0, y = 0, z = 0;
    block *blocks;
    Mesh *mesh = nullptr;
    bool modified = true;

    void getHeightIn(int xMin, int zMin, int xMax, int zMax);
    void getHeightMap();
    Chunk(int x, int y, int z);
    ~Chunk();
};
#endif //OPENGLTEST_CHUNK_HPP
