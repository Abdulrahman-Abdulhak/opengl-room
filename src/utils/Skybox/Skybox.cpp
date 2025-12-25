#include "Skybox.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

Skybox::Skybox(const Mesh& cube, const std::string& hdrPath, int cubemapSize, int restoreW, int restoreH)
    : m_cube(&cube),
      m_equirectToCube("equirect_to_cubemap"),
      m_skyboxShader("skybox"),
      m_skyboxCubemap(0)
{
    unsigned int hdr2D = Texture::loadHDRI2D(hdrPath);
    if (hdr2D == 0) {
        std::cerr << "Skybox: Failed to load HDRI: " << hdrPath << "\n";
        return;
    }

    m_skyboxCubemap = Texture::convertHDRIToCubemap(
        hdr2D,
        m_equirectToCube,
        cube.VAO(),
        cubemapSize,
        restoreW,
        restoreH
    );

    m_skyboxShader.bind();
    m_skyboxShader.setInt("skybox", 0);

    if (hdr2D) glDeleteTextures(1, &hdr2D);
}

Skybox::~Skybox() {
    if (m_skyboxCubemap) glDeleteTextures(1, &m_skyboxCubemap);
}

void Skybox::draw(const Camera& camera) const {
    if (!m_skyboxCubemap || !m_cube) return;

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    m_skyboxShader.bind();

    glm::mat4 proj = camera.getProjectionMatrix();
    glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));

    m_skyboxShader.setMat4("proj", proj);
    m_skyboxShader.setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxCubemap);
    m_cube->draw();

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}
