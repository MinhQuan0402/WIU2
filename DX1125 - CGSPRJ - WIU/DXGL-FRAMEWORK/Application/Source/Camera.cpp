#include "Camera.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "Time.h"
#include <glm/ext/matrix_transform.hpp>

glm::vec3 Camera::CalculateTarget(void)
{
	glm::vec3 direction{};
	direction.x = cos(glm::radians(m_transform.m_rotation.y)) * cos(glm::radians(m_transform.m_rotation.x));
	direction.y = sin(glm::radians(m_transform.m_rotation.x));
	direction.z = sin(glm::radians(m_transform.m_rotation.y)) * cos(glm::radians(m_transform.m_rotation.x));
	return direction;
}

Camera::Camera(void)
{
	isDirty = false;
	cameraTarget = CalculateTarget();
	cameraUp = glm::vec3(0.f, 1.f, 0.f);
	movementSpeed = 12;
	fov = 45.f;
}

Camera::~Camera(void)
{
}

void Camera::Init(glm::vec3 position, glm::vec3 rotation, glm::vec3 up)
{
	m_transform.m_position = position;
	m_transform.m_rotation = rotation;
	cameraUp = up;
}

void Camera::Update(void)
{
	//Camera WASD movement
	{
		float cameraSpeed = movementSpeed * (float)Time::deltaTime;
		glm::vec3 rawMovementDirection = glm::vec3(0, 0, 0);
		if (KeyboardController::GetInstance()->IsKeyDown('D'))
			m_transform.m_position += glm::normalize(glm::cross(cameraTarget, cameraUp)) * cameraSpeed;
		if (KeyboardController::GetInstance()->IsKeyDown('A'))
			m_transform.m_position -= glm::normalize(glm::cross(cameraTarget, cameraUp)) * cameraSpeed;
		if (KeyboardController::GetInstance()->IsKeyDown('W'))
			m_transform.m_position += cameraSpeed * cameraTarget;
		if (KeyboardController::GetInstance()->IsKeyDown('S'))
			m_transform.m_position -= cameraSpeed * cameraTarget;
		if (KeyboardController::GetInstance()->IsKeyDown('E'))
			m_transform.m_position += cameraUp * cameraSpeed;
		if (KeyboardController::GetInstance()->IsKeyDown('Q'))
			m_transform.m_position -= cameraUp * cameraSpeed;
	}

	//Camera zoom
	{
		double mouseScrollY = MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET);
		double mouseScrollX = MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET);
		fov -= (float)mouseScrollY * 0.1f;
		fov += (float)mouseScrollX * 0.1f;
		if (fov < 44.3f)
			fov = 44.3f;
		if (fov > 46.4f)
			fov = 46.4f;
	}

	//Camera look rotation
	{
		double deltaX = 0, deltaY = 0;
		deltaX = MouseController::GetInstance()->GetMouseDeltaX();
		deltaY = MouseController::GetInstance()->GetMouseDeltaY();

		float sensitivity = 0.2f;
		if (deltaX > 0 || deltaY > 0)
		{
			m_transform.m_rotation.y += (float)deltaX * sensitivity;
			m_transform.m_rotation.x += (float)deltaY * sensitivity;
		}

		if (m_transform.m_rotation.x > 89.0f)
			m_transform.m_rotation.x = 89.0f;
		if (m_transform.m_rotation.x < -89.0f)
			m_transform.m_rotation.x = -89.0f;

		cameraTarget = CalculateTarget();
	}
}
