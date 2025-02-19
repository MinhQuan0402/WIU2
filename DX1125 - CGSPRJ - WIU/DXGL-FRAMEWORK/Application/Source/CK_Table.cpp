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
	rb = addBoxCollider(this, 10, 10, 10, mat);
}

void CK_Table::Update()
{
}

void CK_Table::Render(Scene& scene)
{
}
