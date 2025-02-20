#include "TossBoard.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneRingToss.h"

Bottle::Bottle(glm::vec3 color)
	: m_color{ color }
{
	rbGOType = DYNAMIC;
	GameObjectManager::addItem(this);
}

void Bottle::Start(void)
{

}

void Bottle::Update(void)
{
}

void Bottle::LateUpdate(void)
{
}

void Bottle::Render(Scene& scene)
{
}

void TossBoard::Start(void)
{
}

void TossBoard::Update(void)
{
}

void TossBoard::LateUpdate(void)
{
}

void TossBoard::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneRingToss::GEO_BOARD], false, m_transform);
}
