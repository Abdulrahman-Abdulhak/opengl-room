#pragma once

#include <vector>
#include <cstdint>

#include <glad/glad.h>

#include "../../math/Vertex.h"

class Mesh {
public:
    explicit Mesh(const std::vector<Vertex>& vertices, GLenum usage = GL_STATIC_DRAW);

    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        GLenum usage = GL_STATIC_DRAW
    );

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw() const;

    GLuint VAO() const { return m_vao; }
    bool isIndexed() const { return m_indexed; }

private:
    void setupVertexAttributes();

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    GLsizei m_vertexCount = 0;
    GLsizei m_indexCount  = 0;
    bool m_indexed = false;
};
