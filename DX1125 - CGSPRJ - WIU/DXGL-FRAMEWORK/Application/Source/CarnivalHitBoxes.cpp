#include "CarnivalHitBoxes.h"
#include "RigidBody.h"
#include "carnivalroaming.h"
#include "GameObjectManager.h"

CarnivalHitBoxes::CarnivalHitBoxes()
{
	rbGOType = STATIC;
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
	//scene.RenderMesh(scene.meshList[carnivalroaming::GEO_CUBE], false, m_transform);
}

void CarnivalHitBoxes::Update()
{
}
