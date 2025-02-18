#include "CK_Cylinder.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Cylinder::CK_Cylinder() {
	PhysicsMaterial mat;
	rb = addCylinderCollider(this, 1, 1, (PhysicsMaterial&)mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Cylinder::Start()
{
}

void CK_Cylinder::Update()
{
}

void CK_Cylinder::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneCanKnockdown::GEO_CYLINDER], false, m_transform);
}
