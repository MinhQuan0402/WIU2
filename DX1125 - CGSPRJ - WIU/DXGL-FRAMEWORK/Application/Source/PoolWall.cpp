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
	
	rb = addTorusCollider(this, 5.5f, 2.0f, 100, mat);
}

void PoolWall::Update()
{
}

void PoolWall::Render(Scene& scene)
{
	//scene.RenderRigidMesh(scene.meshList[SceneDuckFishing::GEO_FD_POOL], scene.enableLight, m_transform, rb);
}
