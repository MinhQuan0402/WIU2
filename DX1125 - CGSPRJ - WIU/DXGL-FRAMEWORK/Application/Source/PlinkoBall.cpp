#include "PlinkoBall.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoBall::PlinkoBall()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoBall::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 1.0f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;

	m_transform.Translate(-0.05, 7, 0.0001);
	rb = addSphereCollider(this, 0.1f, mat);
}

void PlinkoBall::Update()
{

}

void PlinkoBall::Render(Scene& scene)
{
	// Render model
	//scene.RenderMesh(scene.meshList[ScenePlinko::GEO_SPHERE], false, m_transform);
}
