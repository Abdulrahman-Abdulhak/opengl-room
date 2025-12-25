#include <vector>
#include <glm/glm.hpp>

#include "math/Vertex.h"
#include "math/Mesh/Mesh.h"

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

Mesh createRoomMesh(float width, float height, float depth) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(24);
    indices.reserve(36);

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

    pushQuadInward(vertices, indices,
        p1, p2, p6, p5,
        colFloor
    );

    pushQuadInward(vertices, indices,
        p4, p8, p7, p3,
        colCeiling
    );

    pushQuadInward(vertices, indices,
        p2, p1, p4, p3,
        colNorth
    );

    pushQuadInward(vertices, indices,
        p5, p6, p7, p8,
        colSouth
    );

    pushQuadInward(vertices, indices,
        p1, p5, p8, p4,
        colWest
    );

    pushQuadInward(vertices, indices,
        p6, p2, p3, p7,
        colEast
    );

    Mesh mesh(vertices, indices);
    return mesh;
}