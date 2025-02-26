#include "PoolWall.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneDuckFishing.h"
#include "MeshBuilder.h"

PoolWall::PoolWall()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PoolWall::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	mat.m_bounciness = 3.0f;
	m_transform.Rotate(90, 0, 0);
	rb = addTorusCollider(this, 12.f, 5.0f, 100, mat);
}

void PoolWall::Update()
{
}

void PoolWall::Render(Scene& scene)
{
	//scene.RenderRigidMesh(scene.meshList[SceneDuckFishing::GEO_FD_POOL], scene.enableLight, m_transform, rb);
}
