//
// Created by Yaroslav on 19.07.2021.
//

#ifndef MINE_PLAYERRENDERER_HPP
#define MINE_PLAYERRENDERER_HPP


#include "../Model/Mesh.hpp"
#include "../Player/Player.hpp"

class PlayerRenderer {

public:
    PlayerRenderer(Player *player);
    Mesh *mesh;
    GLuint VAO{};

    std::vector<GLuint> mBuffers;
    unsigned mIndicesBuffer = 0;
    size_t mIndicesCount = 0;

    Player *player;
    Mesh *createMesh();
    void render();

};


#endif //MINE_PLAYERRENDERER_HPP
