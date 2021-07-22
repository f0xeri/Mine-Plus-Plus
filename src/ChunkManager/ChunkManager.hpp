//
// Created by Yaroslav on 01.08.2020.
//

#ifndef OPENGLTEST_CHUNKMANAGER_HPP
#define OPENGLTEST_CHUNKMANAGER_HPP


#include <map>

#include "../Voxels/Chunk.hpp"
#include "../Camera/Camera.h"
#include "../MineNetClient/MineNetClient.hpp"

class ChunkManager
{
    ~ChunkManager();
public:
    int w, d, h;
    std::map<std::pair<int, int>, Chunk*> chunksDict;
    int volume;
    ChunkManager(int w, int h, int d);
    Chunk * add(int wx, int wy, int wz);
    Block *get(int x, int y, int z);
    ChunkChangesSave set(int x, int y, int z, int id);
    Chunk *getChunk(int x, int y, int z);
    Block *rayCast(glm::vec3 a, glm::vec3 dir, float maxDist, glm::vec3 &end, glm::vec3 &norm, glm::vec3 &iend);
    void saveMap();
    void loadMap();
};


#endif //OPENGLTEST_CHUNKMANAGER_HPP
