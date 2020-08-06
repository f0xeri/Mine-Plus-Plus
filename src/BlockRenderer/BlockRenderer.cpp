//
// Created by Yaroslav on 31.07.2020.
//

#include <iostream>
#include "BlockRenderer.hpp"
#include "../Voxels/block.hpp"

#define VERTEX_SIZE (3 + 2)
#define IS_IN(X,Y,Z) ((X) >= 0 && (X) < CHUNK_SIZE && (Y) >= 0 && (Y) < CHUNK_Y && (Z) >= 0 && (Z) < CHUNK_SIZE)
#define VOXEL(X,Y,Z) (chunk->blocks[((Y) * CHUNK_SIZE + (Z)) * CHUNK_SIZE + (X)])
#define IS_BLOCKED(X,Y,Z) ((IS_IN(X, Y, Z)) && VOXEL(X, Y, Z).id)


BlockRenderer::BlockRenderer(int capacity) : capacity(capacity)
{

}

BlockRenderer::~BlockRenderer()
{

}

nModel::Model *BlockRenderer::render(Chunk *chunk, const Chunk **pChunk)
{
    std::vector<glm::vec3> coords;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normal;
    std::vector<int> indices;
    int indecesIndex = 0;
    float dImin = 0.0f;  // diffuseIntesitivityMin
    float dImax = 1.0f;  // diffuseIntesitivityMax
    int s = 1;
    for (int y = 0; y < CHUNK_Y; y++)
        for (int z = 0; z < CHUNK_SIZE; z++)
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                block blk = chunk->blocks[(y * CHUNK_SIZE + z) * CHUNK_SIZE + x];
                unsigned int id = blk.id;

                float pixelSize = 1.0f / 512.0f;
                float tOffset = (1.0f) / 16.0f;
                float u = (id % 16) * tOffset;
                float v = (id / 16) * tOffset;
                float u1 = u + pixelSize;
                float u2 = u + tOffset - pixelSize;
                float v1 = v + pixelSize;
                float v2 = v + tOffset - pixelSize;
                if (!id) continue;

                if (!IS_BLOCKED(x, y + 1, z))
                {
                    coords.insert(coords.end(), {
                            {0 + x, s + y, s + z},
                            {s + x, s + y, s + z},
                            {s + x, s + y, 0 + z},
                            {0 + x, s + y, 0 + z}
                    });
                    texCoords.insert(texCoords.end(), {{u1, v2}, {u2, v2}, {u2, v1}, {u1, v1}});
                    normal.insert(normal.end(), {
                            {dImin, dImax, dImin},
                            {dImin, dImax, dImin},
                            {dImin, dImax, dImin},
                            {dImin, dImax, dImin}
                    });
                    indices.insert(indices.end(), {
                        indecesIndex, indecesIndex + 1, indecesIndex + 2,
                        indecesIndex + 2, indecesIndex + 3, indecesIndex
                    });
                    indecesIndex += 4;
                }
                if (!IS_BLOCKED(x, y,z - 1))
                {
                    coords.insert(coords.end(), {
                            {s + x, 0 + y, 0 + z},
                            {0 + x, 0 + y, 0 + z},
                            {0 + x, s + y, 0 + z},
                            {s + x, s + y, 0 + z}
                    });
                    texCoords.insert(texCoords.end(), {{u1, v2}, {u2, v2}, {u2, v1}, {u1, v1}});
                    normal.insert(normal.end(), {
                            {dImin, dImin, -dImax},
                            {dImin, dImin, -dImax},
                            {dImin, dImin, -dImax},
                            {dImin, dImin, -dImax}
                    });
                    indices.insert(indices.end(), {
                            indecesIndex, indecesIndex + 1, indecesIndex + 2,
                            indecesIndex + 2, indecesIndex + 3, indecesIndex
                    });
                    indecesIndex += 4;
                }
                if (!IS_BLOCKED(x, y,z + 1))
                {
                    coords.insert(coords.end(),{
                            {0 + x, 0 + y, s + z},
                            {s + x, 0 + y, s + z},
                            {s + x, s + y, s + z},
                            {0 + x, s + y, s + z}
                    });
                    texCoords.insert(texCoords.end(), {{u1, v2}, {u2, v2}, {u2, v1}, {u1, v1}});
                    normal.insert(normal.end(), {
                            {dImin, dImin, dImax},
                            {dImin, dImin, dImax},
                            {dImin, dImin, dImax},
                            {dImin, dImin, dImax}
                    });
                    indices.insert(indices.end(), {
                            indecesIndex, indecesIndex + 1, indecesIndex + 2,
                            indecesIndex + 2, indecesIndex + 3, indecesIndex
                    });
                    indecesIndex += 4;
                }
                if (!IS_BLOCKED(x,y - 1,z))
                {
                    coords.insert(coords.end(),{
                            {0 + x, 0 + y, 0 + z},
                            {s + x, 0 + y, 0 + z},
                            {s + x, 0 + y, s + z},
                            {0 + x, 0 + y, s + z}
                    });
                    texCoords.insert(texCoords.end(), {{u1, v2}, {u2, v2}, {u2, v1}, {u1, v1}});
                    normal.insert(normal.end(), {
                            {dImin, -dImax, dImin},
                            {dImin, -dImax, dImin},
                            {dImin, -dImax, dImin},
                            {dImin, -dImax, dImin}
                    });
                    indices.insert(indices.end(), {
                            indecesIndex, indecesIndex + 1, indecesIndex + 2,
                            indecesIndex + 2, indecesIndex + 3, indecesIndex
                    });
                    indecesIndex += 4;
                }
                if (!IS_BLOCKED(x + 1, y, z))
                {
                    coords.insert(coords.end(),{
                            {s + x, 0 + y, s + z},
                            {s + x, 0 + y, 0 + z},
                            {s + x, s + y, 0 + z},
                            {s + x, s + y, s + z}
                    });
                    texCoords.insert(texCoords.end(), {{u1, v2}, {u2, v2}, {u2, v1}, {u1, v1}});
                    normal.insert(normal.end(), {
                            {dImax, dImin, dImin},
                            {dImax, dImin, dImin},
                            {dImax, dImin, dImin},
                            {dImax, dImin, dImin}
                    });
                    indices.insert(indices.end(), {
                            indecesIndex, indecesIndex + 1, indecesIndex + 2,
                            indecesIndex + 2, indecesIndex + 3, indecesIndex
                    });
                    indecesIndex += 4;
                }
                if (!IS_BLOCKED(x - 1, y, z))
                {
                    coords.insert(coords.end(),{
                            {0 + x, 0 + y, 0 + z},
                            {0 + x, 0 + y, s + z},
                            {0 + x, s + y, s + z},
                            {0 + x, s + y, 0 + z}
                    });
                    texCoords.insert(texCoords.end(), {{u1, v2}, {u2, v2}, {u2, v1}, {u1, v1}});
                    normal.insert(normal.end(), {
                            {-dImax, dImin, dImin},
                            {-dImax, dImin, dImin},
                            {-dImax, dImin, dImin},
                            {-dImax, dImin, dImin}
                    });
                    indices.insert(indices.end(), {
                            indecesIndex, indecesIndex + 1, indecesIndex + 2,
                            indecesIndex + 2, indecesIndex + 3, indecesIndex
                    });
                    indecesIndex += 4;
                }
            }
    nModel::Model *chuckModel = new nModel::Model();
    chuckModel->addData(coords, texCoords, normal, indices);
    return chuckModel;
}