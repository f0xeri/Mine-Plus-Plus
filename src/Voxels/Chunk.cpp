//
// Created by Yaroslav on 30.07.2020.
//

#include <cmath>
#include <vector>
#include "Chunk.hpp"
#include "block.hpp"
#include "../Logger.hpp"
#include <glm/gtc/noise.hpp>


Chunk::Chunk(int xpos, int ypos, int zpos) : x(xpos), y(ypos), z(zpos)
{
    blocks = new block[CHUNK_VOLUME];
    for (int z_ = 0; z_ < CHUNK_SIZE; z_++)
        for (int x_ = 0; x_ < CHUNK_SIZE; x_++)
        {
            int real_x = x_ + this->x * CHUNK_SIZE;
            int real_z = z_ + this->z * CHUNK_SIZE;
            float height = glm::perlin(glm::vec3(real_x * 0.05f, real_z * 0.05f, 0.f));
            for (int y_ = 0; y_ < CHUNK_Y; y_++)
            {
                int real_y = y_ + this->y * CHUNK_Y;
                int id = real_y <= height * 10 + 5;
                if (real_y <= 2)
                    id = 2;
                blocks[(y_ * CHUNK_SIZE + z_) * CHUNK_SIZE + x_].id = id;
                // blocksDict[{x, y, z}] = new block(id);
            }
        }

}

Chunk::~Chunk()
{
    delete [] blocks;
    delete mesh;
}

