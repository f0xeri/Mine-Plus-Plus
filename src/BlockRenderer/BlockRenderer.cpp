//
// Created by Yaroslav on 31.07.2020.
//

#include <iostream>
#include <glm/glm.hpp>
#include "BlockRenderer.hpp"
#include "../Voxels/Block.hpp"
#include "../Logger.hpp"

#define IS_IN(X,Y,Z) ((X) >= 0 && (X) < CHUNK_SIZE && (Y) >= 0 && (Y) < CHUNK_Y && (Z) >= 0 && (Z) < CHUNK_SIZE)
#define VOXEL(X,Y,Z) (chunk->blocks[((Y) * CHUNK_SIZE + (Z)) * CHUNK_SIZE + (X)])
#define IS_BLOCKED(X,Y,Z) ((IS_IN(X, Y, Z)) && VOXEL(X, Y, Z).id)


BlockRenderer::BlockRenderer(int capacity) : capacity(capacity)
{

}

BlockRenderer::~BlockRenderer()
{

}

Mesh *BlockRenderer::createMesh(Chunk *chunk)
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
                Block blk = chunk->blocks[(y * CHUNK_SIZE + z) * CHUNK_SIZE + x];
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
                // top
                if (!IS_BLOCKED(x, y + 1, z))
                {
                    coords.insert(coords.end(), {
                            {0 + x, s + y, s + z},
                            {s + x, s + y, s + z},
                            {s + x, s + y, 0 + z},
                            {0 + x, s + y, 0 + z}
                    });

                    // if this Block is a grass Block, we should use another texture for top and sides
                    float uu = u, vv = v, uu1 = u1, uu2 = u2, vv1 = v1, vv2 = v2;
                    if (blk.id == 3) {
                        uu = (5 % 16) * tOffset;
                        vv = (5 / 16) * tOffset;
                        uu1 = uu + pixelSize;
                        uu2 = uu + tOffset - pixelSize;
                        vv1 = vv + pixelSize;
                        vv2 = vv + tOffset - pixelSize;
                    }

                    texCoords.insert(texCoords.end(), {{uu1, vv2}, {uu2, vv2}, {uu2, vv1}, {uu1, vv1}});
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

                // back side
                if (!IS_BLOCKED(x, y,z - 1))
                {
                    // if this Block is a grass Block, we should use another texture for top and sides
                    float uu = u, vv = v, uu1 = u1, uu2 = u2, vv1 = v1, vv2 = v2;
                    if (blk.id == 3 && !IS_BLOCKED(x, y + 1, z)) {
                        uu = (4 % 16) * tOffset;
                        vv = (4 / 16) * tOffset;
                        uu1 = uu + pixelSize;
                        uu2 = uu + tOffset - pixelSize;
                        vv1 = vv + pixelSize;
                        vv2 = vv + tOffset - pixelSize;
                    }

                    coords.insert(coords.end(), {
                            {s + x, 0 + y, 0 + z},
                            {0 + x, 0 + y, 0 + z},
                            {0 + x, s + y, 0 + z},
                            {s + x, s + y, 0 + z}
                    });
                    texCoords.insert(texCoords.end(), {{uu1, vv2}, {uu2, vv2}, {uu2, vv1}, {uu1, vv1}});
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
                // front side
                if (!IS_BLOCKED(x, y,z + 1))
                {
                    // if this Block is a grass Block, we should use another texture for top and sides
                    float uu = u, vv = v, uu1 = u1, uu2 = u2, vv1 = v1, vv2 = v2;
                    if (blk.id == 3 && !IS_BLOCKED(x, y + 1, z)) {
                        uu = (4 % 16) * tOffset;
                        vv = (4 / 16) * tOffset;
                        uu1 = uu + pixelSize;
                        uu2 = uu + tOffset - pixelSize;
                        vv1 = vv + pixelSize;
                        vv2 = vv + tOffset - pixelSize;
                    }

                    coords.insert(coords.end(),{
                            {0 + x, 0 + y, s + z},
                            {s + x, 0 + y, s + z},
                            {s + x, s + y, s + z},
                            {0 + x, s + y, s + z}
                    });
                    texCoords.insert(texCoords.end(), {{uu1, vv2}, {uu2, vv2}, {uu2, vv1}, {uu1, vv1}});
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
                // bottom
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
                // right side
                if (!IS_BLOCKED(x + 1, y, z))
                {
                    // if this Block is a grass Block, we should use another texture for top and sides
                    float uu = u, vv = v, uu1 = u1, uu2 = u2, vv1 = v1, vv2 = v2;
                    if (blk.id == 3 && !IS_BLOCKED(x, y + 1, z)) {
                        uu = (4 % 16) * tOffset;
                        vv = (4 / 16) * tOffset;
                        uu1 = uu + pixelSize;
                        uu2 = uu + tOffset - pixelSize;
                        vv1 = vv + pixelSize;
                        vv2 = vv + tOffset - pixelSize;
                    }

                    coords.insert(coords.end(),{
                            {s + x, 0 + y, s + z},
                            {s + x, 0 + y, 0 + z},
                            {s + x, s + y, 0 + z},
                            {s + x, s + y, s + z}
                    });
                    texCoords.insert(texCoords.end(), {{uu1, vv2}, {uu2, vv2}, {uu2, vv1}, {uu1, vv1}});
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
                // left side
                if (!IS_BLOCKED(x - 1, y, z))
                {
                    // if this Block is a grass Block, we should use another texture for top and sides
                    float uu = u, vv = v, uu1 = u1, uu2 = u2, vv1 = v1, vv2 = v2;
                    if (blk.id == 3 && !IS_BLOCKED(x, y + 1, z)) {
                        uu = (4 % 16) * tOffset;
                        vv = (4 / 16) * tOffset;
                        uu1 = uu + pixelSize;
                        uu2 = uu + tOffset - pixelSize;
                        vv1 = vv + pixelSize;
                        vv2 = vv + tOffset - pixelSize;
                    }

                    coords.insert(coords.end(),{
                            {0 + x, 0 + y, 0 + z},
                            {0 + x, 0 + y, s + z},
                            {0 + x, s + y, s + z},
                            {0 + x, s + y, 0 + z}
                    });
                    texCoords.insert(texCoords.end(), {{uu1, vv2}, {uu2, vv2}, {uu2, vv1}, {uu1, vv1}});
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
    Mesh *chuckModel = new Mesh();
    chuckModel->addData(coords, texCoords, normal, indices);
    return chuckModel;
}

void BlockRenderer::render(ChunkManager &chunks, int cx, int cz, int viewDistance, Shader &shader, int &renderedChunks)
{
    Chunk *chunk;
    Mesh *mesh;
    glm::mat4 model;
    for (int x_ = cx - viewDistance; x_ <= cx + viewDistance; x_++)
    {
        for (int z_ = cz - viewDistance; z_ <= cz + viewDistance; z_++)
        {
            auto findResult = chunks.chunksDict.find({x_, z_});
            if (findResult != chunks.chunksDict.end())
            {
                chunk = findResult->second;
                mesh = chunk->mesh;
                model = glm::mat4(1.0f);
                model = translate(model, glm::vec3(chunk->x * CHUNK_SIZE, chunk->y * CHUNK_SIZE, chunk->z * CHUNK_SIZE));
                shader.uniformMatrix(model, "model");
                mesh->draw();
                renderedChunks++;
            }
        }
    }
}
