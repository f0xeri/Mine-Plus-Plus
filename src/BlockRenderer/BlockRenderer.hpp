//
// Created by Yaroslav on 31.07.2020.
//

#ifndef OPENGLTEST_BLOCKRENDERER_HPP
#define OPENGLTEST_BLOCKRENDERER_HPP


#include "../Voxels/Chunk.hpp"
#include "../ChunkManager/ChunkManager.hpp"
#include "../Shader/Shader.hpp"

class BlockRenderer
{
    int capacity;
public:
    explicit BlockRenderer(int capacity);
    ~BlockRenderer();
    Mesh *createMesh(Chunk *chunk);
    void render(ChunkManager &chunks, int cx, int cz, int viewDistance, Shader &shader, int &renderedChunks);
    void render(ChunkManager &chunks, int cx, int cz, bool frontAngle, std::vector<glm::vec3> &camPoints, Shader &shader, int &renderedChunks);
};


#endif //OPENGLTEST_BLOCKRENDERER_HPP
