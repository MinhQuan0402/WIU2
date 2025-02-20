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
	rb = addBoxCollider(this, 12.7474, 4.80242, 12.7441, mat);
}

void DS_BoothRoof::Update()
{
}

void DS_BoothRoof::Render(Scene& scene)
{
}
