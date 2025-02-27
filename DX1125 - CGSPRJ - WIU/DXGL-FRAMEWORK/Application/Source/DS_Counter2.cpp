#include "DS_Counter2.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"
#include "MeshManager.h"

DS_Counter2::DS_Counter2() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Counter2::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	mat.m_friction = 0.8;
	rb = addBoxCollider(this, 1, 2.25, 7.93374, mat);
}

void DS_Counter2::Update()
{
}

void DS_Counter2::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DS_COUNTER], scene.enableLight, m_transform);
}
