#include "Texture.h"

#include <iostream>

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Texture {
    unsigned int load2D(const std::string& path, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);

        int width, height, channels;
        unsigned char* data = stbi_load(
            path.c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return 0;
        }

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLint prevAlign = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlign);
        int bytesPerRow = width * channels;
        bool changedAlign = false;
        if ((bytesPerRow % 4) != 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            changedAlign = true;
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        if (changedAlign) glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlign);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        return textureID;
    }

    unsigned int loadCubemap(const std::vector<std::string>& faces) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, channels;

        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char* data = stbi_load(
                faces[i].c_str(),
                &width,
                &height,
                &channels,
                0
            );

            if (!data) {
                std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
                continue;
            }

            GLenum format = channels == 4 ? GL_RGBA : GL_RGB;

            GLint prevAlignFace = 4;
            glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignFace);
            int bytesPerRowFace = width * channels;
            bool changedFace = false;
            if ((bytesPerRowFace % 4) != 0) {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                changedFace = true;
            }

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                format,
                width,
                height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data
            );

            if (changedFace) glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignFace);

            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
    unsigned int loadCubemap(const std::string& dir, const std::string& ext) {
        const std::array<std::string, 6> names = {
            "right",
            "left",
            "top",
            "bottom"
            "front",
            "back"
        };

        std::vector<std::string> faces;
        faces.reserve(6);
        for (const auto& n : names) {
            faces.push_back(dir + "/" + n + "." + ext);
        }

        return loadCubemap(faces);
    }

    unsigned int loadHDRI2D(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);

        int w, h, n;
        float* data = stbi_loadf(path.c_str(), &w, &h, &n, 0);
        if (!data) {
            std::cerr << "Failed to load HDR: " << path << "\n";
            return 0;
        }

        unsigned int hdrTex = 0;
        glGenTextures(1, &hdrTex);
        glBindTexture(GL_TEXTURE_2D, hdrTex);

        GLenum format = (n == 4) ? GL_RGBA : GL_RGB;

        GLint prevAlignHdr = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignHdr);
        int bytesPerRowHdr = w * n * static_cast<int>(sizeof(float));
        bool changedHdr = false;
        if ((bytesPerRowHdr % 4) != 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            changedHdr = true;
        }

        glTexImage2D(
            GL_TEXTURE_2D, 0,
            (format == GL_RGBA ? GL_RGBA16F : GL_RGB16F),
            w, h, 0,
            format,
            GL_FLOAT,
            data
        );

        if (changedHdr) glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignHdr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return hdrTex;
    }

    unsigned int createEmptyEnvCubemap(int size) {
        unsigned int tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

        for (int i = 0; i < 6; i++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB16F,
                size, size, 0,
                GL_RGB,
                GL_FLOAT,
                nullptr
            );
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return tex;
    }

    static const glm::mat4 CAPTURE_PROJECTION = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    static const glm::mat4 CAPTURE_VIEWS[6] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3(0.0f,-1.0f, 0.0f))
    };

    unsigned int convertHDRIToCubemap(
        unsigned int hdrTex2D,
        Shader& shaderEquirectToCube,
        unsigned int cubeVAO,
        int cubemapSize,
        int restoreViewportW,
        int restoreViewportH
    ) {
        unsigned int envCubemap = createEmptyEnvCubemap(cubemapSize);

        unsigned int captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemapSize, cubemapSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        shaderEquirectToCube.bind();
        shaderEquirectToCube.setInt("equirectangularMap", 0);
        shaderEquirectToCube.setMat4("projection", CAPTURE_PROJECTION);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTex2D);

        glViewport(0, 0, cubemapSize, cubemapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        for (unsigned int i = 0; i < 6; i++)
        {
            shaderEquirectToCube.setMat4("view", CAPTURE_VIEWS[i]);
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                envCubemap, 0
            );
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(cubeVAO);

            GLint elementBuffer = 0;
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBuffer);

            if (elementBuffer != 0) {
                GLint bufSize = 0;
                glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufSize);
                GLsizei indexCount = static_cast<GLsizei>(bufSize / sizeof(unsigned int));
                glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDeleteFramebuffers(1, &captureFBO);
        glDeleteRenderbuffers(1, &captureRBO);

        glViewport(0, 0, restoreViewportW, restoreViewportH);

        return envCubemap;
    }

}