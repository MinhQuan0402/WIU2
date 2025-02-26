#include "Ducks.h"
#include "RigidBody.h"
#include "GameObjectManager.h"
#include "SceneDuckFishing.h"

Ducks::Ducks()
{
	GameObjectManager::GetInstance()->addItem(this);
}

void Ducks::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.1f;
	mat.m_bounciness = 0.2f;
	mat.m_friction = 1.0f;

	rb = addSphereCollider(this, 1.0f, mat);
}

void Ducks::Update()
{
}

void Ducks::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneDuckFishing::GEO_])
}
