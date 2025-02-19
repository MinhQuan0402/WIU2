#include "CK_Ball.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Ball::CK_Ball() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Ball::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;
	rb = addSphereCollider(this, 0.25, mat);
}

void CK_Ball::Update()
{
}

void CK_Ball::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneCanKnockdown::GEO_BALL], scene.enableLight, m_transform);
}
