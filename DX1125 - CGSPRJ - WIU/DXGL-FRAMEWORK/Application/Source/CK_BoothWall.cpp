#include "CK_BoothWall.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"
#include "RigidBody.h"

CK_BoothWall::CK_BoothWall() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_BoothWall::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	m_transform.Translate(0, 4.39147, -6.06621);
	rb = addBoxCollider(this, 11.8194, 8.90606, 0.314395, mat);
}

void CK_BoothWall::Update()
{
}

void CK_BoothWall::Render(Scene& scene)
{
}
