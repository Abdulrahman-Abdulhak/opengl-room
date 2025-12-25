#include "Primitives.h"

namespace Primitives {
    Mesh Cube(float size) {
        std::vector<Vertex> cubeVertices = {
            // * Front (+Z)
            {{-size/2, -size/2,  size/2}, {1, 0, 0}},
            {{ size/2, -size/2,  size/2}, {1, 0, 0}},
            {{ size/2,  size/2,  size/2}, {1, 0, 0}},
            {{-size/2,  size/2,  size/2}, {1, 0, 0}},

            // * Back (-Z)
            {{ size/2, -size/2, -size/2}, {0, 1, 0}},
            {{-size/2, -size/2, -size/2}, {0, 1, 0}},
            {{-size/2,  size/2, -size/2}, {0, 1, 0}},
            {{ size/2,  size/2, -size/2}, {0, 1, 0}},

            // * Left (-X)
            {{-size/2, -size/2, -size/2}, {0, 0, 1}},
            {{-size/2, -size/2,  size/2}, {0, 0, 1}},
            {{-size/2,  size/2,  size/2}, {0, 0, 1}},
            {{-size/2,  size/2, -size/2}, {0, 0, 1}},

            // * Right (+X)
            {{ size/2, -size/2,  size/2}, {1, 1, 0}},
            {{ size/2, -size/2, -size/2}, {1, 1, 0}},
            {{ size/2,  size/2, -size/2}, {1, 1, 0}},
            {{ size/2,  size/2,  size/2}, {1, 1, 0}},

            // * Top (+Y)
            {{-size/2,  size/2,  size/2}, {0, 1, 1}},
            {{ size/2,  size/2,  size/2}, {0, 1, 1}},
            {{ size/2,  size/2, -size/2}, {0, 1, 1}},
            {{-size/2,  size/2, -size/2}, {0, 1, 1}},

            // * Bottom (-Y)
            {{-size/2, -size/2, -size/2}, {1, 0, 1}},
            {{ size/2, -size/2, -size/2}, {1, 0, 1}},
            {{ size/2, -size/2,  size/2}, {1, 0, 1}},
            {{-size/2, -size/2,  size/2}, {1, 0, 1}},
        };
        std::vector<uint32_t> cubeIndices = {
            0, 1, 2, 2, 3, 0,  // * Front
            4, 5, 6, 6, 7, 4,  // * Back
            8, 9,10,10,11, 8,  // * Left
            12,13,14,14,15,12, // * Right
            16,17,18,18,19,16, // * Top
            20,21,22,22,23,20  // * Bottom
        };

        return Mesh{cubeVertices, cubeIndices};
    }
}