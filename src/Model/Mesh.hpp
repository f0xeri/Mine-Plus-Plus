//
// Created by Yaroslav on 28.07.2020.
//

#ifndef OPENGLTEST_MODEL_HPP
#define OPENGLTEST_MODEL_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class Mesh {
private:
    GLuint mVAO{};
    std::vector<GLuint> mBuffers;
    unsigned mIndicesBuffer = 0;
    size_t mIndicesCount = 0;
public:
    Mesh();
    ~Mesh();
    void bind();
    void draw(unsigned type = GL_TRIANGLES);
    void addVertexBufferObject(const std::vector<glm::vec3> &data);
    void addVertexBufferObject(const std::vector<glm::vec2> &data);
    void addVertexBufferObject(const std::vector<glm::vec4> &data);
    void addVertexBufferObject(const std::vector<float> &data);
    void addIndices(const std::vector<int> &data);
    void addData(const std::vector<glm::vec3> &vertex, const std::vector<glm::vec2> &texCoords, const std::vector<glm::vec3> &normal, const std::vector<int> &indices);
};


#endif //OPENGLTEST_MODEL_HPP
