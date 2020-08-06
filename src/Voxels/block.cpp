//
// Created by Yaroslav on 30.07.2020.
//

#include "block.hpp"

block::block(int id)
{
    this->id = id;
}

block::block(block *pBlock)
{
    pBlock->id = this->id;
}

block::block()
{

}
