//
// Created by Yaroslav on 29.07.2020.
//

#ifndef OPENGLTEST_TEXTURE_HPP
#define OPENGLTEST_TEXTURE_HPP


#include <string>
#include <GL/glew.h>
#include <vector>

class Texture
{
private:
    const char *name;
    unsigned char *data;
    int width, height, nrChannels;
public:
    GLuint texture;
    Texture(const char *name);
    ~Texture();
    void loadTexture();
    void bind();
};


#endif //OPENGLTEST_TEXTURE_HPP
