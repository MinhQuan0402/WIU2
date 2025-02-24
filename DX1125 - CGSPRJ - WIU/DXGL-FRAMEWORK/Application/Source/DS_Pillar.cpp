#include "DS_Pillar.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Pillar::DS_Pillar() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Pillar::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.ScaleBy(3, 10, 3);
	rb = addBoxCollider(this, 3, 10, 3, mat);
}

void DS_Pillar::Update()
{
}

void DS_Pillar::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneDuckShooting::GEO_PILLAR], scene.enableLight, m_transform);
}
