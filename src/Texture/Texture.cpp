//
// Created by Yaroslav on 29.07.2020.
//

#include "Texture.hpp"
#include "../Logger.hpp"
#include <string>
#include <vector>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"


Texture::Texture(const char *name)
{
    this->name = name;
    loadTexture();
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::loadTexture()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    data = stbi_load(name, &width, &height, &nrChannels, 4);
    if (!data)
    {
        LOG("[ERROR] Failed to open texture " + std::string(name) + "\n\t" + stbi_failure_reason());
        // throw std::runtime_error("Failed to open texture " + std::string(name) + "\n\t" + stbi_failure_reason());
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    LOG("[INFO] Texture " + std::string(name) + " loaded.");
}




