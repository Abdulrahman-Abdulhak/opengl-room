#pragma once

#include <string>
#include <glm/glm.hpp>
#include "math/Vertex.h"
#include "math/Mesh/Mesh.h"
#include <memory>
#include "utils/Shader/Shader.h"

class Room {
public:
    // Paths may be empty to skip loading that texture.
    Room(float width, float height, float depth,
         const std::string& wallTexturePath = std::string(),
         const std::string& ceilTexturePath = std::string(),
         const std::string& floorTexturePath = std::string(),
         bool addWindowGlass = false,
         const std::string& glassTexturePath = std::string());

    ~Room();

    // Access the opaque mesh (primary geometry)
    const Mesh& mesh() const { return *m_opaqueMesh; }

    // Texture accessors (0 means not loaded)
    unsigned int wallTexture() const { return m_wallTex; }
    unsigned int ceilTexture() const { return m_ceilTex; }
    unsigned int floorTexture() const { return m_floorTex; }
    unsigned int glassTexture() const { return m_glassTex; }
    unsigned int painting1Texture() const { return m_paint1Tex; }
    unsigned int painting2Texture() const { return m_paint2Tex; }

    // Draw the room using the provided shader. Shader must accept the
    // optional sampler uniforms: `floorTex`, `wallTex`, `ceilTex`, `glassTex`.
    void draw(Shader& shader) const;

private:
    std::unique_ptr<Mesh> m_opaqueMesh;
    std::unique_ptr<Mesh> m_transparentMesh;
    unsigned int m_wallTex = 0;
    unsigned int m_ceilTex = 0;
    unsigned int m_floorTex = 0;
    unsigned int m_glassTex = 0;
    unsigned int m_paint1Tex = 0;
    unsigned int m_paint2Tex = 0;
    glm::vec3 m_paint1Center = glm::vec3(0.0f);
    glm::vec2 m_paint1Size = glm::vec2(1.0f);
    glm::vec3 m_paint2Center = glm::vec3(0.0f);
    glm::vec2 m_paint2Size = glm::vec2(1.0f);
    glm::vec3 m_roomCenter = glm::vec3(0.0f);
};