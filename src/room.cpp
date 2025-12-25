#include "room.h"

#include <vector>
#include <utility>
#include <iostream>

#include "utils/Texture/Texture.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

// Internal helpers (ported from previous header-only implementation)
static void pushQuadInward(
    std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indicies,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c,
    const glm::vec3& d,
    const glm::vec3& color
) {
    uint32_t base = static_cast<uint32_t>(vertices.size());
    vertices.push_back({a, color});
    vertices.push_back({b, color});
    vertices.push_back({c, color});
    vertices.push_back({d, color});

    indicies.push_back(base + 0);
    indicies.push_back(base + 1);
    indicies.push_back(base + 2);

    indicies.push_back(base + 0);
    indicies.push_back(base + 2);
    indicies.push_back(base + 3);
}

// Build two meshes: opaque geometry and transparent geometry (glass)
static std::pair<Mesh, Mesh> buildRoomMeshes(float width, float height, float depth, bool addGlass) {
    std::vector<Vertex> o_vertices;
    std::vector<uint32_t> o_indices;
    std::vector<Vertex> t_vertices;
    std::vector<uint32_t> t_indices;

    o_vertices.reserve(24);
    o_indices.reserve(36);
    if (addGlass) { t_vertices.reserve(4); t_indices.reserve(6); }

    float hx = width * 0.5f;
    float hz = depth * 0.5f;

    float y0 = 0.0f;
    float y1 = height;

    glm::vec3 p1(-hx, y0, -hz);
    glm::vec3 p2( hx, y0, -hz);
    glm::vec3 p3( hx, y1, -hz);
    glm::vec3 p4(-hx, y1, -hz);

    glm::vec3 p5(-hx, y0,  hz);
    glm::vec3 p6( hx, y0,  hz);
    glm::vec3 p7( hx, y1,  hz);
    glm::vec3 p8(-hx, y1,  hz);

    glm::vec3 colFloor(0.8f, 0.8f, 0.8f);
    glm::vec3 colCeiling(0.7f, 0.7f, 0.9f);
    glm::vec3 colNorth(0.9f, 0.7f, 0.7f);
    glm::vec3 colSouth(0.7f, 0.9f, 0.7f);
    glm::vec3 colWest(0.7f, 0.7f, 0.9f);
    glm::vec3 colEast(0.9f, 0.9f, 0.7f);

    auto pushOpaque = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const glm::vec3& col){
        uint32_t base = static_cast<uint32_t>(o_vertices.size());
        o_vertices.push_back({a,col}); o_vertices.push_back({b,col}); o_vertices.push_back({c,col}); o_vertices.push_back({d,col});
        o_indices.push_back(base+0); o_indices.push_back(base+1); o_indices.push_back(base+2);
        o_indices.push_back(base+0); o_indices.push_back(base+2); o_indices.push_back(base+3);
    };

    auto pushTransparent = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const glm::vec3& col){
        uint32_t base = static_cast<uint32_t>(t_vertices.size());
        t_vertices.push_back({a,col}); t_vertices.push_back({b,col}); t_vertices.push_back({c,col}); t_vertices.push_back({d,col});
        t_indices.push_back(base+0); t_indices.push_back(base+1); t_indices.push_back(base+2);
        t_indices.push_back(base+0); t_indices.push_back(base+2); t_indices.push_back(base+3);
    };

    // floor
    pushOpaque(p1,p2,p6,p5,colFloor);
    // ceiling
    pushOpaque(p4,p8,p7,p3,colCeiling);
    // north
    pushOpaque(p2,p1,p4,p3,colNorth);

    // south wall (with possible window/glass)
    // We'll push the wall strips to opaque, and push the glass quad (if any)
    // to the transparent mesh.
    auto P = [&](float x, float y) { return glm::vec3(x, y, +hz); };
    float x0 = -hx, x1 = +hx;
    float wxCenter = 0.0f;
    float wyCenter = height * 0.55f;
    float wWidth = width * 0.35f;
    float wHeight = height * 0.35f;

    float wx0 = wxCenter - wWidth  * 0.5f;
    float wx1 = wxCenter + wWidth  * 0.5f;
    float wy0 = wyCenter - wHeight * 0.5f;
    float wy1 = wyCenter + wHeight * 0.5f;
    const float eps = 1e-4f;
    wx0 = glm::clamp(wx0, x0 + eps, x1 - eps);
    wx1 = glm::clamp(wx1, x0 + eps, x1 - eps);
    wy0 = glm::clamp(wy0, y0 + eps, y1 - eps);
    wy1 = glm::clamp(wy1, y0 + eps, y1 - eps);

    if (wx1 <= wx0 + eps || wy1 <= wy0 + eps) {
        pushOpaque(P(x0,y0), P(x1,y0), P(x1,y1), P(x0,y1), colSouth);
    } else {
        if (wx0 > x0 + eps) pushOpaque(P(x0,y0), P(wx0,y0), P(wx0,y1), P(x0,y1), colSouth);
        if (wx1 < x1 - eps) pushOpaque(P(wx1,y0), P(x1,y0), P(x1,y1), P(wx1,y1), colSouth);
        if (wy0 > y0 + eps) pushOpaque(P(wx0,y0), P(wx1,y0), P(wx1,wy0), P(wx0,wy0), colSouth);
        if (wy1 < y1 - eps) pushOpaque(P(wx0,wy1), P(wx1,wy1), P(wx1,y1), P(wx0,y1), colSouth);
        if (addGlass) {
            float zGlass = hz - 0.01f;
            auto G = [&](float x, float y) { return glm::vec3(x, y, zGlass); };
            pushTransparent(G(wx0, wy0), G(wx1, wy0), G(wx1, wy1), G(wx0, wy1), glm::vec3(0.6f,0.8f,1.0f));
        }
    }

    // west and east
    pushOpaque(p1,p5,p8,p4,colWest);
    pushOpaque(p6,p2,p3,p7,colEast);

    Mesh opaque(o_vertices, o_indices);
    Mesh transparent(t_vertices, t_indices);
    return {std::move(opaque), std::move(transparent)};
}

