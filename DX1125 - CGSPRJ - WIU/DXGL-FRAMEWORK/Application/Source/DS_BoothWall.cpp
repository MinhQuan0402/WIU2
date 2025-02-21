#include "DS_BoothWall.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_BoothWall::DS_BoothWall() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_BoothWall::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.Translate(0, 4.39147, -6.06621 * 1.5);
	m_transform.ScaleBy(3, 1, 1.5);
	rb = addBoxCollider(this, 11.8194 * m_transform.m_scale.x, 8.90606 * m_transform.m_scale.y, 0.314395 * m_transform.m_scale.z, mat);
}

void DS_BoothWall::Update()
{
}

void DS_BoothWall::Render(Scene& scene)
{
}
