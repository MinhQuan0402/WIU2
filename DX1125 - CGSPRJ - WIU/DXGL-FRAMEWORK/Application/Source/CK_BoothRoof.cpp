#include "CK_BoothRoof.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_BoothRoof::CK_BoothRoof() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_BoothRoof::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.Translate(0, 11.2144, 0);
	rb = addBoxCollider(this, 12.7474, 4.80242, 12.7441, mat);
}

void CK_BoothRoof::Update()
{
}

void CK_BoothRoof::Render(Scene& scene)
{
}
