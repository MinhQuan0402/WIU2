#include "DS_Pillar.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Pillar::DS_Pillar() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Pillar::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	rb = addBoxCollider(this, 2.5, 10, 2.5, mat);
}

void DS_Pillar::Update()
{
}

void DS_Pillar::Render(Scene& scene)
{
}
