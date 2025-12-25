#pragma once

#include <string>
#include <vector>

#include "utils/Shader/Shader.h"

namespace Texture {
    unsigned int load2D(
        const std::string& path,
        bool flipVertically = true
    );

    unsigned int loadCubemap(
        const std::vector<std::string>& faces
    );
    unsigned int loadCubemap(
        const std::string& dir,
        const std::string& ext = "png"
    );

    unsigned int loadHDRI2D(const std::string& path);
    unsigned int createEmptyEnvCubemap(int size);
    unsigned int convertHDRIToCubemap(
        unsigned int hdrTex2D,
        Shader& shaderEquirectToCube,
        unsigned int cubeVAO,
        int cubemapSize,
        int restoreViewportW,
        int restoreViewportH
    );
}
