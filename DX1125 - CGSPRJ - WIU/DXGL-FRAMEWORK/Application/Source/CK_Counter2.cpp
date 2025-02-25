#include "CK_Counter2.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"
#include "RigidBody.h"
#include "MeshManager.h"

CK_Counter2::CK_Counter2() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Counter2::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	mat.m_friction = 0.8;
	rb = addBoxCollider(this, 1, 2.65, 11, mat);
}

void CK_Counter2::Update()
{
}

void CK_Counter2::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CK_COUNTER], scene.enableLight, m_transform);
}
