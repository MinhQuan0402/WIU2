#include "PoolFloor.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneDuckFishing.h"
#include "MeshBuilder.h"

PoolFloor::PoolFloor()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PoolFloor::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	
	m_transform.Translate(0, 2.5, 0);
	rb = addCylinderCollider(this, 1.f, 25.f, mat);
}

void PoolFloor::Update()
{
}

void PoolFloor::Render(Scene& scene)
{
}
