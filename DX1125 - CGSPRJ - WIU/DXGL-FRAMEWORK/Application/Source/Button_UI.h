#pragma once
#include <glm/ext/vector_float2.hpp>
#include "Scene.h"

struct Button_UI
{
	Button_UI(glm::vec2 position = glm::vec2{}, float width = 1.0f, float height = 1.0f);

	glm::vec2 position;
	float width, height;
	std::string label;

	bool IsTouching(const float& mouseX, const float& mouseY) const;

	void RenderButton(Scene& scene, int meshIndex, int textGEOIndex, 
		glm::vec3 textColor, float textSize,
		glm::vec2 textOffset = glm::vec2{}) const;
};
