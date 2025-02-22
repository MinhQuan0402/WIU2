#include "FPCamera.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "Time.h"

//Include GLFW
#include <GLFW/glfw3.h>
#include <iostream>

FPCamera::FPCamera() : isDirty(false), right{}, view{}, sensitivity{ 10.0f }
{
	this->target = glm::vec3(0, 0, 0);
	this->up = glm::vec3(0, 1, 0);
}

FPCamera::~FPCamera()
{
}

void FPCamera::Init(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	this->m_transform.m_position = position;
	this->target = target;
	this->up = up;
	this->isDirty = true;

	Refresh();
}

void FPCamera::Update(void)
{
	view = glm::normalize(target - m_transform.m_position); //calculate the new view vector
	right = glm::normalize(glm::cross(view, up));
}

void FPCamera::UpdateCameraPosition(float x, float y, float z)
{
	m_transform.m_position.x = x;
	m_transform.m_position.y = y;
	m_transform.m_position.z = z;
}

void FPCamera::UpdateCameraRotation(void)
{
	float deltaX, deltaY;
	deltaX = (float)MouseController::GetInstance()->GetMouseDeltaX();
	deltaY = (float)MouseController::GetInstance()->GetMouseDeltaY();

	//Yaw update
	float yawAngle = -deltaX * sensitivity * (float)Time::deltaTime;
	m_transform.m_rotation.y += yawAngle;

	glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(yawAngle), up);
	glm::vec3 yawView = yaw * glm::vec4(view, 0.f);

	// Pitch update
	float pitchAngle = deltaY * sensitivity * (float)Time::deltaTime;
	m_transform.m_rotation.x += pitchAngle;

	if ((view.y >= 0.9f && deltaY > 0.0f) || (view.y <= -0.9f && deltaY < 0.0f)) pitchAngle = 0.0f;

	glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(pitchAngle), right);
	glm::vec3 pitchView = pitch * glm::vec4(view, 0.f);

	// Recalculate up vector and target position
	target = m_transform.m_position + yawView + pitchView;
}

void FPCamera::Refresh()
{
	if (!isDirty) return;

	glm::vec3 view = glm::normalize(target - m_transform.m_position);
	glm::vec3 right = glm::normalize(glm::cross(up, view));
	// Recalculate the up vector
	this->up = glm::normalize(glm::cross(view, right));

	this->isDirty = false;
}