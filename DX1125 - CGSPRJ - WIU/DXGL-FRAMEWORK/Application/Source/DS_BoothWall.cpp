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
	m_transform.Translate(0, 4.39147, -6.06621);
	rb = addBoxCollider(this, 11.8194, 8.90606, 0.314395, mat);
}

void DS_BoothWall::Update()
{
}

void DS_BoothWall::Render(Scene& scene)
{
}
