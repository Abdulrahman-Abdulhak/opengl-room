#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down
};

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    Camera(
        glm::vec3 position,
        glm::vec3 target,
        glm::vec3 worldUp,
        float fovDeg,
        float aspect,
        float nearPlane = 0.1f,
        float farPlane = 1000.0f
    );

    Camera(
        glm::vec3 position,
        float yawDeg,
        float pitchDeg,
        glm::vec3 worldUp,
        float fovDeg,
        float aspect,
        float nearPlane = 0.1f,
        float farPlane = 1000.0f
    );

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setAspect(float aspect);
    void setPerspective(float fovDeg, float nearPlane, float farPlane);

    void processKeyboard(CameraMovement direction, float deltaTime, float speed);

    void processMouseMovement(float xOffset, float yOffset, float sensitivity = 0.1f);

    void processMouseScroll(float yOffset);

private:
    void updateVectors();
};
