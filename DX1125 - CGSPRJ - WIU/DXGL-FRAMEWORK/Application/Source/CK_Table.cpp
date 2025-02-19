#include "CK_Table.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Table::CK_Table() 
{
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Table::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	rb = addBoxCollider(this, 2.6, 2.84, 1.8, mat);
}

void CK_Table::Update()
{
}

void CK_Table::Render(Scene& scene)
{
}
