#include "CK_Ball.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Ball::CK_Ball() {
	PhysicsMaterial mat;
	mat.m_mass = 1.0f;
	mat.m_bounciness = 0.6f;
	mat.m_friction = 0.5f;
	rb = addSphereCollider(this, 1.f, (PhysicsMaterial&)mat);
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Ball::Start()
{
}

void CK_Ball::Update()
{
}

void CK_Ball::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[SceneCanKnockdown::GEO_SPHERE], false, m_transform);
}
