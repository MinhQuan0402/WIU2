#include "DS_Bullet.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Bullet::DS_Bullet() 
{	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Bullet::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 1;
	m_transform.ScaleBy(0.213822, 0.213822, 0.213822);
	rb = addCylinderCollider(this, 1.11 * m_transform.m_scale.x, 0.20 * m_transform.m_scale.x, mat);
}

void DS_Bullet::Update()
{
}

void DS_Bullet::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneDuckShooting::GEO_BULLET], scene.enableLight, m_transform);
}
