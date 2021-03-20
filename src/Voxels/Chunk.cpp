//
// Created by Yaroslav on 30.07.2020.
//

#include <cmath>
#include <vector>
#include "Chunk.hpp"
#include "Block.hpp"
#include "../Logger.hpp"
#include <glm/gtc/noise.hpp>


Chunk::Chunk(int xpos, int ypos, int zpos) : x(xpos), y(ypos), z(zpos)
{
    blocks = new Block[CHUNK_VOLUME];
    for (int z_ = 0; z_ < CHUNK_SIZE; z_++)
        for (int x_ = 0; x_ < CHUNK_SIZE; x_++)
        {
            int real_x = x_ + this->x * CHUNK_SIZE;
            int real_z = z_ + this->z * CHUNK_SIZE;
            float height = glm::perlin(glm::vec3(real_x * 0.05f, real_z * 0.05f, 0.f));
            //float height = glm::perlin(glm::vec3(real_x*0.0126125f,real_z*0.0126125f, 0.0f));
            //height += glm::perlin(glm::vec3(real_x*0.016125f,real_z*0.016125f, 0.0f))*3.5f;
            for (int y_ = 0; y_ < CHUNK_Y; y_++)
            {
                int real_y = y_ + this->y * CHUNK_Y;
                int id = (real_y <= height * 10 + 5) == 1 ? 3 : 0;
                if (real_y <= 2)
                   id = 2;

                /*int id = 0;
                if (real_y <= 2)
                    id = 1;*/
                blocks[(y_ * CHUNK_SIZE + z_) * CHUNK_SIZE + x_].id = id;
                // blocksDict[{x, y, z}] = new Block(id);
            }
        }

}

Chunk::~Chunk()
{
    delete [] blocks;
    delete mesh;
}

