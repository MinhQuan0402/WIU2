#include "PlinkoGlass.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoGlass::PlinkoGlass()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;
	rb = addBoxCollider(this, 1.f, 1.f, 1.f, mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoGlass::Start()
{

}

void PlinkoGlass::Update()
{

}

void PlinkoGlass::Render(Scene& scene)
{

}
