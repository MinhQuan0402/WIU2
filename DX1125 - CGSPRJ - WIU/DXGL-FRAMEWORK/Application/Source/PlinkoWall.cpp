#include "PlinkoWall.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoWall::PlinkoWall()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_friction = 0.5f;
	mat.m_bounciness = 0.6f;
	rb = addBoxCollider(this, 1.f, 1.f, 1.f, mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoWall::Start()
{
	m_transform.ScaleBy(1.5f, 1.f, 19.f);
}

void PlinkoWall::Update()
{

}

void PlinkoWall::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[ScenePlinko::GEO_CUBE], false, m_transform);
}
