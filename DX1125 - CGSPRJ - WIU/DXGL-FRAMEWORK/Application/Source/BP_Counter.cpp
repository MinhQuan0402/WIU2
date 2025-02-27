#include "BP_Counter.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"
#include "MeshManager.h"
BP_Counter::BP_Counter()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void BP_Counter::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_friction = 1.f;
	m_transform.ScaleBy(1.5, 2.5, 16);

	rb = addBoxCollider(this, 1.5, 2.5, 16, mat);
}

void BP_Counter::Update()
{

}

void BP_Counter::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_COUNTER], true, m_transform);
}
