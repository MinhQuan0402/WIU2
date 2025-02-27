#include "DS_Bullet.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"
#include "MeshManager.h"
#include "RigidBody.h"

DS_Bullet::DS_Bullet() : hit{}, timeoutTime{}
{	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Bullet::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.5;
	mat.m_bounciness = 1;
	m_transform.ScaleBy(0.05, 0.05, 0.05);
	rb = addSphereCollider(this, 0.05, mat);
}

void DS_Bullet::Update()
{
}

void DS_Bullet::Render(Scene& scene)
{
	scene.RenderRigidMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_DS_BULLET], scene.enableLight, m_transform, rb);
}
