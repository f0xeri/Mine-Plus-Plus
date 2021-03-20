//
// Created by Yaroslav on 01.08.2020.
//

#include <fstream>
#include "ChunkManager.hpp"
#include "../Logger.hpp"
#include <sstream>

ChunkManager::ChunkManager(int w, int h, int d) : w(w), h(h), d(d)
{
    volume = w * h * d;
    int index = 0;
    for (int y = 0; y < h; y++)
        for (int z = 0; z < d; z++)
            for (int x = 0; x < w; x++, index++)
            {
                Chunk *chunk = new Chunk(x, y, z);
                chunksDict[{x, z}] = chunk;
            }
}

ChunkManager::~ChunkManager()
{
    chunksDict.clear();
}

Chunk *ChunkManager::add(int wx, int wy, int wz)
{
    Chunk *chunk = new Chunk(wx, wy, wz);
    chunk->modified = true;
    if (wx > 0) w++;
    if (wy > 0) h++;
    if (wz > 0) d++;
    volume++;
    chunksDict[{wx, wz}] = chunk;
    return chunk;
}

Chunk* ChunkManager::getChunk(int x, int y, int z){
    if (chunksDict.count({x, z}) != 0)
        return chunksDict[{x, z}];
    else
        return nullptr;
}

Block *ChunkManager::get(int x, int y, int z)
{
    int cx = x / CHUNK_SIZE;
    int cy = y / CHUNK_Y;
    int cz = z / CHUNK_SIZE;

    if (x < 0 && x % CHUNK_SIZE != 0) cx--;
    if (y < 0) cy--;
    if (z < 0 && z % CHUNK_SIZE != 0) cz--;
    if (chunksDict.count({cx, cz}) == 0) {
        return nullptr;
    }

    Chunk *chunk = chunksDict[{cx, cz}];

    int lx = x - cx * CHUNK_SIZE;
    int ly = y - cy * CHUNK_Y;
    int lz = z - cz * CHUNK_SIZE;

    //std::cout << lx << " " << ly << " " << lz << std::endl;
    return &chunk->blocks[(ly * CHUNK_SIZE + lz) * CHUNK_SIZE + lx];
    //return &chunk->blocksDict[{lx, ly, lz}];
}

void ChunkManager::set(int x, int y, int z, int id)
{
    int cx = x / CHUNK_SIZE;
    int cy = y / CHUNK_Y;
    int cz = z / CHUNK_SIZE;
    if (x < 0 && x % CHUNK_SIZE != 0) cx--;
    if (y < 0) cy--;
    if (z < 0 && z % CHUNK_SIZE != 0) cz--;
    if (chunksDict.count({cx, cz}) == 0)
        return;
    Chunk* chunk = chunksDict[{cx, cz}];
    int lx = x - cx * CHUNK_SIZE;
    int ly = y - cy * CHUNK_Y;
    int lz = z - cz * CHUNK_SIZE;
    //std::cout << lx << " " << ly << " " << lz << std::endl;
    chunk->blocks[(ly * CHUNK_SIZE + lz) * CHUNK_SIZE + lx].id = id;
    chunk->modified = true;

    if (lx == 0 && (chunk = getChunk(cx-1, cy, cz))) chunk->modified = true;
    if (ly == 0 && (chunk = getChunk(cx, cy-1, cz))) chunk->modified = true;
    if (lz == 0 && (chunk = getChunk(cx, cy, cz-1))) chunk->modified = true;

    if (lx == CHUNK_SIZE - 1 && (chunk = getChunk(cx+1, cy, cz))) chunk->modified = true;
    if (ly == CHUNK_Y - 1 && (chunk = getChunk(cx, cy+1, cz))) chunk->modified = true;
    if (lz == CHUNK_SIZE - 1 && (chunk = getChunk(cx, cy, cz+1))) chunk->modified = true;
}

