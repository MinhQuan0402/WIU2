#include "PlinkoGlass.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoGlass::PlinkoGlass()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoGlass::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_friction = 0.5f;
	mat.m_bounciness = 0.6f;
	m_transform.Translate();

	rb = addBoxCollider(this, 0.05f, 6.5f, 3.f, mat);
}

void PlinkoGlass::Update()
{

}

void PlinkoGlass::Render(Scene& scene)
{

}
