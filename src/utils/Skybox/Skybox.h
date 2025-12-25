#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "utils/Shader/Shader.h"
#include "utils/Texture/Texture.h"
#include "utils/Camera/Camera.h"
#include "math/Mesh/Mesh.h"

class Skybox {
public:
    Skybox(const Mesh& cube, const std::string& hdrPath, int cubemapSize, int restoreW, int restoreH);
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void draw(const Camera& camera) const;

private:
    const Mesh* m_cube = NULL;
    Shader m_equirectToCube;
    Shader m_skyboxShader;
    unsigned int m_skyboxCubemap = 0;
};
