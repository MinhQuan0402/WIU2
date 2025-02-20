#include "CK_Can.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Can::CK_Can() {
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Can::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 1.f;
	mat.m_friction = 0.3;
	m_transform.ScaleBy(glm::vec3(0.092));
	rb = addCylinderCollider(this, 0.67, 0.48, mat);
}

void CK_Can::Update()
{
}

void CK_Can::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneCanKnockdown::GEO_CAN], scene.enableLight, m_transform, rb);
}
