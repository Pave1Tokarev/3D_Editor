#include "camera.h"
#include <iostream>
#include <vector>
#include <string>
#include "Vertex.h"

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == 0) {
        Position += Front * velocity;
    }
    if (direction == 1) {
        Position -= Front * velocity;
    }
    if (direction == 2) {
        Position -= Right * velocity;
    }
    if (direction == 3) {
        Position += Right * velocity;
    }

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch ) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;
    if (constrainPitch) {
        if (Pitch > 89.0f) {
            Pitch = 89.0f;
        }
        if (Pitch < -89.0f) {
            Pitch = -89.0f;
        }

    }
    updateCameraVectors();
}

Camera::Camera(glm::vec3 position , glm::vec3 up , float yaw , float pitch ) {
    this->Position = position;
    this->Up = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    MovementSpeed = 7.5f;
    MouseSensitivity = 0.1f;
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}