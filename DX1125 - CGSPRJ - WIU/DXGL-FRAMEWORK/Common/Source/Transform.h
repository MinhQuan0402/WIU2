#pragma once

//GLM Headers
#include <glm\glm.hpp>
#include <iostream>
#include "MatrixStack.h"

class Transform
{
private:
	Transform* m_parent;

public:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	glm::vec3 m_worldPosition;

public:
	Transform(void);
	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	void Translate(const glm::vec3 translation);
	void Translate(const float x = 0.f, const float y = 0.f, const float z = 0.f);

	void Rotate(const glm::vec3 rotation);
	void Rotate(const float x = 0.f, const float y = 0.f, const float z = 0.f);

	void ScaleBy(const glm::vec3 scalar);
	void ScaleBy(const float x = 1.f, const float y = 1.f, const float z = 1.f);

	void GetTransformationMtx(MatrixStack& modelStack) const;
	void GetWorldTransformationMtx(MatrixStack& modelStack);

	void SetParent(Transform* parent);
	Transform* GetParent(void);

	void LoadbtTransform(void);

	void ResetScale(void);

	bool operator==(Transform& rhs);
	friend std::ostream& operator<<(std::ostream& os, Transform& rhs);
};

