#include "DS_Counter.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"
#include "MeshManager.h"

DS_Counter::DS_Counter() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Counter::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	mat.m_friction = 0.8;
	rb = addBoxCollider(this, 30, 2.25, 1, mat);
}

void DS_Counter::Update()
{
}

void DS_Counter::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DS_COUNTER], scene.enableLight, m_transform);
}
