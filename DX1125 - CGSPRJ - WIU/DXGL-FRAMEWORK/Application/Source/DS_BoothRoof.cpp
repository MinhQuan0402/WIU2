#include "DS_BoothRoof.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_BoothRoof::DS_BoothRoof() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_BoothRoof::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.Translate(0, 11.2144, 0);
	m_transform.ScaleBy(3, 1, 1.5);
	rb = addBoxCollider(this, 12.7474 * m_transform.m_scale.x, 4.80242 * m_transform.m_scale.y, 12.7441 * m_transform.m_scale.z, mat);
}

void DS_BoothRoof::Update()
{
}

void DS_BoothRoof::Render(Scene& scene)
{
}
