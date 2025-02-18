#include "PlinkoBall.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoBall::PlinkoBall()
{
	PhysicsMaterial mat;
	mat.m_mass = 1.0f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;
	rb = addSphereCollider(this, 1.f, mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoBall::Start()
{
	m_transform.Translate(0, 20, 0);
	m_transform.ScaleBy(0.8, 0.8, 0.8);
}

void PlinkoBall::Update()
{

}

void PlinkoBall::Render(Scene& scene)
{
	// Render model
	scene.RenderMesh(scene.meshList[ScenePlinko::GEO_SPHERE], false, m_transform);
}
