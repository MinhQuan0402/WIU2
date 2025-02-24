#include "PlinkoWall.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoWall::PlinkoWall()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoWall::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_friction = 0.5f;
	mat.m_bounciness = 0.6f;
	m_transform.Translate(-0.08, 0, 0);
	m_transform.Rotate(30, 0, 0);

	rb = addBoxCollider(this, 0.15f, 0.25f, 0.05f, mat);
}

void PlinkoWall::Update()
{

}

void PlinkoWall::Render(Scene& scene)
{
	//scene.RenderMesh(scene.meshList[ScenePlinko::GEO_CUBE], false, m_transform);
}
