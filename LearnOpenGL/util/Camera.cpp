#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float pitch, const float yaw)
	: m_Position(position), m_Direction(direction), m_Up(up), m_Pitch(pitch), m_Yaw(yaw)
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
}

Camera::~Camera()
{
}

const glm::mat4& Camera::getViewMatrix()
{
	return m_ViewMatrix;
}

const glm::vec3& Camera::getPosition()
{
	return m_Position;
}

const glm::vec3& Camera::getDirection()
{
	return m_Direction;
}

void Camera::setPosition(const float& speed, const Direction& movementDirection)
{
	switch (movementDirection)
	{
	case Direction::FORWARD:
		m_Position += speed * m_Direction;
		break;

	case Direction::BACKWARD:
		m_Position -= speed * m_Direction;
		break;

	case Direction::RIGHT:
		m_Position += glm::normalize(glm::cross(m_Direction, m_Up)) * speed;
		break;

	case Direction::LEFT:
		m_Position -= glm::normalize(glm::cross(m_Direction, m_Up)) * speed;
		break;

	default:
		std::cout << "Camera direction not supported" << std::endl;
		break;
	}

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
}

void Camera::setDirection(const float& xOffset, const float& yOffset)
{
	glm::vec3 direction(0.0f);

	m_Yaw += xOffset;
	m_Pitch += yOffset;

	m_Pitch = std::min(std::max(m_Pitch, -89.0f), 89.0f);

	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

	m_Direction = glm::normalize(direction);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
}