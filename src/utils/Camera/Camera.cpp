#include <algorithm>

#include "Camera.h"

Camera::Camera(
    glm::vec3 position,
    glm::vec3 target,
    glm::vec3 worldUp,
    float fovDeg,
    float aspect,
    float nearPlane,
    float farPlane
)
    : position(position),
      worldUp(worldUp),
      fov(fovDeg),
      aspect(aspect),
      nearPlane(nearPlane),
      farPlane(farPlane)
{
    front = glm::normalize(target - position);

    yaw   = glm::degrees(std::atan2(front.z, front.x));
    float lenXZ = std::sqrt(front.x * front.x + front.z * front.z);
    pitch = glm::degrees(std::atan2(front.y, lenXZ));

    updateVectors();
}

Camera::Camera(
    glm::vec3 position,
    float yawDeg,
    float pitchDeg,
    glm::vec3 worldUp,
    float fovDeg,
    float aspect,
    float nearPlane,
    float farPlane
)
    : position(position),
      worldUp(worldUp),
      yaw(yawDeg),
      pitch(pitchDeg),
      fov(fovDeg),
      aspect(aspect),
      nearPlane(nearPlane),
      farPlane(farPlane)
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::setAspect(float aspect) {
    this->aspect = aspect;
}

void Camera::setPerspective(float fovDeg, float nearPlane, float farPlane) {
    this->fov = fovDeg;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime, float speed) {
    float velocity = speed * deltaTime;
    if (direction == CameraMovement::Forward)
        position += front * velocity;
    if (direction == CameraMovement::Backward)
        position -= front * velocity;
    if (direction == CameraMovement::Left)
        position -= right * velocity;
    if (direction == CameraMovement::Right)
        position += right * velocity;
    if (direction == CameraMovement::Up)
        position += worldUp * velocity;
    if (direction == CameraMovement::Down)
        position -= worldUp * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset, float sensitivity) {
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera::processMouseScroll(float yOffset) {
    fov -= yOffset;
    fov = std::clamp(fov, 10.0f, 90.0f);
}

void Camera::updateVectors() {
    float yawRad   = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    front.x = std::cos(pitchRad) * std::cos(yawRad);
    front.y = std::sin(pitchRad);
    front.z = std::cos(pitchRad) * std::sin(yawRad);
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
