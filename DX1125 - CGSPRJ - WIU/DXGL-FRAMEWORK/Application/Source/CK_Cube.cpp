#include "CK_Cube.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Cube::CK_Cube() {
	PhysicsMaterial mat;
	rb = addBoxCollider(this, 1, 1, 1, (PhysicsMaterial&)mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Cube::Start()
{
}

void CK_Cube::Update()
{
}

void CK_Cube::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneCanKnockdown::GEO_HITBOX], false, m_transform);
}
