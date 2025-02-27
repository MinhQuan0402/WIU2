#include "BP_Box.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneBalloon.h"
#include "MeshManager.h"

BP_Box::BP_Box()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void BP_Box::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_friction = 1.f;

	m_transform.ScaleBy(1.5, 0.5, 0.2);
	rb = addBoxCollider(this, 1.5, 0.5, 0.2, mat);
}

void BP_Box::Update()
{

}

void BP_Box::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_BP_COUNTER], true, m_transform);
}
