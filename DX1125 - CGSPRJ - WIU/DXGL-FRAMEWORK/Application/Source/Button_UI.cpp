#include "Button_UI.h"

GUI_Button::GUI_Button(glm::vec2 position, float width, float height)
	: position{ position }, width{ width }, height{ height }
{
}

bool GUI_Button::IsTouching(const float& mouseX, const float& mouseY) const
{
	float minX, maxX, minY, maxY;
	minX = position.x - width / 2.0f;
	maxX = position.x + width / 2.0f;
	minY = position.y - height / 2.0f;
	maxY = position.y + height / 2.0f;
	return mouseX > minX && mouseX < maxX && mouseY > minY && mouseY < maxY;
}

void GUI_Button::RenderButton(Scene& scene, int meshIndex, int textGEOIndex, glm::vec3 textColor, float textSize, glm::vec2 textOffset) const
{
	scene.RenderMeshOnScreen(scene.meshList[meshIndex], position.x, position.y, width, height);
	scene.RenderTextOnScreen(scene.meshList[textGEOIndex], label, textColor, textSize, position.x + textOffset.x, position.y + textOffset.y);
}
