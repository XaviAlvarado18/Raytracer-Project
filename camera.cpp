#include "camera.h"
#include <glm/glm.hpp>          // for basic glm types
#include <glm/gtc/quaternion.hpp> // for glm::quat and glm::angleAxis
#include <glm/gtc/matrix_transform.hpp> // for glm::radian

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up, float rotationSpeed)
    : position(position), target(target), up(up), rotationSpeed(rotationSpeed) {
    up = glm::vec3(0, 1, 0);
}

void Camera::rotate(float deltaX, float deltaY) {
    // Create quaternions representing the rotation around the y and x axis
    glm::quat quatAroundY = glm::angleAxis(glm::radians(deltaX * rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quatAroundX = glm::angleAxis(glm::radians(deltaY * rotationSpeed), glm::vec3(1.0f, 0.0f, 0.0f));

    // Rotate the position of the camera around the target.
    position = target + quatAroundY * (position - target);  // First rotate around the Y-axis
    position = target + quatAroundX * (position - target);  // Then rotate around the X-axis
}

void Camera::move(float deltaZ) {
    glm::vec3 dir = glm::normalize(target - position);  // Get the direction vector
    position += dir * deltaZ;  // Move the camera
}
