#include "CK_Counter2.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Counter2::CK_Counter2() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Counter2::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	rb = addBoxCollider(this, 1, 2.65, 11, mat);
}

void CK_Counter2::Update()
{
}

void CK_Counter2::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneCanKnockdown::GEO_COUNTER], scene.enableLight, m_transform);
}
