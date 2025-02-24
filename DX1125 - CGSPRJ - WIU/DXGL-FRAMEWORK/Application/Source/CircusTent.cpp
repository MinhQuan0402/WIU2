#include "CircusTent.h"
#include "RigidBody.h"
#include "carnivalroaming.h"
#include "GameObjectManager.h"

CircusTent::CircusTent()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	rb = addSphereCollider(this, 201, mat);
	GameObjectManager::GetInstance()->addItem(this);
}

CircusTent::~CircusTent()
{
}

void CircusTent::Start()
{

}

void CircusTent::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[carnivalroaming::GEO_SPHERE], false, m_transform);
}

void CircusTent::Update()
{
}
