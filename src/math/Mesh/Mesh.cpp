#include "Mesh.h"

#include <utility>
#include <cstddef>

Mesh::Mesh(const std::vector<Vertex>& vertices, GLenum usage) {
    m_vertexCount = static_cast<GLsizei>(vertices.size());
    m_indexed = false;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        usage
    );

    setupVertexAttributes();

    glBindVertexArray(0);
}

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    GLenum usage
) {
    m_vertexCount = static_cast<GLsizei>(vertices.size());
    m_indexCount  = static_cast<GLsizei>(indices.size());
    m_indexed = true;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        usage
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(uint32_t),
        indices.data(),
        usage
    );

    setupVertexAttributes();

    glBindVertexArray(0);
}

void Mesh::setupVertexAttributes() {
    // layout(location = 0) position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );

    // layout(location = 1) color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, color)
    );
}

Mesh::~Mesh() {
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

Mesh::Mesh(Mesh&& other) noexcept {
    *this = std::move(other);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this == &other) return *this;

    this->~Mesh();

    m_vao = std::exchange(other.m_vao, 0);
    m_vbo = std::exchange(other.m_vbo, 0);
    m_ebo = std::exchange(other.m_ebo, 0);

    m_vertexCount = std::exchange(other.m_vertexCount, 0);
    m_indexCount  = std::exchange(other.m_indexCount, 0);
    m_indexed     = std::exchange(other.m_indexed, false);

    return *this;
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    
    if (m_indexed) {
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, (void*)0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }

    glBindVertexArray(0);
}
