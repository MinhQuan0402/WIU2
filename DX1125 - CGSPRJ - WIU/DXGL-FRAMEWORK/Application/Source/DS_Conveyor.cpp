#include "DS_Conveyor.h"
#include "GameObjectManager.h"
#include "SceneDuckShooting.h"

DS_Conveyor::DS_Conveyor() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void DS_Conveyor::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.Translate(0, 2.17004 * 0.5, -4.78361);
	rb = addBoxCollider(this, 24.3538, 2.17004, 1.94702, mat);
}

void DS_Conveyor::Update()
{
}

void DS_Conveyor::Render(Scene& scene)
{
}
