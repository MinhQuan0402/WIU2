#include "Ducks.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneDuckFishing.h"
#include "MeshBuilder.h"

Ducks::Ducks()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void Ducks::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.5f;
	mat.m_bounciness = 5.f;
	mat.m_friction = 0.5f;

	m_transform.ScaleBy(0.5, 0.5, 0.5);
	rb = addSphereCollider(this, 0.75f, mat);
	
}

void Ducks::Update()
{
	rb->setAngularVelocity(btVector3(0.0f, rb->getAngularVelocity().y(), 0.0f));
}

void Ducks::Render(Scene& scene)
{
	scene.meshList[SceneDuckFishing::GEO_FD_DUCKY]->material = Material::Plastic(YELLOW);
	scene.RenderRigidMesh(scene.meshList[SceneDuckFishing::GEO_FD_DUCKY], true, m_transform, rb);
}