Block* ChunkManager::rayCast(glm::vec3 a, glm::vec3 dir, float maxDist, glm::vec3& end, glm::vec3& norm, glm::vec3& iend) {
    float px = a.x;
    float py = a.y;
    float pz = a.z;

    float dx = dir.x;
    float dy = dir.y;
    float dz = dir.z;

    float t = 0.0f;
    int ix = floor(px);
    int iy = floor(py);
    int iz = floor(pz);

    float stepx = (dx > 0.0f) ? 1.0f : -1.0f;
    float stepy = (dy > 0.0f) ? 1.0f : -1.0f;
    float stepz = (dz > 0.0f) ? 1.0f : -1.0f;

    float infinity = std::numeric_limits<float>::infinity();

    float txDelta = (dx == 0.0f) ? infinity : abs(1.0f / dx);
    float tyDelta = (dy == 0.0f) ? infinity : abs(1.0f / dy);
    float tzDelta = (dz == 0.0f) ? infinity : abs(1.0f / dz);

    float xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
    float ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
    float zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

    float txMax = (txDelta < infinity) ? txDelta * xdist : infinity;
    float tyMax = (tyDelta < infinity) ? tyDelta * ydist : infinity;
    float tzMax = (tzDelta < infinity) ? tzDelta * zdist : infinity;

    int steppedIndex = -1;

    while (t <= maxDist)
    {
        Block* block = get(ix, iy, iz);
        if (block == nullptr || block->id)
        {
            end.x = px + t * dx;
            end.y = py + t * dy;
            end.z = pz + t * dz;

            iend.x = ix;
            iend.y = iy;
            iend.z = iz;

            norm.x = norm.y = norm.z = 0.0f;
            if (steppedIndex == 0) norm.x = -stepx;
            if (steppedIndex == 1) norm.y = -stepy;
            if (steppedIndex == 2) norm.z = -stepz;
            return block;
        }
        if (txMax < tyMax) {
            if (txMax < tzMax) {
                ix += stepx;
                t = txMax;
                txMax += txDelta;
                steppedIndex = 0;
            } else {
                iz += stepz;
                t = tzMax;
                tzMax += tzDelta;
                steppedIndex = 2;
            }
        } else {
            if (tyMax < tzMax) {
                iy += stepy;
                t = tyMax;
                tyMax += tyDelta;
                steppedIndex = 1;
            } else {
                iz += stepz;
                t = tzMax;
                tzMax += tzDelta;
                steppedIndex = 2;
            }
        }
    }
    iend.x = ix;
    iend.y = iy;
    iend.z = iz;

    end.x = px + t * dx;
    end.y = py + t * dy;
    end.z = pz + t * dz;
    norm.x = norm.y = norm.z = 0.0f;
    return nullptr;
}

void ChunkManager::saveMap()
{
    std::ofstream file("map.world", std::ios::out);
    int index = 0;
    std::ostringstream ss;
    ss << chunksDict.size() << "\n";

    for (auto &chunk : chunksDict)
    {
        ss << chunk.second->x;
        ss << "\t";
        ss << chunk.second->z;
        ss << "\n";
        auto blocks = chunk.second->blocks;
        for (int i = 0; i < CHUNK_VOLUME; i++, index++)
        {
            ss << blocks[i].id << " ";
        }
        ss << "\n";
    }
    file << ss.str();
    LOG("[INFO] Map saved on disk.");
}

void ChunkManager::loadMap()
{
    std::ifstream file("map.world");
    int chunkCount;
    file >> chunkCount;
    unsigned int time = glfwGetTime();

    for (int i = 0; i < chunkCount; i++)
    {
        int x, z;
        file >> x >> z;
        Chunk *chunk;
        if (chunksDict.count({x, z}) != 0)
        {
            chunk = chunksDict[{x, z}];
        }
        else
        {
            chunk = add(x, 0, z);
        }
        for (int b = 0; b < CHUNK_VOLUME; b++)
        {
            file >> chunk->blocks[b].id;
        }
        chunk->modified = true;
    }
    LOG("[INFO] " << chunkCount << " chunks loaded from disk in " << glfwGetTime() - time << " seconds.");
}
