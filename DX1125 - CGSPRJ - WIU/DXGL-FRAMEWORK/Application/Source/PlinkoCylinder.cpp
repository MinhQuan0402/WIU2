#include "PlinkoCylinder.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoCylinder::PlinkoCylinder()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;
	rb = addCylinderCollider(this, 2.f, 1.f, mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoCylinder::Start()
{
	m_transform.Translate(0, 5, 0);
	m_transform.Rotate(0, 0, 90);
	m_transform.ScaleBy(0.3, 0.9, 0.3);
}

void PlinkoCylinder::Update()
{

}

void PlinkoCylinder::Render(Scene& scene)
{
	// Render model
	scene.RenderMesh(scene.meshList[ScenePlinko::GEO_CYLINDER], false, m_transform);
}
