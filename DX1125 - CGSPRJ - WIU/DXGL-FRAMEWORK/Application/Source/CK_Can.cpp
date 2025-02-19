#include "CK_Can.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Can::CK_Can() {
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Can::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	rb = addCylinderCollider(this, 0.67, 0.48, mat);
}

void CK_Can::Update()
{
}

void CK_Can::Render(Scene& scene)
{
	
}
