#include "PlinkoPointsWall.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoPointsWall::PlinkoPointsWall()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoPointsWall::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_friction = 0.5f;
	mat.m_bounciness = 0.6f;
	m_transform.Translate(-0.08, 1.3, 1.35);

	rb = addBoxCollider(this, 0.15f, 1.1f, 0.05f, mat);
}

void PlinkoPointsWall::Update()
{

}

void PlinkoPointsWall::Render(Scene& scene)
{

}
