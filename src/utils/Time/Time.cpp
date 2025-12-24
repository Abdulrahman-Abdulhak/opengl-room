#include "Time.h"

#include <GLFW/glfw3.h>

namespace Time {
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    void update() {
        float currentFrame = static_cast<float>(glfwGetTime());
        
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
}
