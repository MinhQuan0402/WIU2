#include "Light.h"

Light::Light(void) : color(1, 1, 1), power(1),
kC(1), kL(0.1f), kQ(0.001f), spotDirection(1.f), cosCutoff(45.f),
cosInner(30.f), exponent(1.f), type(LIGHT_POINT), tempPow{ 0.0f }
{
}

Light::~Light(void)
{
}

void Light::EnableLight(bool on)
{
	if (!on)
	{
		tempPow = power;
		power = 0.0f;
	}
	else
		power = tempPow;
}

void Light::UpdateLight(glm::vec3 pos, glm::vec3 direction)
{
	m_transform.m_position = pos;
	
	glm::vec3 angleInRadians = glm::radians(direction);

	float yaw = angleInRadians.y;   // Y-axis rotation
	float pitch = angleInRadians.x; // X-axis rotation

	// Calculate facing direction
	float x = cos(pitch) * sin(yaw);
	float y = sin(pitch);
	float z = cos(pitch) * cos(yaw);

	spotDirection = -glm::normalize(glm::vec3(x, y, z));
}
