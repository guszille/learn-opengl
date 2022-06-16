#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float pitch = 0.0f, const float yaw = -90.0f);
	~Camera();

	static enum class Direction { FORWARD, BACKWARD, RIGHT, LEFT };

	const glm::mat4& getViewMatrix();
	const glm::vec3& getPosition();

	void setPosition(const float& speed, const Direction& movementDirection);
	void setDirection(const float& xOffset, const float& yOffset);

private:
	glm::vec3 m_Position, m_Direction, m_Up;
	glm::mat4 m_ViewMatrix;

	float m_Pitch, m_Yaw; // Euler angles.
};