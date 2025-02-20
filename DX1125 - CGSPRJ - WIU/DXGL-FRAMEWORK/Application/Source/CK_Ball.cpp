#include "CK_Ball.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"

CK_Ball::CK_Ball() {
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Ball::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.3f;
	mat.m_friction = 1.f;
	mat.m_bounciness = 1.f;
	rb = addSphereCollider(this, 0.25, mat);
}

void CK_Ball::Update()
{
}

void CK_Ball::Render(Scene& scene)
{
	scene.RenderRigidMesh(scene.meshList[SceneCanKnockdown::GEO_BALL], scene.enableLight, m_transform, rb);
}
