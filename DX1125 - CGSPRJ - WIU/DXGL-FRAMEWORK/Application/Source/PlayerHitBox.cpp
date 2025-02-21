#include "PlayerHitBox.h"
#include "RigidBody.h"
#include "carnivalroaming.h"
#include "GameObjectManager.h"
#include "Scene.h"


PlayerHitBox::PlayerHitBox()
{
	rbGOType = DYNAMIC;
	GameObjectManager::GetInstance()->addItem(this);
}

PlayerHitBox::~PlayerHitBox()
{
}

void PlayerHitBox::Start()
{
	
}

void PlayerHitBox::Render(Scene& scene)
{
	//scene.RenderMesh(scene.meshList[carnivalroaming::GEO_CUBE], false, m_transform);
}

void PlayerHitBox::Update()
{
}