Room::Room(float width, float height, float depth,
           const std::string& wallTexturePath,
           const std::string& ceilTexturePath,
           const std::string& floorTexturePath,
           bool addWindowGlass,
           const std::string& glassTexturePath)
{
    auto meshes = buildRoomMeshes(width, height, depth, addWindowGlass);
    m_opaqueMesh = std::make_unique<Mesh>(std::move(meshes.first));
    m_transparentMesh = std::make_unique<Mesh>(std::move(meshes.second));

    if (!wallTexturePath.empty()) {
        m_wallTex = Texture::load2D(wallTexturePath);
        if (m_wallTex == 0) std::cerr << "Room: failed to load wall texture: " << wallTexturePath << "\n";
    }
    if (!ceilTexturePath.empty()) {
        m_ceilTex = Texture::load2D(ceilTexturePath);
        if (m_ceilTex == 0) std::cerr << "Room: failed to load ceiling texture: " << ceilTexturePath << "\n";
    }
    if (!floorTexturePath.empty()) {
        m_floorTex = Texture::load2D(floorTexturePath);
        if (m_floorTex == 0) std::cerr << "Room: failed to load floor texture: " << floorTexturePath << "\n";
    }
    if (addWindowGlass && !glassTexturePath.empty()) {
        m_glassTex = Texture::load2D(glassTexturePath);
        if (m_glassTex == 0) std::cerr << "Room: failed to load glass texture: " << glassTexturePath << "\n";
    }
}

Room::~Room() {
    if (m_wallTex)  glDeleteTextures(1, &m_wallTex);
    if (m_ceilTex)  glDeleteTextures(1, &m_ceilTex);
    if (m_floorTex) glDeleteTextures(1, &m_floorTex);
    if (m_glassTex) glDeleteTextures(1, &m_glassTex);
}

void Room::draw(Shader& shader) const {
    shader.bind();

    // Use fixed texture unit mapping so the shader can always reference
    // the same sampler variables. Presence flags tell the shader which
    // textures are bound.
    shader.setInt("floorTex", 0);
    shader.setInt("wallTex", 1);
    shader.setInt("ceilTex", 2);
    shader.setInt("glassTex", 3);

    shader.setInt("uHasFloor", m_floorTex ? 1 : 0);
    shader.setInt("uHasWall",  m_wallTex  ? 1 : 0);
    shader.setInt("uHasCeil",  m_ceilTex  ? 1 : 0);
    shader.setInt("uHasGlass", m_glassTex ? 1 : 0);
    shader.setFloat("uGlassOpacity", 0.5f);

    if (m_floorTex) {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_floorTex);
    }
    if (m_wallTex) {
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, m_wallTex);
    }
    if (m_ceilTex) {
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, m_ceilTex);
    }
    if (m_glassTex) {
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, m_glassTex);
    }

    // First draw opaque mesh normally
    if (m_opaqueMesh && m_opaqueMesh->VAO() != 0) {
        m_opaqueMesh->draw();
    }

    // Then draw transparent mesh (glass) with blending and depth writes disabled
    if (m_transparentMesh && m_transparentMesh->VAO() != 0 && m_glassTex) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        m_transparentMesh->draw();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    // Unbind bound texture units
    for (int i = 0; i < 4; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}
