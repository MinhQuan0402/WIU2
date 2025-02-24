#include "PlinkoCylinder.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "ScenePlinko.h"

PlinkoCylinder::PlinkoCylinder()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void PlinkoCylinder::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;
	m_transform.Translate(-0.06, 4.978, 0);
	m_transform.Rotate(0, 0, 90);
	
	rb = addCylinderCollider(this, 0.17f, 0.04f, mat);
}

void PlinkoCylinder::Update()
{

}

void PlinkoCylinder::Render(Scene& scene)
{
	// Render model
	//scene.RenderMesh(scene.meshList[ScenePlinko::GEO_CYLINDER], false, m_transform);
}
