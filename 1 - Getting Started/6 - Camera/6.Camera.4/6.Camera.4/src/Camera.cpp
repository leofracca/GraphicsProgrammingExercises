#include "Camera.h"

Camera::Camera(glm::vec3 position)
    : Position(position)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Position(glm::vec3(posX, posY, posZ)), WorldUp(glm::vec3(upX, upY, upZ)), Yaw(yaw), Pitch(pitch)
{
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    // return glm::lookAt(Position, Position + Front, Up);

    // Using glm::lookAt function written from scratch
    
    // Camera direction (direction vector or forward vector)
    glm::vec3 directionVector = glm::normalize(Position - (Position + Front));      // D
    // Right axis direction
    glm::vec3 rightVector = glm::normalize(glm::cross(WorldUp, directionVector));   // R
    // Camera Up axis
    glm::vec3 upVector = glm::normalize(glm::cross(directionVector, rightVector));  // U

    // Create rotation and translation matrices
    // Remember they are column major!!!
    glm::mat4 rotation(1.0f);
    rotation[0][0] = rightVector.x;
    rotation[1][0] = rightVector.y;
    rotation[2][0] = rightVector.z;
    rotation[0][1] = upVector.x;
    rotation[1][1] = upVector.y;
    rotation[2][1] = upVector.z;
    rotation[0][2] = directionVector.x;
    rotation[1][2] = directionVector.y;
    rotation[2][2] = directionVector.z;

    glm::mat4 translation(1.0f);
    translation[3][0] = -Position.x;
    translation[3][1] = -Position.y;
    translation[3][2] = -Position.z;

    glm::mat4 lookAt = rotation * translation;
    return lookAt;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front(
        cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
        sin(glm::radians(Pitch)),
        sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))
    );
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
