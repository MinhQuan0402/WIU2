#include "SpinyWater.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneDuckFishing.h"
#include "MeshBuilder.h"

SpinyWater::SpinyWater()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void SpinyWater::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	mat.m_bounciness = 5.0f;

	
	rb = addBoxCollider(this, 7.f, 0.5f, 0.5f, mat);
}

void SpinyWater::Update()
{
}

void SpinyWater::Render(Scene& scene)
{
}
