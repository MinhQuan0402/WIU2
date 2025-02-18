#include "CarnivalHitBoxes.h"
#include "RigidBody.h"
#include "carnivalroaming.h"
#include "GameObjectManager.h"

CarnivalHitBoxes::CarnivalHitBoxes()
{
	PhysicsMaterial mat;
	mat.m_mass = 0.0f;
	rb = addSphereCollider(this, 50, mat);
	GameObjectManager::GetInstance()->addItem(this);
}

CarnivalHitBoxes::~CarnivalHitBoxes()
{
}

void CarnivalHitBoxes::Start()
{
}

void CarnivalHitBoxes::Render(Scene& scene)
{
	scene.RenderMesh(scene.meshList[carnivalroaming::GEO_CUBE], false, m_transform);
}

void CarnivalHitBoxes::Update()
{
}
