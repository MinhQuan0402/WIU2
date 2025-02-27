#include "DS_Conveyor.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"
#include "MeshManager.h"
#include "RigidBody.h"
DS_Conveyor::DS_Conveyor() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Conveyor::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.Translate(0, 2.3 * 0.5, -4.78361);
	rb = addBoxCollider(this, 24.3538, 2.3, 1.94702, mat);

	m_transform.Translate(0, -0.1, 0);
	m_transform.Rotate(0, 90, 0);
	m_transform.ScaleBy(1.2142, 1, 1.15754);
}

void DS_Conveyor::Update()
{
}

void DS_Conveyor::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DS_CONVEYOR], scene.enableLight, m_transform);
}
