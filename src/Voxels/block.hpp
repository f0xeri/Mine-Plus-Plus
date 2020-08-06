//
// Created by Yaroslav on 30.07.2020.
//

#ifndef OPENGLTEST_BLOCK_HPP
#define OPENGLTEST_BLOCK_HPP


#include <cstdint>
#include <system_error>

struct block
{
    int id;
    block();
    block(int id);
    block(block *pBlock);
};


#endif //OPENGLTEST_BLOCK_HPP
