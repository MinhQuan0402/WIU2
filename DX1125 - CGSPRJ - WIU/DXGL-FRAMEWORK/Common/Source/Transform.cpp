#include "Transform.h"
#include <stdexcept>
#include <glm\gtc\matrix_transform.hpp>

Transform::Transform(void)
{
	m_parent = nullptr;
	m_position = m_rotation = glm::vec3(0.f);
	m_scale = glm::vec3(1.0f);
	m_worldPosition = glm::vec3{};
}
Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	m_parent = nullptr;
	this->Translate(position);
	this->Rotate(rotation);
	this->ScaleBy(scale);
	m_worldPosition = glm::vec3{};
}

void Transform::Translate(const glm::vec3 translation) { m_position += translation; }
void Transform::Translate(const float x, const float y, const float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

void Transform::Rotate(const glm::vec3 rotation) { m_rotation += rotation; }
void Transform::Rotate(const float x, const float y, const float z)
{
	m_rotation.x += x;
	m_rotation.y += y;
	m_rotation.z += z;
}

void Transform::ScaleBy(const glm::vec3 scalar)
{
	if(scalar.x <= 0 || scalar.y <= 0 || scalar.z <= 0)	throw std::invalid_argument("Value cannot be less than or equal to zero!");

	m_scale = scalar;
}
void Transform::ScaleBy(const float x, const float y, const float z)
{
	if(x <= 0 || y <= 0 || z <= 0) throw std::invalid_argument("Value cannot be less than or equal to zero!");

	m_scale = glm::vec3(x, y, z);
}

void Transform::GetTransformationMtx(MatrixStack& modelStack) const
{
	modelStack.PushMatrix();

	modelStack.Translate(m_position.x, m_position.y, m_position.z);

	glm::mat4 rotationX, rotationY, rotationZ;
	rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
	rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
	rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
	modelStack.MultMatrix(rotationX * rotationY * rotationZ);

	modelStack.Scale(m_scale.x, m_scale.y, m_scale.z);
}

void Transform::GetWorldTransformationMtx(MatrixStack& modelStack)
{
	if (m_parent) m_parent->GetWorldTransformationMtx(modelStack);
	glm::mat4 worldMatrix = modelStack.Top();
	m_worldPosition = glm::vec3{ worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2] };
	return GetTransformationMtx(modelStack);
}

void Transform::SetParent(Transform* parent) 
{
	if (parent == this)	return;
	m_parent = parent; 
}
Transform* Transform::GetParent(void) { return m_parent; }

void Transform::LoadbtTransform(void)
{
}

void Transform::ResetScale(void)
{
	m_scale = glm::vec3(1.0f);
}

bool Transform::operator==(Transform& rhs) { return this == &rhs; }

std::ostream& operator<<(std::ostream& os, Transform& rhs)
{
	os << "Position: (" << rhs.m_position.x << ", " << rhs.m_position.y << ", " << rhs.m_position.z << ")" << std::endl;
	os << "Rotation: (" << rhs.m_rotation.x << ", " << rhs.m_rotation.y << ", " << rhs.m_rotation.z << ")" << std::endl;
	os << "Scale: (" << rhs.m_scale.x << ", " << rhs.m_scale.y << ", " << rhs.m_scale.z << ")" << std::endl;
	return os;
}