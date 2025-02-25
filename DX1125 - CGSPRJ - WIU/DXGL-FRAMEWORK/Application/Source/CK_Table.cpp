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
	mat.m_friction = 0.8;
	m_transform.ScaleBy(0.0412, 0.0412, 0.0412);
	rb = addBoxCollider(this, 2.9, 2.84, 2.05, mat);
}

void CK_Table::Update()
{
}

void CK_Table::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneCanKnockdown::GEO_TABLE], scene.enableLight, m_transform);
}
