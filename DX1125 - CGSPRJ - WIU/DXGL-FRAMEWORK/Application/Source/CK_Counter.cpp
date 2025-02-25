#include "CK_Counter.h"
#include "GameObjectManager.h"
#include "SceneCanKnockdown.h"
#include "RigidBody.h"
#include "MeshManager.h"

CK_Counter::CK_Counter() {
	
	GameObjectManager::GetInstance()->addItem(this);
}

void CK_Counter::Start()
{
	PhysicsMaterial mat;
	mat.m_mass = 0;
	mat.m_friction = 0.8;
	rb = addBoxCollider(this, 11.4612, 2.65, 1, mat);
}

void CK_Counter::Update()
{
}

void CK_Counter::Render(Scene& scene)
{
	scene.RenderMesh(MeshManager::GetInstance()->meshList[MeshManager::GEO_CK_COUNTER], scene.enableLight, m_transform);
}
