//
// Created by Yaroslav on 30.07.2020.
//

#include "Block.hpp"

Block::Block(int id)
{
    this->id = id;
}

Block::Block(Block *pBlock)
{
    pBlock->id = this->id;
}

Block::Block()
{

}
